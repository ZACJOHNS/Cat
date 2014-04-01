#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 80

const char *PROMPT = "CLI> ";

int main(int argc, char *argv[])
{
	char buf[BUFSIZE];
	
	while(1) {
		printf("%s", PROMPT);
		
		if (fgets(buf, BUFSIZE, stdin) && strcmp(buf, "exit\n") != 0) {
			printf("%s", buf);
		} else {
			return EXIT_SUCCESS;
		}
	}
	
	return EXIT_SUCCESS;
}
