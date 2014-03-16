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
	
	FILE *input;
	// If file arguments exist
	if (optind < argc) {
		while (optind < argc) {
			input = Fopen((argv[optind++]), "r");
			FormatOutput(input);
		}
	} else {
		FormatOutput(stdin);
	}
	
	return EXIT_SUCCESS;
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

void FormatOutput(FILE *input)
{
	char ch, prev;
	int line = 0;
	
	if (input != NULL) {
		for (prev = '\n'; (ch = getc(input)) != EOF; prev = ch) {
			// Line numbers
			if (nFlag && prev == '\n') {
				fprintf(stdout, "%6d\t", ++line);
				if (ferror(stdout)) {
					break;
				}
			}
			
			// Dollar sign at end of line
			if (eFlag && ch == '\n') {
				fputc('$', stdout);
				if (ferror(stdout)) {
					break;
				}
			}
			
			// after appropriate formatting, print current character
			fputc(ch, stdout);
		}
		if (ferror(input)) {
			printf("ERROR: error reading from input");
		}
		if (ferror(stdout)) {
			printf("ERROR: stdout error");
		}
		
	fclose(input);
	}
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

