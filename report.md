
# Assignment report on Threads (hpcgp011)

## Blocks of code
The code starts with including header files for different functions and features that it uses: 
pthread.h for using mutex and pthreads, complex.h to work with complex types, string.h to use memcpy and time.h to use nanosleep function.

The global variables and pointers are initialized to be accessible by different functions and finished_mutex is a mutex that is initialized to regulate access to pointer to char “finished”.


The defined functions are as below:

newton_step:  This function allows to use more time efficient formula for finding new x_i through the iterations for different d (the degree of the polynomial) since d is between 1 and 9.

check_solution: since we can abort the iteration if  the new x_i is closer that 10^-3 to one of the roots of the polynomial or the origin, also if  the real or imaginary part of the new x_i is bigger than 10^10 this function is defined to abort iteration in such cases.

compute_line: this function receives a row (the real part is constant and the imaginary part from -2 to 2) it checks for every point in that line whether it is close enough to one of the roots and as long at it is not it goes through the newton_step function to find the new x_i and checks again until one the required conditions to abort the iterations is met. For every point the number of iterations and the root which the point converges to is saved in the two global variables, also the item of that line in “finished” pointer changes from 0 to 1. In fact it is equivalent to item_done pointer that we had in the lecture.

thread_function: this pointer to function receives void pointer which then it is caster to pointer to integer that is thread_id and this thread_id shows the thread offset which can be from 0 to number of threads-1.  The rows is set from 0 to number of rows -1. Number of rows is taken as one of the arguments from the command line.  For every row compute_line function is called.

## Program layout:
First it receives the number of threads, the number of rows and the degree of the polynomial from the command line. The exact values of the roots of the polynomial is computed and saved in the pointer “roots”. Also zero is considered as an additional zero.
We initialize pointer thread_args just to be able to pass thread_id to thread_function using pthread_create command. The function thread_function is responsible to do the calculations for every row by a thread. After finding the number of iterations and the root that every point converges to for all points in all rows and saving them in the global pointer to pointer to unsigned int “iterations” and pointer to pointer to signed char “attractors”	and changing the items in the “finished” pointer to 1 now it is time to write them to file.
Here a local item_done pointer is initialized: finished_loc and using pthread_mutex_lock and unlock the access of other threads to “finished” pointer is prevented. For every row it is checked that whether the row is done or not, if it is done the finished pointer copied to finished_loc and if it is not done yet nanosleep function will suspend the checking for 50 microseconds. After all rows being done we can copy attractors and iterations of that line to two local pointers. It is done because accessing to global variables is expensive in terms of performance. Those local pointers are used to write ppm files. At the end all threads are terminated using pthrea_join function and the pthread_mutex finished_mutex which was used to regulate access to pointer to char “finished” is destroyed using pthread_mutex_destroy. Also the allocated memories are freed.

## Performance:
In order to increase the performance vectorization is implemented using these two lines of code and optimization level 3 to calculate the difference for the real and imaginary part at the same time

`real = creal(solution) - creal(roots[i]);`
<br />
`imag = cimag(solution) - cimag(roots[i]);`

In order to avoid taking the square root to calculate the absolute value of a complex number creal and cimag functins are used to find the real and imaginary part of the complex number and then by the code line below we find the square of the absolute value and compare it with the 10^-6. Also we have avoided using power function and multiplication is used.
`real*real+imag*imag < LOWER_THRESHOLD * LOWER_THRESHOLD`

In addition taking a local copy of the item_done array which in this case is called "finished" is helpful to the performance because access to the global variables is expensive in terms of performance. 

