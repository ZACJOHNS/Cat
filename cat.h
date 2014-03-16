#ifndef CAT_H
#define CAT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

// Member Variables
// Command Line Argument list
int nFlag = 0, eFlag = 0;

// Member Functions
// Function Headers
void ParseArgs(int argc, char *argv[]); // unused 
void FormatOutput(FILE *input);

FILE *Fopen(const char *path, const char *mode);

#endif
