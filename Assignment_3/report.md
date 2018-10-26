# Assignment 3 report (hpcgp011)

## Data types

The input as well as the output use a fixed point format to store the coordinates resp. distances.
For the coordinates, this is 5 digit decimal fixed point number, preceded with a sign, for the output it is a four digit unsigned number. As floating point data types require relatively expensive operations, a big size (4 bytes for *float*, 8 for *double*), short integers are used instead. The coordinates are stored as *signed short*, the distances as *unsigned short*. These have only a size of two bytes each. input coordinates range from -10.000 to 10.000. They are stored in the program ignoring the decimal point, therefore they can be stored in *short*, which ranges from -32768 to 32767. Thes same goes for the output, ranging between 0 and 34.64 (the maximum distances between two points in our coordinates cube, $$\sqrt{3*(2*10)^2}$$.

*coordinates_t* is a *struct* containing a coordinate triplet, therefor having a size of 6 bytes. All read input coordiantes are stored in the arrays *input_a* and *input_b.

To count the number of occurences for each of the 3465 possible rounded distances, we use an null-initialized array of 3465 *unsigned long*. As the program should support up to $$2^{32}$$ cells, $$2^{32}*(2^{32}-1)/2 = 2^{63}-2^{31}$$ distances will be computed. Even if all these distances would be the same (which only happens if all cells are at the same location), an 8 byte unsigned integer is big enough to store the count.

## Blocks of code
*compute_distances* computes the distances between two coordinate triplets, returning it as a 4 decimal digit unsigned short integer. While the input coordinates are scaled by 1000, the output has to be scaled only by 100, therefor the result must be divided by 10. The use of the *sqrtf()* function for floats instead of *sqrt()* for longer doubles results in a significant speed up of the program. The float result is then implicitly truncated and returned as an integer.

*read_coordinate* convertso a 7 character string of format \<sign\>ab.cde, with \<sign\> being + or - and a,b,c,d,e between 0 and 9 into a short integer. It uses the fact, that ASCII encoding of a digit is the same as the digit plus an offset. That offset is represented by the integer constant '0'. The characters are  multiplied by the decimal power of their significance and added up, then the offset is substracted for all digits at once.

*read_coordinates* uses the *read_coordinate* function to convert a 24 characters array *str* into a coordinate triplet, storing it in *input*

*read_block* reads a block of severeal cells from the file given by *fp*, storing it in the provided coordinate_t array input. While the reading from the file to the buffer is done sequentially, the parsing uses OpenMP for parallelization.

*main* starts with the reading of the command line argument and opens the input file afterwards. Using *fstat* it checks the size of the input file. As the line length is fixed (24 bytes), we can assume, that there are $$\frac{\text{file size in byte}}{24 \text{byte}}$$ coordinate triplets to read. 

Then, the distances between each combination of two blocks of the input cells are computed. This is done to avoid too big memory consumption in case of huge input files. For each block, the coordinates are loaded into memory. 

The actual computation is paralellized again using OpenMP and dynamic scheduling. The scheduling is necessary, as the inner for loop is getting shorter for every iteration of the outer loop, therefor every outer loop iteration takes less time than the previous.

Finally, the program output is produced. For this, the counter integers have to be converted to fixed point rational numbers. For that, we split up the integers in the pre- and post-decimal point values, printing both of them with a fixed width using a format string.

## Parallelization
For the parsing, first a big chunk of the file is loaded into memory. Then, in parallel using OpenMP the lines are parsed into coordinate triplets. Every line can be parsed individually and independent from each other, as both the *buffer* as well as the *input_* arrays contain indiviudal locations for every point.

When the distances between the cells are computed, OpenMP comes into play again. To avoid significant slowdown, atomic and critical statements are avoided. Each thread works on its individual section of the *distance_counters* array, so there are no data races. In the end, the thread arrays are summed up to achieve the final result. Even though OpenMP 4.5 seems to support an array reduction, this implementation turned out to be better in terms of performances.





