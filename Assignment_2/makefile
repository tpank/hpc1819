OBJS = 
TARGETS = newton
CC = gcc
CFLAGS = -O3 -pthread
CFLAGS2 = -O2 -pthread
LIBS = -lm

.PHONY : clean all

all : $(TARGETS)

newton : newton.c $(OBJS)
	$(CC) newton.c $(CFLAGS) $(LIBS) $(OBJS) -o newton 

debug : newton.c $(OBJS)
	$(CC) newton.c $(CFLAGS) $(LIBS) $(OBJS) -o newton

clean :
	rm -f *.o  $(TARGETS)
