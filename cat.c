#include <stdio.h>
#include <string.h>
#define BLOCK_SIZE 512

int main(int argc, char *argv[])
{
	FILE *filePath;
	char fileName[BLOCK_SIZE], ch;
	int i;

	for (i = 1; i < argc; i++) {
		strncpy(fileName, argv[i], BLOCK_SIZE);

		filePath = fopen(fileName, "r");
		if (filePath == NULL) {
			printf("%s: No such file or directory\n", fileName);
			return 0;
		}

		while ((ch = fgetc(filePath)) != EOF) {
			putchar(ch);
		}
		fclose(filePath);
	}

	return 0;
}
