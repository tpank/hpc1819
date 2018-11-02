#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <string.h>
#include <time.h>

#define UPPER_THRESHOLD 10000000000
#define LOWER_THRESHOLD 0.001
#define COLOR_LENGTH 12
//10 distinct colors in RGB
const char * const greyscale[] = {
				 "000 000 000 ", "005 005 005 ", "010 010 010 ", "015 015 015 ", "020 020 020 ",
				 "025 025 025 ", "030 030 030 ", "035 035 035 ", "040 040 040 ", "045 045 045 ",
				 "050 050 050 ", "055 055 055 ", "060 060 060 ", "065 065 065 ", "070 070 070 ",
				 "075 075 075 ", "080 080 080 ", "085 085 085 ", "090 090 090 ", "095 095 095 ",
				 "100 100 100 ", "105 105 105 ", "110 110 110 ", "115 115 115 ", "120 120 120 ",
				 "125 125 125 ", "130 130 130 ", "135 135 135 ", "140 140 140 ", "145 145 145 ",
				 "150 150 150 ", "155 155 155 ", "160 160 160 ", "165 165 165 ", "170 170 170 ",
				 "175 175 175 ", "180 180 180 ", "185 185 185 ", "190 190 190 ", "195 195 195 ",
				 "200 200 200 ", "205 205 205 ", "210 210 210 ", "215 215 215 ", "220 220 220 ",
				 "225 225 225 ", "230 230 230 ", "235 235 235 ", "240 240 240 ", "245 245 245 ",
				 "250 250 250 ", "255 255 255 "
};
const char * const colors[] = {
			       "230 025 075 ", "060 180 075 ", "255 225 025 ", "000 130 200 ", "245 130 048 ",
			       "145 030 180 ", "070 240 240 ", "240 050 230 ", "210 245 060 ", "250 190 190 ",
			       "000 128 128 ", "230 190 255 ", "170 110 040 ", "255 250 200 ", "128 000 000 ",
			       "170 255 195 ", "128 128 000 ", "255 215 180 ", "000 000 128 ", "128 128 128 ",
			       "255 255 255 ", "000 000 000 "
}; //RGB triplets of 20 different colors, encoded as strings

unsigned int  nthreads, nrc, d;
double complex *roots; //d+1 array
signed char **attractors;
unsigned int **iterations;
char *finished;
pthread_mutex_t finished_mutex;

inline double complex newton_step(double complex x_last) {
    complex double intermediate;
    //x_(n+1) = x_n - f(x_n)/f'(x_n)
    //= x_n - (x^d-1)/d*x^(n-1)
    //= 1/(d*x_n^(d-1)) + (d-1)/d * x_n
    switch (d) {
    case 1:
        return 1;
    case 2:
        return 1/(2*x_last) + x_last/2;
    case 3:
        return 1/(3*x_last*x_last)+2*x_last/3;
    case 4:
        return 1/(4*x_last*x_last*x_last)+3*x_last/4;
    case 5:
        intermediate = x_last * x_last;
        return 1/(5*intermediate*intermediate)+4*x_last/5;
    case 6:
        intermediate = x_last * x_last;
        return 1/(6*intermediate*intermediate*x_last)+5*x_last/6;
    case 7:
        intermediate = x_last * x_last *x_last;
        return 1/(7*intermediate*intermediate)+6*x_last/7;
    case 8:
        intermediate = x_last * x_last;
        return 1/(8*intermediate*intermediate*x_last)+7*x_last/8;
    case 9:
        intermediate = x_last * x_last;
        intermediate = intermediate * intermediate;
        return 1/(9*intermediate * intermediate)+8*x_last/9;
    default:
        printf("Unexpected power: d = %u\n", d);
        exit(1);
    }
}

//checks whether a solution matches one of the precalculated roots;
//returns d if the solution exceeds the upper threshold (and therefor probably diverges)
//returns 0..d-1 if the solution matches a root
//returns -1 otherwise
inline signed char check_solution(double complex solution)
{
    for (int i = 0; i < d+1; i++)
    {
        double real, imag;
        real = creal(solution) - creal(roots[i]);
        imag = cimag(solution) - cimag(roots[i]);
        if (real*real+imag*imag < LOWER_THRESHOLD * LOWER_THRESHOLD)
	    return i;
    }
    if (abs(creal(solution))>UPPER_THRESHOLD || abs(cimag(solution))>UPPER_THRESHOLD)
        return d;
    return -1;
}

inline void compute_line(int line) {
    complex double point;
    complex double imag_part =  I*(2 - 4.0*line/nrc);
    int iteration_count;
    signed char attractor;
    
    signed char *attractors_line = malloc(nrc*sizeof(**attractors)); //freed in main
    unsigned int *iterations_line = malloc(nrc*sizeof(**iterations));

    for (int i = 0; i < nrc; i++)
    {
        point =  (-2 + 4.0*i/nrc) + imag_part;
        iteration_count = 0;
        attractor = check_solution(point);
        while (-1 == attractor)
        {
	        point = newton_step(point);
	        attractor = check_solution(point);
	        ++iteration_count;
        }
        attractors_line[i] = attractor;
        iterations_line[i] = iteration_count > 50 ? 50 : iteration_count;
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
    const char * const usage = "Usage: newton -t<number of threads> -l<number of lines> <power d of x in f(x)=x^d-1>\n";
    if (argc < 4)
    {
        printf("%s", usage);
        exit(1);
    }
  
    if (!strncmp(argv[1], "-t", 2) && !strncmp(argv[2], "-l", 2))
    {
        nthreads = atoi(argv[1]+2); //number of threads
        nrc = atoi(argv[2]+2); //number of rows and columns
        d = atoi(argv[3]); //f(x) = x^d-1
    }
    else if (!strncmp(argv[1], "-l", 2) && !strncmp(argv[2], "-t", 2))
    {
        nthreads = atoi(argv[2]+2); //number of threads
        nrc = atoi(argv[1]+2); //number of rows and columns
        d = atoi(argv[3]); //f(x) = x^d-1
    }
    else
    {
        printf("%s", usage);
        exit(1);
    }

    if (d > 9 || d == 0 || nthreads == 0 || nrc == 0)
    {
        printf("d must be smaller than 10 and all parameters must be greater than 0\n");
	printf("%s", usage);
        exit(1);
    }

    roots = malloc((d+1) * sizeof(*roots));

    // compute the actual roots
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
    name_attr[19] = d+'0'; //ASCII representation of character d if 0<=d<=9
    name_conv[20] = d+'0';
    FILE *fa = fopen(name_attr, "w");
    FILE *fc = fopen(name_conv, "w");

    fprintf(fa, "P3\n%d %d\n%d\n", nrc, nrc, 255); //print headers
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
	        fwrite(colors[attractors_loc[jx]], 1, COLOR_LENGTH, fa); //sizeof(char) is 1 by definition
            fwrite("\n", 1, 1, fa);
            for (int jx = 0; jx < nrc; ++jx)
                fwrite(greyscale[iterations_loc[jx]], 1, COLOR_LENGTH, fc);
            fwrite("\n", 1, 1, fc);    
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
