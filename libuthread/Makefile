# scroll down for actual makefile

c:
	clear
	make queue.out

%.out: %_tester.c %.o
	clear
	$(CC) $(CFLAGS) -o $@ $^
	./$@

## MAKEFILE ##

# Target library
lib := libuthread.a

all: $(lib)

# Define compilation toolchain
CC	= gcc

# General gcc options
CFLAGS := -Wall
CFLAGS += -Wextra # -Werror
CFLAGS += -g # Debug flag

# Application objects to compile
objs := queue.o uthread.o sem.o preempt.o context.o

$(lib): $(objs)
	ar -rcs $@ $^

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

sem.o: queue.h private.h

uthread.o: queue.h private.h

preempt.o: uthread.h private.h

context.o: uthread.h private.h

clean:
	rm -f *.o *.a *.out 