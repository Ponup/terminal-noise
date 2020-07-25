CC ?= gcc
CFLAGS := $(CFLAGS) -D_POSIX_C_SOURCE=200809L -Wall -Wpedantic -ansi -std=c99 -O2
PROGRAM = terminal-noise

.PHONY: all
all:
	$(CC) $(CFLAGS) -o $(PROGRAM) main.c

.PHONY: clean
clean:
	$(RM) $(PROGRAM) 

