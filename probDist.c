//#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


struct vertex {
  float x;
  float y;
};

struct triangle {
  struct vertex r;
  struct vertex s;
  struct vertex t;
};

void randomInscribedTri(struct triangle *A) {

  A->r.x = 0;
  A->r.y = 2 * ((float)rand()/(float)RAND_MAX);

  A->t.x = (float)rand()/(float)RAND_MAX;
  A->t.y = 0;

  A->s.x = (float)rand()/(float)RAND_MAX;
  A->s.y = ((-2 * A->s.x) + 2);

}


void printStruct(struct triangle A) {
  printf("(%0.3f,%0.3f)\n",A.r.x, A.r.y);
  printf("(%0.3f,%0.3f)\n",A.s.x, A.s.y);
  printf("(%0.3f,%0.3f)\n",A.t.x, A.t.y);
}

float calculateArea(struct triangle A) {

  float area;
  area = fabsf((A.r.x * (A.s.y -A.t.y)) + (A.s.x * (A.t.y - A.r.y)) + (A.t.x * (A.r.y - A.s.y)));

  return area/2;
}

int main() {

  srand(time(NULL));
  struct triangle A[5];
  //struct vertex r,s,t;
  float area;
  int i;

  for(i = 0; i < 5; i++) {

    // Assigning random values for each vertices
    randomInscribedTri(&A[i]);

    // Printing the (x,y) coordinates of each vertice
    printStruct(A[i]);

    // Calculating the area using the coordinates of each vertice
    area = calculateArea(A[i]);

    printf("AREA: %0.3f\n", area);

    printf("-----------------------\n");

  }



  return 0;
}
