OBJS = 
TARGETS = cell_distance
CC = gcc
CFLAGS = -O3 -fopenmp -march=native
CFLAGS_GCOV = -ftest-coverage -fprofile-arcs -fopenmp -march=native
CFLAGS_GPROF = -O3 -pg -fopenmp -march=native
LIBS = -lgomp -lm

.PHONY : clean all

all : $(TARGETS)

cell_distance : cell_distance.c $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) cell_distance.c -o cell_distance

gcov : cell_distance.c
	$(CC) $(OBJS) $(LIBS) $(CFLAGS_GCOV) cell_distance.c -o cell_distance

gprof : cell_distance.c
	$(CC) $(OBJS) $(LIBS) $(CFLAGS_GPROF) cell_distance.c -o cell_distance


clean :
	rm -f *.o  $(TARGETS)
