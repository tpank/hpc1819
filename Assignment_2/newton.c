#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <string.h>
#include <time.h>

#define UPPER_THRESHOLD 10000000000
#define LOWER_THRESHOLD 0.001

//10 distinct colors in RGB
const char * const colors[] =
  {"230 25 75 ", "60 180 75 ", "255 225 25 ", "0 130 200 ", "245 130 48 ", "145 30 180 ", "70 240 240 ", "240 50 230 ", "210 245 60 ", "250 190 190 ", "0 128 128 ", "230 190 255 ", "170 110 40 ", "255 250 200 ", "128 0 0 ", "170 255 195 ", "128 128 0 ", "255 215 180 ", "0 0 128 ", "128 128 128 ", "255 255 255 ", "0 0 0 "};
unsigned int  nthreads, nrc, d;
double complex *roots; //d+1 array
signed char **attractors;
unsigned int **iterations;
char *finished;
pthread_mutex_t finished_mutex;

inline double complex newton_step(double complex x_last) {
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
    
    signed char *attractors_line = malloc(nrc*sizeof(**attractors)); //freed in main
    unsigned int *iterations_line = malloc(nrc*sizeof(**iterations));

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
        attractors_line[i] = attractor;
        iterations_line[i] = iteration_count > 20 ? 20 : iteration_count;
    }
    attractors[line] = attractors_line;
    iterations[line] = iterations_line;
    finished[line] = 1;
}

void *thread_function(void *args) {
    int thread_id = ((int*)args)[0];
    int linen = thread_id;
    while (linen < nrc)
    {
        compute_line(linen);
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
    finished = calloc(nrc, sizeof(*finished));

    pthread_t *threads;
    threads = malloc(nthreads * sizeof(*threads));
    int *thread_args = malloc(nthreads * sizeof(*thread_args));
    pthread_mutex_init(&finished_mutex, NULL);
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

    
  
    //write to file
    char name_attr[] = "newton_attractors_xd.ppm";
    char name_conv[] = "newton_convergence_xd.ppm";
    name_attr[19] = d+'0';
    name_conv[20] = d+'0';
    FILE *fa = fopen(name_attr, "w");
    FILE *fc = fopen(name_conv, "w");

    fprintf(fa, "P3\n%d %d\n%d\n", nrc, nrc, 255);
    fprintf(fc, "P3\n%d %d\n%d\n", nrc, nrc, 255);

    char *finished_loc = malloc(nrc*sizeof(*finished_loc));
    struct timespec sleep_timespec;
    sleep_timespec.tv_sec = 0;
    sleep_timespec.tv_nsec = 50000;

    for (int ix = 0; ix < nrc;)
    {
        pthread_mutex_lock(&finished_mutex);
        if ( finished[ix] != 0 )
            memcpy(finished_loc, finished, nrc*sizeof(*finished));
        pthread_mutex_unlock(&finished_mutex);


        if ( finished_loc[ix] == 0 )
        {
            nanosleep(&sleep_timespec, NULL);
            continue;
        }
        signed char *attractors_loc;
        unsigned int *iterations_loc;
        for ( ; ix < nrc && finished[ix] != 0; ++ix ) {
            attractors_loc = attractors[ix];
            iterations_loc = iterations[ix];
            for (int jx = 0; jx < nrc; ++jx)
                fprintf(fa, colors[attractors_loc[jx]]);
            fprintf(fa, "\n");
            for (int jx = 0; jx < nrc; ++jx)
	            fprintf(fc, colors[iterations_loc[jx]]);
            fprintf(fc, "\n");    
            free(attractors_loc); //allocated in thread_function
            free(iterations_loc);
        }
    }
    free(finished_loc);
    
    fclose(fa);
    fclose(fc);
    //finish the threads
    for (int tx=0; tx < nthreads; ++tx) {
        int ret;
        if (ret = pthread_join(threads[tx], NULL))
        {
            printf("Error joining thread: %d\n", ret);
            exit(1);
        }
    }
    pthread_mutex_destroy(&finished_mutex);
    free(roots);
    free(threads);
    free(thread_args);
    free(attractors);
    free(iterations);
    free(finished);

    return 0;
}
