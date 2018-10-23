#include <stdio.h>
#include <stdlib.h>
#include <math.h>


typedef struct
{
    double x;
    double y;
    double z;
} Point;

int find_row_index(int i, int j, int size)
{
  int k;
  if(i==0)
  {
    return -1;
  }
  if(i==1)
  {
    return 0;
  }
  if(i>1)
  {
    k = 0;
    for(int index=0; index<i ; ++index)
    {
      k+=index;
    }
    k=k+j;
  }
return k;
}

int counter_function(double* array, int i ,int size)
{
  int count;
  if(i==0)
  {
    count = 1;
    for(int j = i+1; j < size; ++j)
    {
      if(fabs(array[j]-array[i]) < 0.001)
      {
        ++count;
      }
      else
      {
        return count;
      }
    }
  }
  if(i == size-1 && fabs(array[i-1]-array[i])>=0.001)
  {
    return 1;
  }

  if(i!=0 && fabs(array[i-1]-array[i])< 0.001)
  {
    return 0;
  }
  else
  {
    count = 1;
    for(int j = i+1; j < size; ++j)
    {
      if(fabs(array[j]-array[i]) < 0.001)
      {
        ++count;
      }
      else
      {
        return count;
      }
    }
  }
}



int main()
{
  FILE * fp;
  fp = fopen ("cells", "r");

  int nmb_cells=0;
  char c;
  // First we calculate the number of cells in the file cells
  if (fp == NULL)
  {
      printf("Could not open file %s", "cells");
      return 0;
  }

  // Extract characters from file and store in character c
  for (c = getc(fp); c != EOF; c = getc(fp))
      if (c == '\n') // Increment count if this character is newline
          nmb_cells = nmb_cells + 1;


  printf("The file %s has %d lines\n", "cells", nmb_cells);

  Point *cell = malloc(nmb_cells * sizeof(Point));
  rewind(fp);  //rewind function sets the position indicator associated with the pointer fp to the beginning of the file.

  int index = 0;
  for(index=0; index<nmb_cells;++index)
  {
    fscanf(fp, "%lf %lf %lf", &cell[index].x, &cell[index].y, &cell[index].z);
    //Now points is ready!
    printf("x = %.3f  y = %.3f  z = %.3f\n", cell[index].x, cell[index].y,cell[index].z);
  }
  fclose(fp);
  fp = NULL;

  //Now we want to claculate the distance between cells!

 double **distance;
 distance = (double**) malloc(((nmb_cells-1)*(nmb_cells)/2) * sizeof(double));

 for (int i=1; i<nmb_cells;++i)
 {
    distance[i] = (double*) malloc(i * sizeof(double));
 }

 double* distance_row = (double*) malloc(((nmb_cells-1)*(nmb_cells)/2) * sizeof(double));

 for (int i=1; i<nmb_cells; ++i)
  {
    for (int j=0; j<i;++j)
    {
      double xdistance = (cell[j].x - cell[i].x)*(cell[j].x - cell[i].x);
      double ydistance = (cell[j].y - cell[i].y)*(cell[j].y - cell[i].y);
      double zdistance = (cell[j].z - cell[i].z)*(cell[j].z - cell[i].z);
      double tmp = sqrt(xdistance+ydistance+zdistance); //calculates the distance and rounds it to 2 decimal places
      distance[i][j] = round(tmp*100.00)/100.00;
      int k = find_row_index(i,j,nmb_cells);
      distance_row[k] = distance[i][j];
      printf("Distance between cell #:%d and cell #:%d  is [row_index=%d with %05.2f] padding to %05.2f\n",i,j,k,distance_row[k],distance[i][j]);
    }

  }

for (int i=1; i< nmb_cells;++i)
{
  for (int j=0; j<i;++j)
  {
    printf("distance[%d][%d] = %05.2f;\n",i,j,distance[i][j]);
  }
}


for (int i=0; i<((nmb_cells-1)*(nmb_cells)/2);++i)
{
  printf("a[%d] = %05.2f;\n",i , distance_row[i]);
}

// We reorder the array in ascending reorder
for (int i = 0; i < ((nmb_cells-1)*(nmb_cells)/2); i++)                     //Loop for asccending ordering
{
  for (int j = i + 1; j < ((nmb_cells-1)*(nmb_cells)/2); j++)             //Loop for comparing other values
  {
    if (distance_row[j] - distance_row[i] < 0.001)                //Comparing other array elements
    {
      double tmp = distance_row[i];         //Using temporary variable for storing last value
      distance_row[i] = distance_row[j];            //replacing value
      distance_row[j] = tmp;             //storing last value
    }
  }
}
  for (int k=0; k< ((nmb_cells-1)*(nmb_cells)/2); ++k)
  {
    printf("%05.2f new k index is:%d\n" , distance_row[k], k);
  }

for (int k=0; k< ((nmb_cells-1)*(nmb_cells)/2); ++k)
{
  int counter = counter_function(distance_row,k,((nmb_cells-1)*(nmb_cells)/2));
  if (counter!=0)
  {
    printf("%05.2f %d\n" , distance_row[k],counter);
  }
}


  free (cell);
  for (int i=1; i<nmb_cells;++i)
  {
    free(distance[i]);
  }
  free (distance);

  free(distance_row);

  return 0;
}
