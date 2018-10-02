OBJS = 
TARGETS = newton
CC = gcc
CFLAGS = -O3 -pthread
LIBS = -lm
.PHONY : clean all
all : $(TARGETS)
newton : newton.c $(OBJS)
	$(CC) -o newton $(CFLAGS) $(LIBS) $(OBJS) newton.c 
debug : newton.c $(OBJS)
	$(CC) -o newton -pthread -Og -g $(LIBS) $(OBJS) newton.c 
clean :
	rm -f *.o  $(TARGETS)
