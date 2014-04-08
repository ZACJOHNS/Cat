#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/wait.h>


#define BUFSIZE 80

const char *PROMPT = "CLI > ";

void parse(char *command, int *argc, char *argv[]);
void call_system(int argc, char *argv[]);

int main()
{
	int argc;
	char command[BUFSIZE];
	char *argv[64];
	
	
	// Missing error handling
	while(1) {
		printf("%s", PROMPT);
		fgets(command, BUFSIZE, stdin);
		parse(command, &argc, argv);
		
		if (strcmp(argv[0], "exit") == 0) 
			exit(EXIT_SUCCESS);
			
		call_system(argc, argv);
	} 
	
	return EXIT_FAILURE;
}

/*	Parse Function
 * 	Takes an input and returns a zero-terminated array of char pointers,
 *  also returns the number of arguments within the array.
 *	each of which point to a zero-terminated character string.
*/ 
void parse(char *command, int *argc, char *argv[]) 
{
	int i = 0;
	char **next = argv;
    char *temp = strtok(command, " \n");
    while (temp != NULL) {
		i++;
		*next++ = temp;
        temp = strtok(NULL, " \n"); 
    }
    *next = NULL;
    *argc = i;
    
    argv[0] = basename(argv[0]);
}

void call_system(int argc, char *argv[]) 
{
	int rc = 0, run_bg = 0;
	
	// If an ampersand exists at the end of the argument list, set flag and replace argument
	if (!strcmp(argv[argc-1],"&")) {
		run_bg = 1;
		argv[argc-1] = NULL;
	}
	
	// To avoid child zombies the SIGCHLD signal is set
	signal(SIGCHLD, SIG_IGN); // <-- Must have error handling
	
	
	pid_t pid = fork();

	switch(pid) {
		case 0:
		// if the cd command is given change the parent directory and 
		// exit the switch. (Don't run execvp).
			if (!strcmp(argv[0], "cd")) {
				chdir(argv[1]); // <- error handling
				break;
			}
				
			if (execvp(argv[0], argv) < 0) {
				printf("ERROR: exec failed\n");
				exit(EXIT_FAILURE);
			}
		case -1:
			printf("ERROR: forking child process failed\n");
			exit(EXIT_FAILURE);
		default:
			if (run_bg) {
				printf("[proc %d started]\n", pid);
			} else {
				if ((waitpid(pid, &rc, 0) != -1)) {
					// wait for child process to finish
				}
			}
	}
}

