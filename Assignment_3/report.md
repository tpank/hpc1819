# Assignment 3 report (hpcgp011)

## Data types

The input as well as the output uses a fixed point format to store the coordinates resp. distances.
For the coordinates, this is 5 digit decimal fixed point number, preceded with a sign, for the output it is a four digit unsigned number. As floating point data types require relatively expensive operations, a big size (4 bytes for *float*, 8 for *double*), short integers are used instead. The coordinates are stored as *signed short*, the distances as *unsigned short*. These have only a size of two bytes each. input coordinates range from -10.000 to 10.000. They are stored in the program ignoring the decimal point, therefore they can be stored in *short*, which ranges from -32768 to 32767. Thes same goes for the output, ranging between 0 and 34.64 (the maximum distances between two points in our coordinates cube, $$\sqrt{3*(2*10)^2}$$.

*coordinates_t* is a *struct* containing a coordinate triplet, therefor having a size of 6 bytes. All read input coordiantes are stored in the array *input*.

To count the number of occurences for each of the 3465 possible rounded distances, we use an null-initialized array of 3465 *unsigned short*. For the example data, no distance occurs more often than 65535 times, so this is sufficient.

## Blocks of code
*compute_distances* computes the distances between two coordinate triplets, returning it as a 4 decimal digit unsigned short integer. While the input coordinates are scaled by 1000, the output has to be scaled only by 100, therefor the result must be divided by 10. The use of the *sqrtf()* function for floats instead of *sqrt()* for longer doubles results in a significant speed up of the program. The float result is then implicitly truncated and returned as an integer.

*read_coordinates* converts a 7 character string of format \<sign\>ab.cde, with \<sign\> being + or - and a,b,c,d,e between 0 and 9 into a short integer. It uses the fact, that ASCII encoding of a digit is the same as the digit plus an offset.

*main* starts with the reading of the command line argument and opens the input file afterwards. Using *fstat* it checks the size of the input file. As the line length is fixed (24 bytes), we can assume, that there are $$\frac{\text{file size in byte}}{24 \text{byte}}$$ coordinate triplets to read. The file is then read linewise. 

The actual computing is done in the next step. For each tuple of two points the distance is computed and the corresponding distance counter incremented.

Finally, the program output is done. For this, integers have to be converted to fixed point rational numbers. For that, we split up the integers in the pre- and post-decimal point values, printing both of them with a fixed with using a format string.

## Parallization
In the code parallelization is not used for parsing, but even if it had been used synchronization would not be an issue because it only contains reading from a file and not writing to it.
Parallelization is done at computing the distance between the cells and the explicit synchroniztion types like atomic, critical and reduction which slow down the code are not used. In fact by allocating memory of size which is multiplication of number of threads and the size of global distance_counters array every thread is working only on its own part of the distance_counter_local array and there is no issue with synchronization. Since the version of the gcc is 8.1.1 it was  possible to use reduction on array but the way used in the code is more efficient than using reduction for the global array in terms of performance.





