#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


//John E. Sadie
//CS430 Computer Graphics
//Project 3 - Illumination

/*

*/



//declaration list of variables
//From the JSON file
typedef struct{
        char type;
        double *color;
        double *position;
        double *normal;
        double radius;
        double width, height;
        unsigned char r,g,b;

}Scene;

typedef struct{
        char type;
        double *color;
        double *position;
        double radiala2;
}Light;

int line = 1;
int cameraOne;
int incrementObject;
int lastIndex = 0;
int lastIndexLight = 0;

Scene r, g, b;
Scene camera;
Scene *scene;
Light *lightScene;
Scene *PixelBuffer;


//creating a square operator
static inline double sqr(double n){
    return n*n;
}

//sphereIntersection function
//Source: http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter1.htm
double sphereIntersection(double *Ro, double *Rd, double *position, double radius){
    double a,b,c;
    //sphere intersection equation provided by Dr. Palmer
    a = sqr(Rd[0]) + sqr(Rd[1]) + sqr(Rd[2]);
    b = 2*(Rd[0]*(Ro[0]-position[0])+ Rd[0]*(Ro[1]-position[1])+Rd[2]*(Ro[2]-position[2]));
    c = sqr(Ro[0]-position[0]) + sqr(Ro[1]-position[1]) + sqr(Ro[2]-position[2]) - sqr(radius);

    double t0, t1;

    t0 = (-b - sqrt(sqr(b) - 4*(a*c)))/2*a;
    t1 = (-b + sqrt(sqr(b) - 4*(a*c)))/2*a;

    //discriminant
    double dis = sqr(b) - 4*(a*c);

    //Checking if the discriminant is 0
    //If so, there was no intersection
    if(dis = 0){
        return -1;
    }
    //The intersection is behind the camera
    //Don't render
    if(t0 < 0){
        return t1;
    }
    //Render the object
    return t0;

}

static inline double normalize(double *v){
    //getting the length of the vector
    double length = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));

    //normalizing our vector
    //turning it into a unit vector
    v[0] = v[0]/length;
    v[1] = v[1]/length;
    v[2] = v[2]/length;

}

//planeIntersection function
//Source: http://www.siggraph.org/education/materials/HyperGraph/raytrace/rayplane_intersection.htm
double planeIntersection(double *Ro, double *Rd, double *position, double *normal){
    normalize(normal);

    //The length from camera to plane
    double d = -(normal[0]*position[0] + normal[1]*position[1] + normal[2]*position[2]);

    //denominator
    double denominator = (normal[0]*Rd[0] + normal[1]*Rd[1] + normal[2]*Rd[2]);
    if(denominator == 0)
        return -1;
    //plane intersection equation provided by Dr. Palmer
    //t = -(AX0 + BY0 + CZ0 + D) / (AXd + BYd + CZd)
    double t = -(normal[0]*Ro[0] + normal[1]*Ro[1] + normal[2]*Ro[2] + d)/(normal[1]*Rd[0] + normal[1]*Rd[1] + normal[2]*Rd[2]);
    //else
        return t;
}

