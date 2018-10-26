#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <string.h>


#define MAX(a,b) (a>b ? a : b)
#define MIN(a,b) (a<b ? a : b)
#define NDISTANCES 3465 //round(100*sqrt(20^2+20^2+20^2)+1
#define MAX_BLOCK_SIZE ((unsigned long)1<<20)
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
short read_coordinate(char *restrict str)
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

inline
void read_coordinates(coordinates_t *restrict input, char *restrict str)
{
  input->x = read_coordinate(str);
  input->y = read_coordinate(str+8);
  input->z = read_coordinate(str+16);
}

inline
void read_block(size_t offset, size_t block_size, FILE *file, char *buffer, coordinates_t *input)
{
  fseek(file, 24 * offset, SEEK_SET);
  fread(buffer, 1, 24 * block_size, file);

#pragma omp parallel for
  for (size_t m = 0; m < block_size; m++)
    read_coordinates(input+m, buffer+24*m);
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
  const size_t block_size = MIN(MAX_BLOCK_SIZE, ncoordinates);
  omp_set_num_threads(nthreads);
  unsigned long *distance_counters = calloc(nthreads * NDISTANCES, sizeof(*distance_counters));
  coordinates_t *input_i = malloc(block_size * sizeof(*input_i));
  coordinates_t *input_j = malloc(block_size * sizeof(*input_j));
  coordinates_t * const input_j_backup = input_j;
  char *buffer = malloc(block_size * 24);

  for (size_t ib = 0; ib < ncoordinates; ib+=block_size) {
    size_t i_block_size = ib + block_size < ncoordinates ? block_size : ncoordinates -ib;
    read_block(ib, i_block_size, fp, buffer, input_i);
    for (size_t jb = ib; jb < ncoordinates; jb+=block_size) {
      size_t j_block_size = jb + block_size < ncoordinates ? block_size : ncoordinates - jb;
      if (jb != ib)
	read_block(jb, j_block_size, fp, buffer, input_j);
      else
	input_j = input_i;

      
#pragma omp parallel
      {
       	const int ithread = omp_get_thread_num(); 
#pragma omp for schedule(dynamic, 1000)
	for (size_t ix = 0; ix < i_block_size; ++ix)
	  for (size_t jx = (ib < jb ? 0 : ix+1); jx < j_block_size; ++jx)
	    distance_counters[NDISTANCES*ithread + compute_distance(input_i+ix, input_j+jx)]++;
      }
      input_j = input_j_backup;
    }
  }
  
  //#pragma omp parallel for
  for (size_t i = 0; i < NDISTANCES; i++)
    for (size_t j = 1; j < nthreads; j++)
      distance_counters[i]+=distance_counters[NDISTANCES*j + i];
  
  fclose(fp);
  fp = NULL;
  free(input_i);
  free(input_j);
  free(buffer);
  input_i = input_j  = NULL;
  buffer = NULL;

  for (size_t i = 0; i < NDISTANCES/100; i++) { //count up integer part of output
    for (size_t j = 0; j < 100; j++) { //count up rational part
      unsigned long count = distance_counters[100*i+j];
      if (count)
	printf("%.2d.%.2d %lu\n", i, j, count);
    }
  }

  for (size_t j = 0; j < 65; j++) { //count up rational part
    unsigned long count = distance_counters[3400+j];
    if (count)
      printf("34.%.2d %lu\n", j, count);
  }

  free(distance_counters);
  return 0;
}
