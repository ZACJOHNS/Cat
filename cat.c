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
	while ((c = getopt(argc, argv, "nebsvt")) != -1) {
		switch(c) {
			case 'n':
				nFlag = 1;
				break;
			case 'e':
				eFlag = 1, vFlag = 1;
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
			case 't':
				tFlag = 1, vFlag = 1;
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

void ParseArgs(int argc, char *argv[]) {}

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
			
			// is sflag is set skip blank lines > 1
			if (prev == '\n') {
				if (sFlag) {
					if (ch == '\n') {
						if (skipLine) { continue; }
						skipLine = 1;					
					} else 
						skipLine = 0;
				}
				// print appropriate line number 
				if (nFlag && (!bFlag || ch != '\n')) {
					fprintf(stdout, "%6d\t", ++line);
				}
			}
			
			// if at the end of a line (indicated by '\n') 
			if (ch == '\n') {
				if (eFlag) {
					fputc('$', stdout);
				}
			
			// if not at the end of a line and current char == tab char
			} else if (ch == '\t') {
				if (tFlag) {
					fprintf(stdout, "%s", "^I");
					// t implies v so we must continue here
					continue;
				}
			// if vFlag is set and ch is a ASCII control character (iscntrl)
			} else if (vFlag) {
				if (iscntrl(ch)) {
					// ascii control ch 
					// bitwise or operation on control character to get printable char
					fprintf(stdout, "%c%c", '^', ch | 0100);
					continue;
				}
			}
			
			// after appropriate formatting, print current character
			fputc(ch, stdout);
		}
		// error handling
		if (ferror(input)) {
			fprintf(stderr, "ERROR: error reading from input");
		}
		if (ferror(stdout)) {
			fprintf(stderr, "ERROR: stdout error");
		}
		
	// close input file
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

