# Variables
CC = clang

# Generic Rules
.c.o: 	$(CC) -c -Wall -std=c99 -o $*.o $*.c

# All Target
all:	cat hello cli

# Make Rules
cat: 	cat.o
		$(CC) -o cat cat.o
		
hello:	hello.c
		$(CC) -o hello hello.c
		
cli:	cli.c
		$(CC) -o cli cli.c

cat.o:	cat.h cat.c

# Clean Rules
.PHONY: clean
clean:	
	rm cat cat.o hello cli
