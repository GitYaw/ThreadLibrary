# scroll down for actual makefile

queue.out: queue_tester.c queue.o
	clear
	$(CC) $(CFLAGS) -o $@ $^
	./$@

c:
	clear
	make


## MAKEFILE ##

# Target library
lib := libuthread.a

all: $(lib)

# Define compilation toolchain
CC	= gcc

# General gcc options
CFLAGS := -Wall
#CFLAGS += -Wextra -Werror
CFLAGS += -g # Debug flag

# Application objects to compile
objs := queue.o

$(lib): $(objs)
	ar -rcs $@ $^

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.a *.out 