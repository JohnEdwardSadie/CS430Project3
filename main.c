#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


//John E. Sadie
//CS430 Computer Graphics
//Project 3 - Illumination

/*
Description:
In this project you will color objects based on the shading model
we discussed in class.
*/

/*
Procedure:
I had a very hard time with this project. We must first completely understand the mathematical equations
that Dr. Palmer had provided us with. Once we've conceptually understood, we had to utilize them and implement them within our code.
At first, I simply continued to work on my parser to be able to handle all of the new contents of the JSON file.
That part was not difficult. When I began needing to creating these inline functions for vector math. I used this website: http://csis.pace.edu/~marchese/CG_Rev/Lect10New/cg_l10new.htm
to further help me get clarity. I had a lot of help from other classmates on certain mistakes I kept running into. For example, I simply could not figure how to create both the,
AngularAttenuation() and RadialAttenuation() functions. In conclusion, this was by far the hardest project I've done yet. As you will see, this is still not finished. As you will see,
sphere looks spotty. I'm not quite sure if that is a memory leak or not.
*/

//declaration list of variables
//From the JSON file
typedef struct{
        char type;
        double diffuseColor[3];
        double specularColor[3];
        double color[3];
        double position[3];
        double normal[3];
        double radius;
        double width, height;
        unsigned char r,g,b;


} Scene;

typedef struct{
        char type;
        double color[3];
        double position[3];
        double direction[3];
        double theta;
        double radiala2;
        double radiala1;
        double radiala0;
        double angulara0;

} Light;

Scene r,g,b;
Scene camera;
Scene *scene;
Light *lightScene;
Scene *PixelBuffer;
Scene closestS;

int line = 1;
int cameraOne;
int incrementObject;
int lastIndex = 0;
int lastIndexLight = 0;
Pi = 3.14;




//creating a square operator
static inline double Sqr(double n){
	return n*n;
}

static inline void normalize(double* v){
    //getting the length of the vector
    double length = sqrt(Sqr(v[0]) + Sqr(v[1]) + Sqr(v[2]));

    //normalizing our vector
    //turning it into a unit vector
    v[0] = v[0]/length;
    v[1] = v[1]/length;
    v[2] = v[2]/length;
}
/*The following equations were provided by Palmer in-class(hand-written)*/
//Sources: http://csis.pace.edu/~marchese/CG_Rev/Lect10New/cg_l10new.htm
static inline double VectorLength(double* v){
    return sqrt(Sqr(v[0])+Sqr(v[1])+Sqr(v[2]));

}
static inline void VectorScaling(double scaler, double* vector, double* ans){
    ans[0] = scaler * vector[0];
    ans[1] = scaler * vector[1];
    ans[2] = scaler * vector[2];
}

static inline void VectorSubtraction(double* a, double* b, double* ans){
    ans[0] = a[0] - b[0];
    ans[1] = a[1] - b[1];
    ans[2] = a[2] - b[2];
}

static inline Scene ObjectProperty(Scene old, Scene og){
    old.type = og.type;
    //diffuse
    old.diffuseColor[0] = og.diffuseColor[0];
    old.diffuseColor[1] = og.diffuseColor[1];
    old.diffuseColor[2] = og.diffuseColor[2];
    //specular
    old.specularColor[0] = og.specularColor[0];
    if(og.type == 'p'){
        old.normal[1] = og.normal[1];
        old.normal[0] = og.normal[0];
        old.normal[2] = og.normal[3];
    }
    if(og.type == 's') old.radius = og.radius;
    return old;
}
//clamps the color(restrict the value)
static inline double clamp(double n){
    if(n > 1){
        n = 1;
    }
    else if(n < 0){
        n = 0;
    }
    else{
    return n;
    }
}
static inline double DotProduct(double* a, double* b){
    return ((a[0]*b[0] + a[1]*b[1] + a[2]*b[2]));
}
static inline void VectorReflection(double* normal, double* R, double* reflection){
    double a[3] = {0,0,0};
    double nv = DotProduct(normal, R);

    VectorScaling(nv,normal, a);
    VectorScaling(2,a,a);
    VectorSubtraction(R,a,reflection);
}

