all:	hello cat

hello:	hello.c
	clang -Wall -std=c99 -o hello hello.c

cat:	cat.c
	clang -Wall -std=c99 -o cat cat.c
