#ifndef SHELL_H
#define SHELL_H

#define LINE_LENGTH 	80
#define MAX_ARGS 		64
#define TRUE 			1

static char commandInput = '\0';
static char commandLine[LINE_LENGTH];
static int buf_chars = 0;

struct Command {
	char *name;
	int argc;
	char *argv[MAX_ARGS];
};

int readCommand(char *commandLine, char *commandInput);

#endif