//Source: http://csis.pace.edu/~marchese/CG_Rev/Lect10New/cg_l10new.htm
static inline double AngularAttenuation(Light a, double* NewRd, double Pi){
    double SpotlightToObject[3];
    SpotlightToObject[0] = (1 * NewRd[0]);
    SpotlightToObject[1] = (1 * NewRd[1]);
    SpotlightToObject[2] = (1 * NewRd[2]);
    double cos = (((a.direction[0]*SpotlightToObject[0]) +(a.direction[1]*SpotlightToObject[1]) + (a.direction[2]*SpotlightToObject[2])));
    return pow(cos, a.angulara0);
    free(SpotlightToObject);
}

//Source: http://csis.pace.edu/~marchese/CG_Rev/Lect10New/cg_l10new.htm
static inline double RadialAttenuation(Light a, double* NewRo){
    double* VL = malloc(sizeof(double)*3);
    VectorSubtraction(a.position, NewRo, VL);
    double den = (a.radiala2 * Sqr(VectorLength(VL))) + (a.radiala1 * VectorLength(VL)) + (a.radiala0);
    return 1/den;
    free(VL);
}
//Source: http://csis.pace.edu/~marchese/CG_Rev/Lect10New/cg_l10new.htm
static inline double IlluminateDiffuse(int index,double* diffusecolor, Light objectlight, double NL){
    if(NL <= 0) return 0;
    return diffusecolor[index]*objectlight.color[index]*NL;
}
/*
   Calculate normal vector using cross product:
n = PxQ

nx = PyQz - PzQy
ny = PzQx -PxQz
nz = PxQy - PyQx

Create a light vector
�       Select a point on the polygon surface and position for the light source
     A polygon vertex may be selected or a position inside the polygon.
*/
//Source: http://csis.pace.edu/~marchese/CG_Rev/Lect10New/cg_l10new.htm
static inline double IlluminateSpecular(int index, double* SpecularColor, Light objectlight, double NL, double VR, double ns){
    if(VR <= 0){
    return 0;
    }
    if(NL <= 0){
    return 0;
    }

    return SpecularColor[index]*objectlight.color[index]*pow(VR,ns);

}

/*END EQUATION*/

//rayCast function
//implements pseudocode provided by Palmer
double sphereIntersection(double* Ro, double* Rd, double* position, double radius){
    double a, b, c;
    normalize(Rd);

    //sphere intersection equation provided by Dr. Palmer
     a = Sqr(Rd[0])+Sqr(Rd[1])+Sqr(Rd[2]);
     b = 2*(Rd[0]*(Ro[0]-position[0]) + Rd[1]*(Ro[1]-position[1]) + Rd[2]*(Ro[2]-position[2]));
     c = (Sqr((Ro[0]-position[0])) + Sqr((Ro[1]-position[1])) + Sqr((Ro[2]-position[2])) - Sqr(radius));

    double t0;
    t0 = ((-b - sqrt(Sqr(b) - 4.0*c*a))/(2.0*a));

    if(t0 > 0.0){
        return t0;
    }

    return -1;
}

//planeIntersection function
//Source: http://www.siggraph.org/education/materials/HyperGraph/raytrace/rayplane_intersection.htm
double planeIntersection(double* Ro, double* Rd, double* position, double* normal){
    normalize(normal);
    normalize(Rd);

    //The length from camera to plane
    double d = -(normal[0]*position[0] + normal[1]*position[1] + normal[2]*position[2]);

    //denominator
    double denominator = (normal[0]*Rd[0] + normal[1]*Rd[1] + normal[2]*Rd[2]);

    //plane intersection equation provided by Dr. Palmer
    //t = -(AX0 + BY0 + CZ0 + D) / (AXd + BYd + CZd)
    double t = -(normal[0]*Ro[0] + normal[1]*Ro[1] + normal[2]*Ro[2] + d)/(normal[0]*Rd[0] + normal[1]*Rd[1] + normal[2]*Rd[2]);

    //else
        return t;
}

