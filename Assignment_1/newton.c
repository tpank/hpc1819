#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <complex.h>

#define UPPER_THRESHOLD 10000000000
#define LOWER_THRESHOLD 0.001

unsigned int  nthreads, nrc, d;
double complex *roots; //d x 2 array
inline double complex newton_step(double complex x_last)
{
  double complex ddx = cpow(x_last, d-1);
  // cpow(a,b) is implemented as exp(log(a)*b), maybe it will be faster to implement our own pow function
  return x_last - (x_last*ddx -1) / ddx; 
}
//checks whether a solution matches one of the precalculated roots;
//returns -2 if the solution exceeds the upper threshold (and therefor probably diverges)
//returns 0..d-1 if the solution matches a root
//returns -1 otherwise
inline int check_solution(double complex solution)
{
  for (int i = 0; i < d; i++)
    if (cabs(solution - roots[i]) < LOWER_THRESHOLD)
      return i;
  if (abs(cimag(solution))>UPPER_THRESHOLD || abs(creal(solution))>UPPER_THRESHOLD)
    return -2;
  return -1;
}



int main(int argc, char** argv)
{
  //get the arguments
  if (argc < 4)
    exit(1);

  nthreads = atoi(argv[1]+2); //number of threads
  nrc = atoi(argv[2]+2); //number of rows and columns
  d = atoi(argv[3]); //f(x) = x^d-1
  roots = malloc(d * sizeof(*roots));
  // compute the actual results for the roots
  for (int i = 0; i < d; i++)
    roots[i] = cos(2* M_PI * i / d) + I * sin(2* M_PI * i / d);
  
  pthread_t *threads;
  threads = malloc(nthreads * sizeof(pthread_t));
  for (int i = 0; i<nthreads; i++) {
    //start the threads
  }
  //finish the threads
  //write to file
  free(roots);
  free(threads);
  return 0;
}
