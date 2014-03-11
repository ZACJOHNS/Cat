#include <stdio.h>
#include <string.h>
#define BLOCK_SIZE 255

/* 
* A simple cat command program that emulates the unix cat command.
* Ability to take command line arguments and display data appropriately
*/
int main(int argc, char *argv[])
{
	// helper variables
	char ch;
	int i;
	
	// if command line arguments exist (only support for file names)
	if (argc > 1) {
		FILE *filePath;
		char fileName[BLOCK_SIZE];

		for (i = 1; i < argc; i++) {
			strncpy(fileName, argv[i], BLOCK_SIZE);
			filePath = fopen(fileName, "r");
			if (filePath == NULL) {
				printf("%s: No such file or directory\n", fileName);
				continue;
			}		
			
			//  write file contents to stdout
			while ((ch = fgetc(filePath)) != EOF) {
				fputc(ch, stdout);
			}

			fclose(filePath);
		}
	} else {
		// if no command line arguments exist (no file name specified)
		while ((ch = fgetc(stdin)) != EOF) {
			fputc(ch, stdout);
		}
	}
	return 0;
}