void rayCast(double N, double M){
    //instantiating variables
    int index;
    int i = 0;
    int j = 0;
    int u,t;
    double Ro[3] = {0, 0, 0};
    double c[3] = {0, 0, 0};

    double w = camera.width;
    double h = camera.height;
    double PixelWidth = w/N;
    double PixelHeight = h/M;

    for(i=0; i<M; i+=1){
        for(j=0; j<N; j+=1){
            double p_y = (c[1] - h/2.0 + PixelHeight*(i + 0.5));
            double p_x = (c[0] - w/2.0 + PixelWidth*(j + 0.5));
            double p_z = 1; //z-coordinate view plane
            double Rd[3] = {p_x, p_y, p_z};
            //Normalization of Rd
            normalize(Rd);

            double closestT =  999999; //closest point to the camera
            double closesetC[3] = {0,0,0};  //what ever colro closestT is, that is colorT


            Scene closestObject;
            closestObject.type = NULL;
            for(index=0; index<=lastIndex; index++){
                double t = 0;
                   //Shoot function from the pseudocode provided by Palmer
                if(scene[index].type == 's'){
                        t = sphereIntersection(Ro, Rd, scene[index].position, scene[index].radius);
                }
                //Shoot function from the pseudocode provided by Palmer
                if(scene[index].type == 'p'){
                        t = planeIntersection(Ro, Rd, scene[index].position, scene[index].normal);
                }
                if(t > 0 && t < closestT){ //setting closest point to t
                    closestT = t;
                    //setting closest c to the color
                    closesetC[0] = scene[index].diffuseColor[0];
                    closesetC[1] = scene[index].diffuseColor[1];
                    closesetC[2] = scene[index].diffuseColor[2];
                    closestObject = ObjectProperty(closestObject, scene[index]);
                }
            }
                    double color[3] = {0,0,0};
                    color[0] = 0;
                    color[1] = 0;
                    color[2] = 0;

            if(closestT > 0 && closestT != 99999999999){//INFINITY
                    double NewRo[3] = {0,0,0}; //NewRo is vector from Light towards Object.  Current Pixel location of intersection.
                    NewRo[0] = (closestT * Rd[0]) + Ro[0];
                    NewRo[1] = (closestT * Rd[1]) + Ro[1];
                    NewRo[2] = (closestT * Rd[2]) + Ro[2];
                for(u=0; u<=lastIndexLight; u++){
                    double NewRd[3] = {0,0,0};

                    VectorSubtraction(lightScene[u].position, NewRo, NewRd);

                    double object_to_light[3] = {0,0,0};
                    object_to_light[0] = NewRd[0];
                    object_to_light[1] = NewRd[1];
                    object_to_light[2] = NewRd[2];

                    double lightDistance = sqrt(Sqr(NewRo[0]-NewRd[0])+ Sqr(NewRo[1]-NewRd[1])+ Sqr(NewRo[2]-NewRd[2]));
                    closestS.type = NULL;


                    double closestShadow = 99999999999; //INFINITY
                    for(t=0; t<=lastIndex; t++){
                        double shadow = 0;

                        if(scene[t].type == 's'){
                                shadow = sphereIntersection(NewRo, NewRd, scene[t].position, scene[t].radius);
                        }
                        if(scene[t].type == 'p'){
                                shadow = planeIntersection(NewRo, NewRd, scene[t].position, scene[t].normal);
                        }
                        if(shadow > 0.0 && shadow < closestShadow){
                            closestShadow = shadow;
                            closestS = ObjectProperty(closestS, scene[t]);
                        }
                    }
                    //Casting light where there is no shadow
                    if(closestS.type == NULL){
                        double objectNormal[3] = {0,0,0};
                        if(closestObject.type == 's') {
                                VectorSubtraction(closestObject.position, NewRo, objectNormal);
                                }
                        if(closestObject.type == 'p'){
                            objectNormal[0] = closestObject.normal[0];
                            objectNormal[1] = closestObject.normal[1];
                            objectNormal[2] = closestObject.normal[2];
                        }
                        double *vectorDirection = NewRd;
                        double vectorReflection[3] = {0,0,0};
                        VectorReflection(objectNormal, vectorDirection, vectorReflection);
                        normalize(vectorReflection);
                        //Camera sees an object
                        double objectseen[3] = {0,0,0};
                        objectseen[0] = (-1 * Rd[0]);
                        objectseen[1] = (-1 * Rd[1]);
                        objectseen[2] = (-1 * Rd[2]);
                        normalize(objectseen);
                        //current diffuse and specular
                        double* Diffuse = closestObject.diffuseColor;
                        double* Specular = closestObject.specularColor;
                        //Palmer suggested 20
                        //For shininess
                        double ns = 20;

                        normalize(objectNormal);
                        double VR = DotProduct(objectseen, vectorReflection);
                        double NL = DotProduct(objectNormal, vectorDirection);

                        //Acquiring the value for the color to be drawn
                        //Equation provided by Palmer in-class(handwritten)
                        color[0] += RadialAttenuation(lightScene[u], NewRo) * AngularAttenuation(lightScene[u],NewRd,Pi) *
                            (IlluminateDiffuse(0,Diffuse,lightScene[u],NL) + (IlluminateSpecular(0,Specular,lightScene[u],NL,VR,ns)));

                        color[1] += RadialAttenuation(lightScene[u], NewRo) * AngularAttenuation(lightScene[u],NewRd,Pi) *
                                        (IlluminateDiffuse(1,Diffuse,lightScene[u],NL) + (IlluminateSpecular(1,Specular,lightScene[u],NL,VR,ns)));

                        color[2] += RadialAttenuation(lightScene[u], NewRo) * AngularAttenuation(lightScene[u],NewRd,Pi) *
                                        (IlluminateDiffuse(2,Diffuse,lightScene[u],NL) + (IlluminateSpecular(2,Specular,lightScene[u],NL,VR,ns)));
                    }
                        }
                int pos = (int)((M-i-1)*N + j);
                PixelBuffer[pos].r = (char)(clamp(color[0])*255);
                PixelBuffer[pos].g = (char)(clamp(color[1])*255);
                PixelBuffer[pos].b = (char)(clamp(color[2])*255);
            }
            //Once the light has been casted
            //The rest should be black
            else{
                int pos = (int)((M-i-1)*N + j);
                PixelBuffer[pos].r = (char)clamp(color[0])*255;
                PixelBuffer[pos].g = (char)clamp(color[1])*255;
                PixelBuffer[pos].b = (char)clamp(color[2])*255;
            }
        }
    }
}



