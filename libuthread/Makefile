## MAKEFILE ##

# Target library
lib := libuthread.a

all: $(lib)

# Don't print the commands unless explicitly requested with `make V=1`
ifneq ($(V),1)
Q = @
V = 0
endif

# Define compilation toolchain
CC	= gcc

# General gcc options
CFLAGS := -Wall
CFLAGS += -Wextra -Werror
## Debug flag
ifneq ($(D),1)
CFLAGS	+= -O2
else
CFLAGS	+= -g
endif
## Dependency generation
CFLAGS	+= -MMD

# Application objects to compile
objs := queue.o uthread.o preempt.o context.o
# objs += sem.o

# Include dependencies
deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

# Rule for libuthread.a
$(lib): $(objs)
	ar -rcs $@ $^

# Generic rule for compiling objects
%.o: %.c
	@echo "CC	$@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

# Cleaning rule
clean: FORCE
	@echo "CLEAN	$(CUR_PWD)"
	$(Q)rm -rf $(objs) $(deps) $(lib)

# Keep object files around
.PRECIOUS: %.o
.PHONY: FORCE
FORCE: