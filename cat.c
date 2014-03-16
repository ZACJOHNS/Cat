#include "cat.h"

/* Main Function
 *  Purpose: Simple cat command program that emulates the unix cat command.
 * 	Ability to take command line arguments and display data appropriately.
 * 	Input: Command line arguments that affect data presentation.
 * 	Output: Returns exit status
 */
int main(int argc, char *argv[])
{
	
	// Parse cmd line arguments and set appropriate flags
	ParseArgs(argc, argv);
	
	
	
	
	/*FILE *in;
	unsigned count = 0;
	int ch;
	
	in = stdin;
	
	while ((ch = fgetc(in)) != EOF) {
		count++;
	}
	if (ferror(in)) {
		// handle error
	}
	* */
	
	return EXIT_SUCCESS;
	
	
	/*
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
			// requires error checking! (EOF)
			while ((ch = fgetc(filePath)) != EOF) {
				fputc(ch, stdout);
			}

			fclose(filePath);
		}
	} else {
		// if no command line arguments exist (no file name specified)
		// requires error checking! (EOF)
		while ((ch = fgetc(stdin)) != EOF) {
			fputc(ch, stdout);
		}
	}
	return EXIT_SUCCESS;
	*/
}

void ParseArgs(int argc, char *argv[])
{
	int aflag = 0, bflag = 0, index, c;
	char *cvalue = NULL;
	
	while ((c = getopt(argc, argv, "ab")) != -1) {
		switch(c) {
			case 'a':
				aflag = 1;
				break;
			case 'b':
				bflag = 1;
				break;
			default: /* '?' */
				fprintf(stderr,
						"Unknown option character '\\x%x'.\n",
						optopt);
				exit(EXIT_FAILURE);
		}
	}
		
	printf("aflag = %d, bflag = %d\n, optind=%d\n", aflag, bflag, optind);
		
		
	printf("name argument = %s\n", argv[optind]);
}

FILE *Fopen(const char *path, const char *mode)
{
	FILE *f = fopen(path, mode);
	if (f == NULL) {
		perror(path);
		exit(EXIT_FAILURE);
	}
	return f;
}

