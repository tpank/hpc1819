#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <complex.h>

#define UPPER_THRESHOLD 10000000000
#define LOWER_THRESHOLD 0.001

//10 distinct colors in RGB
const char * const colors[] =
  {"230 25 75 ", "60 180 75 ", "255 225 25 ", "0 130 200 ", "245 130 48 ", "145 30 180 ", "70 240 240 ", "240 50 230 ", "210 245 60 ", "250 190 190 ", "0 128 128 ", "230 190 255 ", "170 110 40 ", "255 250 200 ", "128 0 0 ", "170 255 195 ", "128 128 0 ", "255 215 180 ", "0 0 128 ", "128 128 128 ", "255 255 255 ", "0 0 0 "};
unsigned int  nthreads, nrc, d;
double complex *roots; //d+1 array
signed char **attractors;
unsigned int **iterations;

inline double complex newton_step(double complex x_last)
{
     double complex ddx = cpow(x_last, d-1); //x^(n-1)
    // cpow(a,b) is implemented as exp(log(a)*b), maybe it will be faster to implement our own pow function
    return x_last - (x_last*ddx -1) / (d * ddx);
}

//checks whether a solution matches one of the precalculated roots;
//returns d if the solution exceeds the upper threshold (and therefor probably diverges)
//returns 0..d-1 if the solution matches a root
//returns -1 otherwise
inline signed char check_solution(double complex solution)
{
    for (int i = 0; i < d+1; i++)
        if (cabs(solution - roots[i]) < LOWER_THRESHOLD)
        return i;
    if (abs(creal(solution))>UPPER_THRESHOLD || abs(cimag(solution))>UPPER_THRESHOLD)
        return d;
    return -1;
}

inline void compute_line(int line) {
    complex double point;
    int iteration_count;
    signed char attractor;
    for (int i = 0; i < nrc; i++)
    {
        point = -2 + 4.0*line/nrc  + I * (2 - 4.0*i/nrc);
        iteration_count = 0;
        attractor = check_solution(point);
        while (-1 == attractor)
        {
	    point = newton_step(point);
	    attractor = check_solution(point);
	    ++iteration_count;
        }
        attractors[line][i] = attractor;
        iterations[line][i] = iteration_count > 20 ? 20 : iteration_count;
    }
}

void *thread_function(void *args) {
    int thread_id = ((int*)args)[0];
    int linen = thread_id;
    while (linen < nrc)
    {
        compute_line(linen);
        // TODO here somehow the file writing thread should be informed that there is a line ready to be printed
        linen += nthreads;
    }
  return NULL;
}

int main(int argc, char** argv) {
  
    //get the arguments
    if (argc < 4)
        exit(1);
  
    nthreads = atoi(argv[1]+2); //number of threads
    nrc = atoi(argv[2]+2); //number of rows and columns

    d = atoi(argv[3]); //f(x) = x^d-1
    if (d>=10)
        exit(1);
    

    roots = malloc((d+1) * sizeof(*roots));

    // compute the actual results for the roots
    for (int i = 0; i < d; i++)
        roots[i] = cos(2* M_PI * i / d) + I * sin(2* M_PI * i / d);

    roots[d] = 0; //workaround to break if the newton algorithm is to close to the origin
    attractors = malloc(nrc*sizeof(*attractors));
    iterations = malloc(nrc*sizeof(*iterations));

    for (int i = 0; i < nrc; i++)
    {
        attractors[i] = malloc(nrc*sizeof(**attractors));
        iterations[i] = malloc(nrc*sizeof(**iterations));
    }

    pthread_t *threads;
    threads = malloc(nthreads * sizeof(*threads));
    int *thread_args = malloc(nthreads * sizeof(*thread_args));

    for (int tx = 0; tx<nthreads; tx++)
    {
        //start the threads
        thread_args[tx] = tx;
        int ret;
        if(ret = pthread_create(threads+tx, NULL, thread_function, (void*)(thread_args+tx))) 
        {
            printf("Error creating thread: %d, error code %d\n", tx, ret);
            exit(1);
        }
    }

    //finish the threads
    for (int tx=0; tx < nthreads; ++tx) {
        int ret;
        if (ret = pthread_join(threads[tx], NULL))
        {
            printf("Error joining thread: %d\n", ret);
            exit(1);
        }
    }
  
    //write to file
    char name_attr[] = "newton_attractors_xd.ppm";
    char name_conv[] = "newton_convergence_xd.ppm";
    name_attr[19] = d+'0';
    name_conv[20] = d+'0';
    FILE *fa = fopen(name_attr, "w");
    FILE *fc = fopen(name_conv, "w");

    fprintf(fa, "P3\n%d %d\n%d\n", nrc, nrc, 255);
    fprintf(fc, "P3\n%d %d\n%d\n", nrc, nrc, 255);

    //TODO Here apparently a maximum number of iterations should be known. I dont know how we can obtain it though.
    for (int i = 0; i< nrc; ++i)
    {
        for (int j = 0; j < nrc; ++j)
            fprintf(fa, colors[attractors[i][j]]);
        fprintf(fa, "\n");
        for (int j = 0; j < nrc; ++j)
	    fprintf(fc, colors[iterations[i][j]]);
        fprintf(fc, "\n");
    }
    fclose(fa);
    fclose(fc);

    free(roots);
    free(threads);
    free(thread_args);
    for (int i = 0; i < nrc; ++i)
    {
        free(attractors[i]);
        free(iterations[i]);
    }
    free(attractors);
    free(iterations);

    return 0;
}
