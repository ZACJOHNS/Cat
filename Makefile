# Variables
CC = clang

# Generic Rules
.c.o: 	$(CC) -c -Wall -std=c99 -o $*.o $*.c

# Make Rules
cat: 	cat.o
		$(CC) -o cat cat.o

cat.o:	cat.h cat.c

# Clean Rules
.PHONY: clean
clean:	
	rm cat cat.o
