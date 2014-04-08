#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

	while((*commandInput != '\n') && (buf_chars < LINE_LENGTH)) { 
		buffer[buf_chars++] = *commandInput;	// read characters into buffer
		*commandInput = getchar();
	}
	buffer[buf_chars] = '\0';					// append terminating null character to end of string
	return 0;
}

/* 
 ====================================================================================
 MAIN METHOD
 ====================================================================================
 */

int main(int argc, char* argv[]) 
{
	int i;
	
	while(TRUE) {
		printf("SHELL > ");
		commandInput = getchar(); 						// get first character
		if (commandInput == '\n') { 					// if no input continue loop
			continue;
		} else {
			readCommand(commandLine, &commandInput); 	// read command
			
			if (strcmp(commandLine, "exit") == 0)		// exit shell if appropriate
				break;
				
			parseCommand(commandLine, &command); 		// parse command into argv[], argc
			//processCommand();
			
		}	
	}
	printf("\n");
	exit(EXIT_SUCCESS);									// shell exited successfully
}
