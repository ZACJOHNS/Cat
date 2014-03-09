#include <stdio.h>
#include <string.h>
#define BLOCK_SIZE 255

int main(int argc, char *argv[])
{
	char ch;

	if (argc > 1) {
		FILE *filePath;
		char fileName[BLOCK_SIZE];
		int i;

		for (i = 1; i < argc; i++) {
			strncpy(fileName, argv[i], BLOCK_SIZE);
			filePath = fopen(fileName, "r");
			if (filePath == NULL) {
				printf("%s: No such file or directory\n", fileName);
				continue;
			}		
	
			while ((ch = fgetc(filePath)) != EOF) {
				fputc(ch, stdout);
			}

			fclose(filePath);
		}
	} else {
		while ((ch = fgetc(stdin)) != EOF) {
			fputc(ch, stdout);
		}
	}
	return 0;
}
