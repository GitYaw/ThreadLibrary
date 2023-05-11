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

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

uthread.o: queue.h private.h uthread.h

sem.o: queue.h private.h sem.h

preempt.o: uthread.h private.h

context.o: uthread.h private.h

clean:
	rm -f *.o *.a *.out 