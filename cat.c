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
	int c;
	while ((c = getopt(argc, argv, "nE")) != -1) {
		switch(c) {
			case 'n':
				nFlag = 1;
				break;
			case 'E':
				eFlag = 1;
				break;
			default: /* '?' */
				fprintf(stderr,
						"Error: Unknown option character '\\x%x'.\n",
						optopt);
				exit(EXIT_FAILURE);
		}
	}
	
	if (optind < argc) {
		while (optind < argc)
			formatFileOutput(argv[optind++]);
	}
	
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
	// not used
	/*
	int c;
	while ((c = getopt(argc, argv, "nE")) != -1) {
		switch(c) {
			case 'n':
				args.nFlag = 1;
				break;
			case 'E':
				args.eFlag = 1;
				break;
			default: 
				fprintf(stderr,
						"Error: Unknown option character '\\x%x'.\n",
						optopt);
				exit(EXIT_FAILURE);
		}
	}
	
	if (optind < argc) {
		while (optind < argc)
			printf("%s ", argv[optind++]);
		printf("\n");
	}
	*/
}

void formatFileOutput(const char *fileName)
{
	char ch;
	FILE *input = Fopen(fileName, "r");
	
	if (input == NULL)
		return;
	
	while ((ch = fgetc(input)) != EOF) {
		// format and display file contents
		fputc(ch, stdout);
	}
	if (ferror(input)) {
		// handle error
	}
	
	fclose(input);
}

FILE *Fopen(const char *fileName, const char *mode)
{
	FILE *f = fopen(fileName, mode);
	if (f == NULL) {
		perror(fileName);
		return f;
	}
	return f;
}

