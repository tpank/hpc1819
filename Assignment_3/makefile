OBJS = 
TARGETS = cell_distance
CC = gcc
CFLAGS = -O3 -fopenmp -march=native
CFLAGS2 = -O2 -fopenmp -march=native
LIBS = -lgomp -lm

.PHONY : clean all

all : $(TARGETS)

cell_distance : cell_distance.c $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) cell_distance.c -o cell_distance

clean :
	rm -f *.o  $(TARGETS)
