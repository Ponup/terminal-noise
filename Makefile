CC ?= gcc
CFLAGS := $(CFLAGS) -D_POSIX_C_SOURCE -Wall -Wpedantic -ansi -std=c99
PROGRAM = terminal-noise

.PHONY: all
all:
	$(CC) $(CFLAGS) -o $(PROGRAM) main.c

.PHONY: clean
clean:
	$(RM) $(PROGRAM) 

