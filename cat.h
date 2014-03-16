#ifndef CAT_H
#define CAT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

// Command Line Argument list
int nFlag = 0, eFlag = 0;

// Function Headers
void ParseArgs(int argc, char *argv[]);
void formatFileOutput(const char *fileName);
void formatRawOutput();

FILE *Fopen(const char *path, const char *mode);

#endif
