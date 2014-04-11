#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <libgen.h>
#include <signal.h>
#include <ctype.h>

#define MAXARGS 	64
#define BUFSIZE 	128
#define MAXLINE		64
#define TRUE 		1

int run_bg = 0, file_out = 0, file_in = 0;

char outFile[MAXLINE];
char inFile[MAXLINE];

struct Command {
	char name[MAXLINE];
	int argc;
	char *argv[MAXARGS];
};

void parse(char *commandLine);
void process();
void execute();
void changeDir(char *path);
void formatFileOut(int i);

struct Command command;

/*
=======================================================
MAIN FUNCTION
=======================================================
*/
int main(int argc, char *argv[])
{
	char commandLine[BUFSIZE];
	command.argc = 0;
	
	while (TRUE) {
		printf("SHELL > ");
		if (fgets(commandLine, BUFSIZE, stdin) == NULL) {
			printf("ERROR: fgets()\n");
			return(EXIT_FAILURE);
		} else if (*commandLine == '\n') {
			continue;
		} else {
			parse(commandLine);
			process();
			if (strcmp(command.argv[0], "exit") == 0)
				return(EXIT_SUCCESS);
			
			execute();
		}
		

		command.argc = 0;
	}
	return EXIT_FAILURE;
}

void createToken(char *start, char *end) 
{
	char token[MAXLINE];
	
	int i = 0;
	while (start <= end) {
		if (*start != '\\')
			token[i++] = *start;
		start++;
	}
	token[i] = '\0';
	
	command.argv[command.argc] = calloc(MAXLINE, sizeof(char));
	strcpy(command.argv[command.argc], token);
	command.argc++;
}

/*
=======================================================
PARSE FUNCTION
=======================================================
*/
void parse(char *commandLine) 
{
	int c, escape = 0;
	char *startTok;
	enum states { DULL, IN_WORD, IN_STRING } state = DULL;
	for (char *p = commandLine; *p != '\0'; p++) {
	c = (unsigned char) *p;
		switch (state) {
			case DULL:
				if (isspace(c)) {
					continue;
				}
				if (c == '"') {
					state = IN_STRING;
					startTok = p + 1;
					continue;
				}
				state = IN_WORD;
				startTok = p;
				continue;
			case IN_STRING:
				if (c == '\\') {
					escape = 1;
				}
				
				if (c == '"' && escape == 0) {
					createToken(startTok, --p);
					state = DULL;
				}
				
				if (c == '"' && escape == 1) {
					escape = 0;
				}
				continue;
			case IN_WORD:
				if (isspace(c)) { // if word has a space following
					createToken(startTok, --p);
					state = DULL;
				}
				if (*(p+1) == '\0') { // else if only one word
					createToken(startTok, p);
					state = DULL;
				}
				continue;
			
		}
	}
	
	strcpy(command.name, command.argv[0]);
	command.argv[0] = basename(command.name);
	
	command.argv[command.argc] = NULL;
}

/*
=======================================================
PROCESS FUNCTION
=======================================================
*/
void process()
{
	for (int i = 0; i < command.argc; i++) {
		if (strcmp(command.argv[i], "cd") == 0) {
			changeDir(command.argv[i+1]);
		}
		
		if (strcmp(command.argv[i], ">") == 0) {
			file_out = 1;
			strcpy(outFile, command.argv[i+1]);
			command.argv[i] = NULL;
			i++;
		}
		
		if (strcmp(command.argv[i], "<") == 0) {
			file_in = 1;
			strcpy(inFile, command.argv[i+1]);
			command.argv[i] = NULL;
			i++;
		}
		
		if (strcmp(command.argv[i], "&") == 0) {
			command.argv[i] = NULL;
			run_bg = 1;
			i++;
		}
	}
}


/*
=======================================================
EXECUTE FUNCTION
=======================================================
*/
void execute()
{
	pid_t pid;
	pid_t thisChildPid;
	int status;
	
	pid = fork();
	if (pid == -1) {	
		/* FORK FAILED */	
		printf("ERROR: fork()\n");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {	
		/* CHILD PROCESS */
		FILE *file;
		if (file_out) {
			file = freopen(outFile, "w", stdout);
		}
		
		if (file_in) {
			file = freopen(inFile, "r", stdin);
		}
		
		if (execvp(command.name, command.argv) < 0) {
			printf("ERROR: execvp() : %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		if (file_out) {
			file_out = 0;
			fclose(file);
		}
		
		if (file_in) {
			file_in = 0;
			fclose(file);
		}
		
	} else if (pid > 0) {	
		/* PARENT PROCESS */
		if (!run_bg) {
			do {
				thisChildPid = waitpid(pid, &status, 0);
				
				if (thisChildPid == -1) {
					printf("ERROR: waitpid() : %s\n", strerror(errno));
					exit(EXIT_FAILURE);
				}
				
				if (WIFEXITED(status)) {
					//printf("Child process exited, status=%d\n", WEXITSTATUS(status));
				}
			
			// while the child process has not exited normally 
			} while (!WIFEXITED(status));
		}
	}
}

void changeDir(char *path) 
{
	if (chdir(path) < 0) {
		printf("ERROR: chdir(): %s\n", strerror(errno));
	}
	
}
