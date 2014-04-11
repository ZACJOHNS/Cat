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
#define MAXCOMMANDS	16
#define TRUE 		1

int run_bg = 0, file_out = 0, file_in = 0, multiple_progs = 0;
int progCount = 0;
int processCount = 0;
int fgWait = 0;
pid_t fgPid = -1;

char outFile[MAXLINE];
char inFile[MAXLINE];

struct Command {
	char name[MAXLINE];
	int argc;
	char *argv[MAXARGS];
};

void parse(char *commandLine);
int process();
void execute();
void changeDir(char *path);
void createCommands();
void signalHandler(int signal);
void intstpSignalHandler(int signal);

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
			if (strcmp(command.argv[0], "exit") == 0)
				return(EXIT_SUCCESS);
		
			if (process())
				execute();
		}
		command.argc = 0;
	}
	return EXIT_FAILURE;
}

/*
=======================================================
CREATE TOKEN FUNCTION
=======================================================
*/
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
	command.argv[command.argc] = NULL;
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
				if (c == ';') {
					execute();
					command.argc = 0;
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
					createToken(startTok, (p-1));
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
}

/*
=======================================================
PROCESS FUNCTION
=======================================================
*/
int process()
{
	for (int i = 0; i < command.argc; i++) {
		if (strcmp(command.argv[i], "cd") == 0) {
			changeDir(command.argv[i+1]);
			return 0;
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
	}
	return 1;
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
	
	strcpy(command.name, command.argv[0]);
	command.argv[0] = basename(command.name);
	
	if (strcmp(command.argv[command.argc-1], "&") == 0) {
		run_bg = 1;
		command.argv[command.argc-1] = NULL;
	}
	
	for (int i = 0; i < command.argc; i++) 
		printf("Arg %d: %s\n", i, command.argv[i]);
		
	signal(SIGINT, intstpSignalHandler);
	signal(SIGTSTP, intstpSignalHandler);
	signal(SIGCHLD, signalHandler);
	
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
			fgPid = pid;
			fgWait = TRUE;
			
			while(fgWait) {
				pause();
			}
		} else {
			processCount++;
			printf("[%d] %d\n", processCount, pid);
			run_bg = 0;
		}
				
	}
}

/*
=======================================================
CHANGE DIRECTORY FUNCTION
=======================================================
*/
void changeDir(char *path) 
{
	if (chdir(path) < 0) {
		printf("ERROR: chdir(): %s\n", strerror(errno));
	}
}


/*
=======================================================
SIGNAL HANDLER FUNCTION
=======================================================
*/
void signalHandler(int signal)
{
	int status;
	pid_t pid;
	
	pid = wait3(&status, WNOHANG | WUNTRACED | WCONTINUED, NULL);
	
	if (pid == -1) {
		printf("ERROR: waitpid() : %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	if (pid > 0) {
		if (fgPid == pid) {
			fgPid = -1;
			fgWait = 0;
			
		} else {
			if (WIFEXITED(status)) {
				
				printf("[%d] %d Done\n", processCount, pid);
				//printf("[%d] Terminated (Status: %d)\n", pid, WEXITSTATUS(status));
				processCount--;
				return;
			}
		}
		if (WIFSIGNALED(status)) {
			printf("[%d] Terminated (Signal: %d)\n", pid, WTERMSIG(status));
			processCount--;
			return;
		}
		
		if (WIFSTOPPED(status)) {
			printf("[%d] Stopped (Signal: %d)\n", pid, WSTOPSIG(status));
			processCount--;
			return;
		}
		
		if (WIFCONTINUED(status)) {
			printf("[%d] Continued...\n", pid);
			return;
		}
	}
}

/*
=======================================================
INTSTP SIGNAL HANDLER FUNCTION
=======================================================
*/
void intstpSignalHandler(int signal) 
{
	if (fgPid != -1)
		kill(fgPid, signal);
}



