# Target library
lib := libuthread.a

# will add more object files for next phases
obj := queue.o

# Compiler
CC := gcc
CFLAGS := -Wall 
CFLAGS += -g

all: $(lib)

# ----- NOTES -----
# creating a static library use this command:
# ar rcs my_library.a file1.o file2.o

# $@: replaced by name of target
# $<: replaced by name of first prerequisite/dependency
# $^: replaced by names of all prerequisites
# -----------------


## TODO: Phase 1
$(lib): $(obj)
	ar -rcs $@ $^

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(lib) $(obj)