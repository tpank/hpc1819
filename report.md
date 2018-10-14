# Assignment 2 report

The code starts with including header files for different functions and features that it uses: 
pthread.h for using mutex and pthreads, complex.h to work with complex types, string.h to use memcpy and time.h to use nanosleep function.

The global variables and pointers are initialized to be accessible by different functions and finished_mutex is a mutex that is initialized to regulate access to pointer to char “finished”.


The defined functions are as below:

newton_step:  This function allows to use more time efficient formula for finding new x_i through the iterations for different d (the degree of the polynomial) since d is between 1 and 9.

check_solution: since we can abort the iteration if  the new x_i is closer that 10^-3 to one of the roots of the polynomial or the origin, also if  the real or imaginary part of the new x_i is bigger than 10^10 this function is defined to abort iteration in such cases.

compute_line: this function receives a row (the real part is constant and the imaginary part from -2 to 2) it checks for every point in that line whether it is close enough to one of the roots and as long at it is not it goes through the newton_step function to find the new x_i and checks again until one the required conditions to abort the iterations is met. For every point the number of iterations and the root which the point converges to is saved in the two global variables, also the item of that line in “finished” pointer changes from 0 to 1. In fact it is equivalent to item_done pointer that we had in the lecture.

thread_function: this pointer to function receives void pointer which then it is caster to pointer to integer that is thread_id and this thread_id shows the thread offset which can be from 0 to number of threads-1.  The rows is set from 0 to number of rows -1. Number of rows is taken as one of the arguments from the command line.  For every row compute_line function is called.

Program layout:
First it receives the number of threads, the number of rows and the degree of the polynomial from the command line. The exact values of the roots of the polynomial is computed and saved in the pointer “roots”. Also zero is considered as an additional zero.
We initialize pointer thread_args just to be able to pass thread_id to thread_function using pthread_create command. The function thread_function is responsible to do the calculations for every row by a thread. After finding the number of iterations and the root that every point converges to for all points in all rows and saving them in the global pointer to pointers “iterations” and “attractors”	and changing the items in the “finished” pointer to 1 now it is time to write them to file.
Here a local item_done pointer is initialized: finished_loc and using pthread_mutex_lock and unlock the access of other threads to “finished” pointer is prevented. For every it is checked that whether the row is done or not, if it is done the finished pointer copied to finished_loc and if it is not done yet nanosleep function will suspend the checking for 50 microseconds. After all rows being done we can copy attractors and iterations of that line to two local pointers. It is done because accessing to global variables is expensive in terms of performance. Those local pointers are used to write ppm files. At the end all threads are terminated using pthrea_join function and the pthread_mutex finished_mutex which was used to regulate access to pointer to char “finished” is destroyed using pthread_mutex_destroy. Also the allocated memories are freed.


Question which should be addressed in the report:(after making sure that the answers are correct we will put the answers in right positions in the report)

1) Given the assumption that the degree and hence number of attractors is small, what is the smallest data type that can encode the attractor index associated with a pixel? Call this type TYPE_ATTR.
It is signe char.

2) Given the assumption that the number of iterations may be capped at a small value, what is the smallest data type that can encode the (capped) number of iterations till convergence associated with a pixel? Call this type TYPE_CONV.
It is unsigned int.
3) The absolute value of a complex number is the square root of its square norm. How can one avoid taking the square root? In particular, how can you avoid the use of the function cabs?
creal and cimag functins are used to find the real and imaginary part of the complex number and then by the code line below we find the square of the absolute value and compare it with the 10^-6.
real*real+imag*imag < LOWER_THRESHOLD * LOWER_THRESHOLD

4) The square norm of a complex number is the sum of two squares. When computing it for a difference x - x’, how can one avoid computing twice the difference of the respective real and imaginary parts?
Vectorization is implemented using these two line codes and optimization level 3 to calculate the difference for the real and imaginary part at the same time
real = creal(solution) - creal(roots[i]);
imag = cimag(solution) - cimag(roots[i]);

5) How can the suggested precomputing of the exact values of the roots of the given polynomial accelerate any of these steps? (What do you think)
