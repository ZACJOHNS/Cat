#ifndef CAT_H
#define CAT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

// Command Line Argument list
struct Args {
	int nFlag;
	int eFlag;
};

struct Args args;

void ParseArgs(int argc, char *argv[]);
FILE *Fopen(const char *path, const char *mode);

#endif