// next_c() wraps the getc() function and provides error checking and line
// number maintenance
int next_c(FILE* json) {
  int c = fgetc(json);
#ifdef DEBUG
  printf("next_c: '%c'\n", c);
#endif
  if (c == '\n') {
    line += 1;
  }
  if (c == EOF) {
    fprintf(stderr, "Error: Unexpected end of file on line number %d.\n", line);
    exit(1);
  }
  return c;
}


// expect_c() checks that the next character is d.  If it is not it emits
// an error.
void expect_c(FILE* json, int d) {
  int c = next_c(json);
  if (c == d) return;
  fprintf(stderr, "Error: Expected '%c' on line %d.\n", d, line);
  exit(1);
}


// skip_ws() skips white space in the file.
void skip_ws(FILE* json) {
  int c = next_c(json);
  while (isspace(c)) {
    c = next_c(json);
  }
  ungetc(c, json);
}


// next_string() gets the next string from the file handle and emits an error
// if a string can not be obtained.
char* next_string(FILE* json) {
  char buffer[129];
  int c = next_c(json);
  if (c != '"') {
    fprintf(stderr, "Error: Expected string on line %d.\n", line);
    exit(1);
  }
  c = next_c(json);
  int i = 0;
  while (c != '"') {
    if (i >= 128) {
      fprintf(stderr, "Error: Strings longer than 128 characters in length are not supported.\n");
      exit(1);
    }
    if (c == '\\') {
      fprintf(stderr, "Error: Strings with escape codes are not supported.\n");
      exit(1);
    }
    if (c < 32 || c > 126) {
      fprintf(stderr, "Error: Strings may contain only ascii characters.\n");
      exit(1);
    }
    buffer[i] = c;
    i += 1;
    c = next_c(json);
  }
  buffer[i] = 0;
  return strdup(buffer);
}

