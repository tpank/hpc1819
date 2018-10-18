OBJS = 
TARGETS = cell_distance
CC = gcc
CFLAGS = -O3 -fopenmp -march=native
CFLAGS2 = -O2 -fopenmp -march=native
LIBS = -lgomp

.PHONY : clean all

all : $(TARGETS)

newton : cell_distance.c $(OBJS)
	$(CC) cell_distance.c $(CFLAGS) $(LIBS) $(OBJS) -o cell_distance 

clean :
	rm -f *.o  $(TARGETS)