//rayCast function
//implements pseudocode provided by Palmer
void rayCast(double N, double M){
    //instantiating variables
    int index;
    int i = 0;
    int j = 0;
    double Ro[3] = {0.0,0};
    double c[3] = {0, 0, 0};
    double w = camera.width;
    double h = camera.height;
    double pixelHeight = h/M;
    double pixelWidth = w/N;

    for(i=0; i<M; i+=1){
        for(j=0; j<N; j+=1){
            double p_y = (c[1] - h/2.0 + pixelHeight*(i + 0.5));
            double p_x = (c[0] - w/2.0 + pixelWidth*(j + 0.5));
            double p_z = 1; //z-coordinate view plane
            double Rd[3] = {p_x, p_y, p_z};
            //Normalization of Rd
            normalize(Rd);


            double closestT = 999999; //closest point to the camera
            double *closestC; //what ever colro closestT is, that is colorT
            for(index=0; index<=lastIndex; index+=sizeof(Scene)){
                double t = 0;
                //Shoot function from the pseudocode provided by Palmer
                if(scene[index].type == 's'){
                        t = sphereIntersection(Ro, Rd, scene[index].position, scene[index].radius);
                }
                 //Shoot function from the pseudocode provided by Palmer
                if(scene[index].type == 'p'){
                        t = planeIntersection(Ro, Rd, scene[index].position, scene[index].normal);
                }
                if(t > 0 && t < closestT){
                    closestT = t; //setting closest point to t
                    closestC = scene[index].color; //setting closest c to the color
                }
                if(closestT > 0 && closestT != 999999){

                    double r = closestC[0] * 255;
                    double g = closestC[1] * 255;
                    double b = closestC[2] * 255;
                    //Type casting
                    int int_r = (int) r;
                    int int_g = (int) g;
                    int int_b = (int) b;

                    int position = (int)((M - i - 1)*N + j);

                    PixelBuffer[position].r = int_r;
                    PixelBuffer[position].g = int_g;
                    PixelBuffer[position].b = int_b;

                    /*First attempt*/
                    /*for(index = 0; index <= scene.object[index]; index += sizeof(Object)){
				double t = 0;
				//Shoot function
				if(scene.object[index].type = "sphere"){
					t = sphereIntersection(Ro, Rd, scene[index].position, scene[index].radius);
				}
				//Shoot function
				if(scene.object[index].type = "plane"){
					t = planeIntersection(Ro, Rd, scene[index].position, scene[index].radius);

				}
				 if(t > 0 && t < closestT){
                    closestT = t;

                    colorT = scene[index].color;

                }*/


        }
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
    //Accounting for only one camera
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
    //Keeping track of variables
    //to be incremented
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

             //*populating object array with our json contents*//
            if ((strcmp(key, "width") == 0) || (strcmp(key, "height") == 0) || (strcmp(key, "radius") == 0) || (strcmp(key, "radial-a2") == 0)) {
                double value = next_number(json);
            if(strcmp(key, "width") == 0){
                camera.width = value;
                incrementCamera += 1;
            }
            if(strcmp(key, "height") == 0){
                camera.height = value;
                incrementCamera += 1;
            }
            if((strcmp(key, "radius") == 0)){
                scene[lastIndex].radius = value;
                incrementObject += 1;
            }
            if((strcmp(key, "radial-a2") == 0)){
                lightScene[lastIndexLight].radiala2 = value;
                incrementObject += 1;
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
            else if ((strcmp(key, "color") == 0) || (strcmp(key, "position") == 0) || (strcmp(key, "normal") == 0)) {
                double* value = next_vector(json);

            if((strcmp(key, "color") == 0)){

                    if(Object == 'l'){
                        lightScene[lastIndexLight].color = malloc(3*sizeof(double));
                        lightScene[lastIndexLight].color = value;
                    }

                    if(Object != 'l'){
                        scene[lastIndex].color = malloc(3*sizeof(double));
                        scene[lastIndex].color = value;
                    }
            //Keep track objects
            incrementObject += 1;
            }
            if((strcmp(key, "position") == 0)){

                if(Object == 'l'){
                        lightScene[lastIndexLight].position = malloc(3*sizeof(double));
                        lightScene[lastIndexLight].position = value;
                    }

                    if(Object != 'l'){
                        scene[lastIndex].position = malloc(3*sizeof(double));
                        scene[lastIndex].position = value;
                    }
            //Keep track objects
            incrementObject += 1;
            }
            if((strcmp(key, "normal") == 0)){
                scene[lastIndex].normal = malloc(3*sizeof(double));
                scene[lastIndex].normal = value;
                incrementObject += 1;
                }
            }
            else{
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
            lastIndex += sizeof(Scene);
            if(incrementObject != 3){
                fprintf(stderr, "ERROR! Non-correct parameters!");
                exit(1);
            }
            incrementObject = 0;
        }
        if(Object == 'c'){
            if(incrementCamera != 2){
            fprintf(stderr, "ERROR! Non-correct parameters!");
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