double next_number(FILE* json) {
  double value;
  fscanf(json, "%lf", &value); //altered from %f to read doubles
  // Error check this..
  return value;
}

double* next_vector(FILE* json) {
  double* v = malloc(3*sizeof(double));
  expect_c(json, '[');
  skip_ws(json);
  v[0] = next_number(json);
  skip_ws(json);
  expect_c(json, ',');
  skip_ws(json);
  v[1] = next_number(json);
  skip_ws(json);
  expect_c(json, ',');
  skip_ws(json);
  v[2] = next_number(json);
  skip_ws(json);
  expect_c(json, ']');
  return v;
}


//read_scene function
//Read's in our json file
//populates our object array
//saves data for future usage
void read_scene(char* filename) {
    int index = -1;
    int c;
    char Object;

    FILE* json = fopen(filename, "r");

  if (json == NULL) {
    fprintf(stderr, "Error: Could not open file \"%s\"\n", filename);
    exit(1);
  }

  skip_ws(json);

  // Find the beginning of the list
  expect_c(json, '[');

  skip_ws(json);

  // Find the objects

  while (1) {
    c = fgetc(json);
    if (c == ']') {
       printf("%d", line);
      fprintf(stderr, "Error: This is the worst scene file EVER.\n");
      fclose(json);
      return;
    }
    if (c == '{') {
      skip_ws(json);

      // Parse the object
    char* key = next_string(json);
    if (strcmp(key, "type") != 0) {

        fprintf(stderr, "Error: Expected \"type\" key on line number %d.\n", line);
        exit(1);
      }
    skip_ws(json);
    expect_c(json, ':');
    skip_ws(json);
    char* value = next_string(json);
    if (strcmp(value, "camera") == 0) {
        cameraOne += 1;
        Object = 'c';
      }
      //What object are we looking at
      //String compare of value and sphere
    else if (strcmp(value, "sphere") == 0) {
        incrementObject += 1;
        Object = 's';
        scene[lastIndex].type = 's';

      }
       //String compare of value and plane
    else if (strcmp(value, "plane") == 0) {
        incrementObject += 1;
        Object = 'p';
        scene[lastIndex].type = 'p';
      }
      //String compare of value and light
    else if (strcmp(value, "light") == 0) {
        incrementObject += 1;
        Object = 'l';
        lightScene[lastIndexLight].type = 'l';
    }

    else {
        fprintf(stderr, "Error: Unknown type, \"%s\", on line number %d.\n", value, line);
        exit(1);
      }
    skip_ws(json);
    int incrementCamera = 0;
	int incrementObject = 0;
    while (1) {
        c = next_c(json);
        if (c == '}') {
            // stop parsing this object
            break;
        }
        else if (c == ',') {
            // read another field
            skip_ws(json);
            char* key = next_string(json);
            skip_ws(json);
            expect_c(json, ':');
            skip_ws(json);
            if ((strcmp(key, "width") == 0)     ||
                (strcmp(key, "height") == 0)    ||
                (strcmp(key, "radius") == 0)    ||
                (strcmp(key, "radial-a2") == 0) ||
                (strcmp(key, "radial-a1") == 0)   ||
                (strcmp(key, "radial-a0") == 0)   ||
                (strcmp(key, "angular-a0") == 0) ||
                (strcmp(key, "theta") == 0))
                {
                double value = next_number(json);
                //*populating object array with our json contents*//
                if((strcmp(key, "width") == 0)){
                    camera.width = value;
                    incrementCamera+=1;
                }
                else if((strcmp(key, "height") == 0)){
                    camera.height = value;
                    incrementCamera +=1;
                }
                else if((strcmp(key, "radius") == 0)){
                    scene[lastIndex].radius = value;
                }
                else if((strcmp(key, "radial-a2") == 0)){
                    lightScene[lastIndexLight].radiala2 = value;
                }
                else if((strcmp(key, "radial-a1") == 0)){
                    lightScene[lastIndexLight].radiala1 = value;
                }
                else if((strcmp(key, "radial-a0") == 0)){
                    lightScene[lastIndexLight].radiala0 = value;
                }
                else if((strcmp(key, "angular-a0") == 0)){
                    lightScene[lastIndexLight].angulara0 = value;
                }
                else if((strcmp(key, "theta") == 0)){
                    lightScene[lastIndexLight].theta = value;
                }

            }
            /*
            //populating object array with our json contents
            if (strcmp(key, "width") == 0) {
                scene.width = next_number(json);
            } else if (strcmp(key, "height") == 0){
                scene.height = next_number(json);
            } else if (strcmp(key, "radius") == 0){
                scene.object[index].radius = next_number(json);
            } else if ((strcmp(key, "color") == 0)){
                scene.object[index].color = next_vector(json);
            } else if (strcmp(key, "position") == 0){
                scene.object[index].position = next_vector(json);
            } else if (strcmp(key, "normal") == 0) {
                scene.object[index].normal = next_vector(json);
            }
            */
            else if ((strcmp(key, "color") == 0) ||
                     (strcmp(key, "position") == 0) ||
                     (strcmp(key, "normal") == 0) ||
                     (strcmp(key, "direction") == 0) ||
                     (strcmp(key, "specular_color") == 0) ||
                     (strcmp(key, "diffuse_color") == 0))
                        {
                double* value = next_vector(json);
                //assigning value of color
                if((strcmp(key, "color") == 0)){
                        //set color value of scene
                    if(Object != 'l'){
                        scene[lastIndex].color[0] = value[0];
                        scene[lastIndex].color[1] = value[1];
                        scene[lastIndex].color[2] = value[2];

                    }
                    //set color value of lightScene
                    else if(Object == 'l'){
                        lightScene[lastIndexLight].color[0] = value[0];
                        lightScene[lastIndexLight].color[1] = value[1];
                        lightScene[lastIndexLight].color[2] = value[2];
                    }
                }
                 //assigning value of diffuseColor
                else if((strcmp(key, "diffuse_color") == 0)){
                    scene[lastIndex].diffuseColor[0] = value[0];
                    scene[lastIndex].diffuseColor[1] = value[1];
                    scene[lastIndex].diffuseColor[2] = value[2];

                }
                //assigning value of specularColor
               else if((strcmp(key, "specular_color") == 0)){
                    scene[lastIndex].specularColor[0] = value[0];
                    scene[lastIndex].specularColor[1] = value[1];
                    scene[lastIndex].specularColor[2] = value[2];

                }
                //assigning value of position
               else if((strcmp(key, "position") == 0)){
                    //setting value of positon in scene
                    if(Object != 'l'){
                        scene[lastIndex].position[0] = value[0];
                        scene[lastIndex].position[1] = value[1];
                        scene[lastIndex].position[2] = value[2];

                    }
                    //setting value of of position in lightScene
                   else if(Object == 'l'){
                        lightScene[lastIndexLight].position[0] = value[0];
                        lightScene[lastIndexLight].position[1] = value[1];
                        lightScene[lastIndexLight].position[2] = value[2];

                    }
                }
                //assigning value of normal
               else if((strcmp(key, "normal") == 0)){
                    scene[lastIndex].normal[0] = value[0];
                    scene[lastIndex].normal[1] = value[1];
                    scene[lastIndex].normal[2] = value[2];

                }
                //assigning value of direction
               else if((strcmp(key, "direction") == 0)){
                    lightScene[lastIndexLight].direction[0] = value[0];
                    lightScene[lastIndexLight].direction[1] = value[1];
                    lightScene[lastIndexLight].direction[2] = value[2];
                }

            }
            else {
                fprintf(stderr, "Error: Unknown property, \"%s\", on line %d.\n",
                key, line);

            }
            skip_ws(json);
        }
        else {
            fprintf(stderr, "Error: Unexpected value on line %d\n", line);
            exit(1);
        }
      }
    skip_ws(json);
    c = next_c(json);
    if (c == ',') {
        //Error checking
        skip_ws(json);
        if(Object != 'c'){
            if(Object != 'l'){
                lastIndex++;
            }
            if(Object == 'l'){
                lastIndexLight++;
            }
        }
        if(Object == 'c'){
            if(incrementCamera != 2){
                exit(1);
            }
        incrementCamera = 0;
        }
    }
    else if (c == ']') {
        //Iterated through all objects
        fclose(json);
        return;
    }
    else {
        fprintf(stderr, "Error: Expecting ',' or ']' on line %d.\n", line);
        exit(1);
      }
    }
  }
}
/*This was to testing to see if I've properly acquired
the contents within the JSON by populating my array of objects.
I then printed it out to the terminal.
*/

