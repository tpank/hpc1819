OBJS = 
TARGETS = newton
CC = gcc
CFLAGS = -O3 -pthread -march=native
CFLAGS2 = -O2 -pthread -march=native
LIBS = -lm

.PHONY : clean all

all : $(TARGETS)

newton : newton.c $(OBJS)
	$(CC) newton.c $(CFLAGS) $(LIBS) $(OBJS) -o newton 

debug : newton.c $(OBJS)
	$(CC) newton.c $(CFLAGS) $(LIBS) $(OBJS) -o newton

clean :
	rm -f *.o  $(TARGETS)
