OBJS = 
TARGETS = newton
CC = gcc
CFLAGS = -O3
LIBS = -lm
.PHONY : clean all
all : $(TARGETS)
newton : newton.c $(OBJS)
	$(CC) -o newton $(CFLAGS) $(LIBS) $(OBJS) newton.c 
clean :
	rm -f *.o  $(TARGETS)
