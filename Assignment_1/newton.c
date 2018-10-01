#include <pthreads.h>
#include <math.h>

int **roots; //d x 2 array
int main(int argc, char** argv)
{
  //get the arguments
  if (argc < 4)
    exit(1);
  int nthreads, nrc, d;
  nthreads = atoi(argv[1]+2); //number of threads
  nrc = atoi(argv[2]+2); //number of rows and columns
  d = atoi(argv[3]); //f(x) = x^d-1
  roots = malloc(d * sizeof(*roots));
  // compute the actual results for the roots
  for (int i = 0; i < d; i++) {
    roots[i] = malloc(2* sizeof(**roots));
    roots[i][0] = cos(2* M_PI * i / d);
    roots[i][1] = sin(2* M_PI * i / d);
  }
  
  pthread_t *threads;
  threads = malloc(nthreads * sizeof(pthread_t));
  for (int i = 0; i<nthreads; i++) {
    //start the threads
  }
  //finish the threads
  //write to file
  for (int i = 0; i<d; i++)
    free(roots[d]);
  free(roots);
  free(threads);
  return 0;
}
