#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <string.h>

#define NDISTANCES 3465 //round(100*sqrt(20^2+20^2+20^2)+1

typedef struct coordinates {
  short x, y, z; //these are the coordinates * 1000
} coordinates_t;


inline
unsigned short compute_distance(coordinates_t * restrict a, coordinates_t * restrict b)
{
  short dx, dy, dz;
  dx = a->x - b->x;
  dy = a->y - b->y;
  dz = a->z - b->z;
  unsigned short result = sqrtf(dx*dx + dy*dy + dz*dz) / 10;
  return result;
}

inline
short read_coordinates(char *str)
{
  //str has format +03.123
  short v =
     10000 * str[1]
    + 1000 * str[2]
    +  100 * str[4]
    +   10 * str[5]
    +    1 * str[6]
    -11111 * '0';
  return str[0] == '+' ? v : -v;
}

int main(int argc, char *argv[])
{
  if (argc < 2 || !strcmp(argv[1], "-t")) {
    printf("Usage: %s -t<number of OpenMP threads>\n", argv[0]);
    exit(1);
  }
  size_t nthreads = strtol(argv[1]+2, NULL, 0);

  FILE *fp = fopen("cells", "r");
  if (fp == NULL) {
    printf("Could not open \"cells\"\n");
    exit(1);
  }
  struct stat st;
  if(fstat(fileno(fp), &st)) { //gets some statistics for the file fp. st.size is the filesize in byte
    printf("Call of fstat went wrong\n");
    exit(1);
  }
  size_t ncoordinates = st.st_size / 24; //24 characters/line

  coordinates_t *input = malloc(ncoordinates * sizeof(*input));
  char *buffer = malloc(25);
  for (size_t i = 0; i < ncoordinates; i++) {
    if (NULL == fgets(buffer, 25, fp))
      exit(1);
    input[i].x = read_coordinates(buffer);
    input[i].y = read_coordinates(buffer+8);
    input[i].z = read_coordinates(buffer+16);  
  }
  free(buffer);
  buffer = NULL;
  fclose(fp);
  fp = NULL;
  
  omp_set_num_threads(nthreads);
  unsigned *distance_counters = calloc(nthreads * NDISTANCES, sizeof(*distance_counters));

#pragma omp parallel
  {
    const int ithread = omp_get_thread_num();
#pragma omp for schedule(dynamic, 1000)
    for (size_t i = 0; i < ncoordinates; i++)
      for (size_t j = i+1; j < ncoordinates; j++)
	distance_counters[NDISTANCES*ithread + compute_distance(input+i, input+j)]++;

#pragma omp for
    for (size_t i = 0; i < NDISTANCES; i++)
      for (size_t j = 1; j < nthreads; j++)
	distance_counters[i]+=distance_counters[NDISTANCES*j + i];
  }

  free(input);
  input = NULL;  

  for (size_t i = 0; i < NDISTANCES/100; i++) { //count up integer part of output
    for (size_t j = 0; j < 100; j++) { //count up rational part
      unsigned count = distance_counters[100*i+j];
      if (count)
	printf("%.2d.%.2d %d\n", i, j, count);
    }
  }

  for (size_t j = 0; j < 65; j++) { //count up rational part
    unsigned count = distance_counters[3400+j];
    if (count)
      printf("34.%.2d %d\n", j, count);
  }

  free(distance_counters);
  return 0;
}
