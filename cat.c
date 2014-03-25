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
	while ((c = getopt(argc, argv, "nEbsv")) != -1) {
		switch(c) {
			case 'n':
				nFlag = 1;
				break;
			case 'E':
				eFlag = 1;
				break;
			case 'b':
				bFlag = 1, nFlag = 1;
				break;
			case 's':
				sFlag = 1;
				break;
			case 'v':
				vFlag = 1;
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
		// Format raw input 
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

/* Format Output Function
 *  Purpose: Takes an input and formats output appropriate to set flags.
 * 	Input: FILE determined by a file path or standard input (stdin).
 * 	Output: Writes formatted output to stdout, no return type.
 */
void FormatOutput(FILE *input)
{
	char ch, prev, cntrlCh;
	int line = 0, skipLine = 0;
	
	if (input != NULL) {
		for (prev = '\n'; (ch = getc(input)) != EOF; prev = ch) {
			
			if (prev == '\n') {
				if (sFlag) {
					if (ch == '\n') {
						if (skipLine) { continue; }
						skipLine = 1;					
					} else 
						skipLine = 0;
				}
				if (nFlag && (!bFlag || ch != '\n')) {
					fprintf(stdout, "%6d\t", ++line);
				}
			}
			
			
			if (ch == '\n') {
				if (eFlag) {
					fputc('$', stdout);
				}
			} else if (vFlag) {
				if (iscntrl(ch)) {
					fputc('^', stdout);
					cntrlCh = (ch == '\177' ? '?' : ch | 0100);
					fputc(cntrlCh, stdout);
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

/* Fopen Function
 *  Purpose: fopen extension with simple error checking
 * 	Input: Filepath and single char mode
 * 	Output: Returns a pointer to the new FILE 
 */
FILE *Fopen(const char *fileName, const char *mode)
{
	FILE *f = fopen(fileName, mode);
	if (f == NULL) {
		perror(fileName);
		return f;
	}
	return f;
}

