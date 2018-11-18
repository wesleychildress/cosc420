//#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

struct vertex {
  float x;
  float y;
};

void printStruct(struct vertex vert) {
  printf("(%0.3f,%0.3f)\n",vert.x, vert.y);
}

float calculateArea(struct vertex r, struct vertex s, struct vertex t) {

  float area;
  area = fabsf((r.x * (s.y -t.y)) + (s.x * (t.y - r.y)) + (t.x * (r.y - s.y)));

  return area/2;
}

int main() {

  srand(time(NULL));
  struct vertex r,s,t;
  float area;

  float k = (float)rand()/(float)RAND_MAX;

  r.x = 0;
  r.y = 2 * ((float)rand()/(float)RAND_MAX);

  t.x = (float)rand()/(float)RAND_MAX;
  t.y = 0;

  s.x = (float)rand()/(float)RAND_MAX;
  s.y = ((-2 * s.x) + 2);

  printStruct(r);
  printStruct(s);
  printStruct(t);

  area = calculateArea(r,s,t);

  printf("AREA: %0.3f\n", area);

  return 0;
}
