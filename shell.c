#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "shell.h"

struct Command command;

 /* 
 ====================================================================================
 READ COMMAND FUNCTION
 Read user input character by character and store it into the buffer / commandLine
 ====================================================================================
 */
int readCommand(char *buffer, char *commandInput) 
{
	buf_chars = 0;

	while ((*commandInput != '\n') && (buf_chars < LINE_LENGTH)) { 
		buffer[buf_chars++] = *commandInput;	// read characters into buffer
		*commandInput = getchar();
	}
	buffer[buf_chars] = '\0';					// append terminating null character to end of string
	return 0;
}

 /* 
 ====================================================================================
 PARSE COMMAND FUNCTION
 Parse commandLine into Command struct appropriate for use with execvp (array of null
 terminated strings) -> using the strtok function
 ====================================================================================
 */
 int parseCommand(char *commandLine, struct Command *command) 
 {
	 char *pch;
	 pch = strtok(commandLine, " ");
	 
	 int i = 0;
	 while (pch != NULL) {
		 command->argv[i] = pch;
		 pch = strtok(NULL, " ");
		 i++;
	 }
	 command->argc = i;
	 command->argv[i++] = NULL;
	
	 return 0;
 }
 
 /* 
 ====================================================================================
 EXECUTE COMMAND FUNCTION
 Execute command based on the pre-parsed commandLine buffer -> using execvp function
 to fork a process and wait or continue according to the ampersand argument (&) 
 ====================================================================================
 */
 int executeCommand()
 {
	 pid_t child_pid, thisChildPid;
	 int status;
	 
	 child_pid = fork(); 									// fork a child process
	 if (child_pid < 0) {
		 fprintf(stderr, "ERROR: Child forking process failed\n");
		 exit(EXIT_FAILURE);
	 } else if (child_pid == 0) {							// CHILD PROCESS
		 if (execvp(command.argv[0], command.argv) < 0) {     	// execute the command
			fprintf(stderr, "ERROR: execvp failed\n");
            exit(EXIT_FAILURE);
         }
	 } else if (child_pid > 0) {							// PARENT PROCESS
	 	if (run_bg) {
			printf("proccess %d started\n", child_pid);		// run process in background
		} else {
			do {
				thisChildPid = waitpid(child_pid, &status, WUNTRACED | WCONTINUED);
				if (thisChildPid == -1) {
					perror("waitpid");
					exit(EXIT_FAILURE);
				}
				if (WIFEXITED(status)) {
					printf("exited, status=%d\n", WEXITSTATUS(status));
					return 0;
				} else if (WIFSIGNALED(status)) {
					printf("killed by signal %d\n", WTERMSIG(status));
				} else if (WIFSTOPPED(status)) {
					printf("stopped by signal %d\n", WSTOPSIG(status));
				} else if (WIFCONTINUED(status)) {
					printf("continued\n");
				}
				
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
			 
		 }
	 }
	 return 0;
 }
 
 /* 
 ====================================================================================
 PROCESS COMMAND FUNCTION
 Checks passed command argument with official command arguments for a match, handles
 program according to results
 ====================================================================================
 */
 int processCommand() 
 {
	 for (int i = 0; i < command.argc; i++) {
		if (strcmp(command.argv[i], "cd") == 0) {
			changeDir();
			return 1;
		}
	 }
	 
	 return 0;
 }
 
  /* 
 ====================================================================================
 CHANGE DIRECTORY FUNCTION
 Changes directory based on passed parameters. If no directory is given, the directory
 is changed to root.
 ====================================================================================
 */
 void changeDir() {
	 if (command.argv[1] == NULL) {
		 chdir(getenv("HOME"));
	 } else {
		 if (chdir(command.argv[1]) == -1) {
			 printf(" %s: no such directory\n", command.argv[1]);
		 }
	 }
	 
 }

/* 
 ====================================================================================
 MAIN FUNCTION
 ====================================================================================
 */
int main(int argc, char* argv[]) 
{
	int i;
	
	while (TRUE) {
		printf("SHELL > ");
		commandInput = getchar(); 							// get first character
		if (commandInput == '\n') { 						// if no input continue loop
			continue;
		} else {
			readCommand(commandLine, &commandInput); 		// read command
			if (strcmp(commandLine, "exit") == 0) break;	// exit shell if appropriate
			parseCommand(commandLine, &command); 			// parse command into argv[], argc
			if (processCommand() == 0)						// process commands if nesseccary
				executeCommand();							// execute command
		}	
	}

	exit(EXIT_SUCCESS);										// shell exited successfully
}