/*
void printScene(){
    int index = 0;
    while(scene.object[index].color != NULL){
    printf("object: %d\n", index);
    printf("type %s\n", scene.object[index].type);
    printf("color: %f %f %f\n", scene.object[index].color[0],scene.object[index].color[1],scene.object[index].color[2]);
    printf("position: %f %f %f\n", scene.object[index].position[0],scene.object[index].position[1],scene.object[index].position[2]);
    if(scene.object[index].normal != NULL){
    printf("normal: %f %f %f\n", scene.object[index].normal[0],scene.object[index].normal[1],scene.object[index].normal[2]);
    }
    else{
    printf("radius: %f\n", scene.object[index].radius);
    }
    index++;
    }
}*/


//Customized write function from project 1
int write(int w, int h, FILE* outputFile){
    int i;

    FILE *fp;

    char BufferSize[2] = {'P', '6'};
    int height = h;
    int width = w;

    fp = fopen(outputFile, "wb");
    fwrite(BufferSize, sizeof(BufferSize), sizeof(BufferSize)-1, fp);

    fprintf(fp,"\n%d %d", width, height);
    fprintf(fp,"\n%d", 255);
    fprintf(fp,"\n");
            for (i=0; i < width*height; i++){
                fwrite(&PixelBuffer[i].r, 1, 1, fp);
                fwrite(&PixelBuffer[i].g, 1, 1, fp);
                fwrite(&PixelBuffer[i].b, 1, 1, fp);
            }
    return 0;
}


//Main function
int main(int argc, char** argv) {

    //memory allocation
    scene = malloc(sizeof(Scene)*128);
    lightScene = malloc(sizeof(Light)*128);

    //ascii to integer.
    double N = (double)atoi(argv[1]);
    double M = (double)atoi(argv[2]);
    //referencing the type for memory allocation
    PixelBuffer = (Scene*)malloc(sizeof(Scene)* N * M);
    //treating PixelBuffer as a series of bytes
    memset(PixelBuffer, 0, 3*N * M);

    read_scene(argv[3]);
    rayCast(N, M);

    //Utilizing our command line arguments
    write(N, M, argv[4]);

    return 0;
}
