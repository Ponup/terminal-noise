

all:
	gcc -D_POSIX_C_SOURCE -Wall -Wpedantic -std=c11 -o terminal-noise main.c

clean:
	rm -f terminal-noise

