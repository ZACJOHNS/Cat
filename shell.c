#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#include "shell.h"

#define MAX_LINE_LENGTH 64
#define MAX_ARG_NUM     12

#define NORMAL          0
#define PIPE            1

#define TRUE            1
#define FALSE           0

int parse(char *userInput, char *cmdArgv[], char **nextArg, int *modePtr);
void childOutPipe(int pipeDes[], char *cmdArgv[]);
void childInPipe(int pipeDes[], char *cmdArgv[]);
void execute(char *cmdArgv[], int mode, char **nextArg);
void executePipe(int pipeDes[], char *cmdArgv[], char *cmdArgv2[]);

/*
=========================================================================
MAIN FUNCTION
=========================================================================
*/
int main(int argc, char *argv[])
{
    char userInput[MAX_LINE_LENGTH];
    char *cmdArgv[MAX_ARG_NUM];
    char *nextArg = NULL;
    int cmdArgc;
    int mode;
    int debug = FALSE;

    if (argc > 1)
    {
        if (*argv[1] == '1')
        {
            debug = TRUE;
        }
    }

    while (1)
    {
        mode = NORMAL;
        printf("SHELL > ");
        if (fgets(userInput, MAX_LINE_LENGTH, stdin) != NULL)
        {
            cmdArgc = parse(userInput, cmdArgv, &nextArg, &mode);

            if (strcmp(cmdArgv[0], "exit") == 0)
                return EXIT_SUCCESS;

            if (debug)
            {
                for (int i = 0; i < cmdArgc; i++)
                    printf("Arg[%d]: %s\n", i, cmdArgv[i]);
            }

            execute(cmdArgv, mode, &nextArg);

        }
    }
    return EXIT_FAILURE;
}

int parse(char *userInput, char *cmdArgv[], char **nextArg, int *modePtr)
{
    int cmdArgc = 0;
    int terminate = 0;
    char *ptr = userInput;

    while (*ptr != '\0' && terminate == 0)
    {
        *cmdArgv = ptr;
        cmdArgc++;

        while (*ptr != ' ' && *ptr != '\t' && *ptr != '\0' && *ptr != '\n' && terminate == 0)
        {
            switch (*ptr)
            {
                case '|':
                    *modePtr = PIPE;
                    *cmdArgv = '\0';
                    ptr++;
                    while (*ptr == ' ' || *ptr == '\t')
                        ptr++;
                    *nextArg = ptr;
                    terminate = 1;
                    break;
            }
            ptr++;
        }

        while ((*ptr == ' ' || *ptr == '\t' || *ptr == '\n') && terminate == 0)
        {
            *ptr = '\0';
            ptr++;
        }

        cmdArgv++;
    }

    *cmdArgv = '\0';
    return cmdArgc;
}

void execute(char *cmdArgv[], int mode, char **nextArg)
{
    pid_t pid;
    int status;

    int mode2 = NORMAL;
    char *nextArg2;
    char *cmdArgv2[MAX_LINE_LENGTH];

    int pipeDes[2];

    switch (mode)
    {
        case PIPE:
            if (pipe(pipeDes) == -1)
            {
                fprintf(stderr, "Error: %s: cannot open pipe: %s\n", cmdArgv[0], strerror(errno));
                exit(EXIT_FAILURE);
            }
            parse(*nextArg, cmdArgv2, &nextArg2, &mode2);
            executePipe(pipeDes, cmdArgv, cmdArgv2);
            break;

        case NORMAL:
            pid = fork();

            if (pid < 0)
            {
                fprintf(stderr, "Error: %s: cannot create child: %s\n", cmdArgv[0], strerror(errno));
                exit(EXIT_FAILURE);
            }
            else if (pid == 0)
                execvp(cmdArgv[0], cmdArgv);
            break;
    }

    while ((pid = wait(&status)) >= 0)
    {
        if (WEXITSTATUS(status) != 0)
            fprintf(stderr, "child %d exited with code %d\n", pid, WEXITSTATUS(status));
    }
}

void executePipe(int pipeDes[], char *cmdArgv[], char *cmdArgv2[])
{
    pid_t pid1, pid2;
    int status1, status2;

    // create the first child
    if ((pid1 = fork()) == -1)
    {
        fprintf(stderr, "Error: %s: cannot create child 1: %s\n", cmdArgv[0], strerror(errno));
        exit(EXIT_FAILURE);
    }
    else if (pid1 == 0)
        childOutPipe(pipeDes, cmdArgv);

    if ((pid2 = fork()) == -1)
    {
        fprintf(stderr, "Error: %s: cannot create child 1: %s\n", cmdArgv2[0], strerror(errno));
        exit(EXIT_FAILURE);
    }
    else if (pid2 == 0)
        childInPipe(pipeDes, cmdArgv2);

    // the pipe is handed off the the childer, so we should close the parent ends
    close(pipeDes[0]);
    close(pipeDes[1]);
}

void childOutPipe(int pipeDes[], char *cmdArgv[])
{
    close(pipeDes[0]);  // close the reading end of the pipe

    // re-direct stdout to the writting end of the pipe
    if (dup2(pipeDes[1], STDOUT_FILENO) < 0)
    {
        perror("child 1: cannot redirect stdout to pipe");
        exit(EXIT_FAILURE);
    }
    close(pipeDes[1]);  // this is now STDOUT_FILEOUT

    execvp(cmdArgv[0], cmdArgv);
    perror("Child 1: cannot run");
    exit (EXIT_FAILURE);
}

void childInPipe(int pipeDes[], char *cmdArgv[])
{
    close(pipeDes[1]); // close the writting end of the pipe

    // re-direct stdin from the reading end of the pipe
    if (dup2(pipeDes[0], STDIN_FILENO) < 0)
    {
        perror("child 2: cannot redirect stdin from pipe");
        exit(EXIT_FAILURE);
    }
    close(pipeDes[0]);  // this is now STDIN_FILENO

    execvp(cmdArgv[0], cmdArgv);
    perror("Child 2: cannot run");
    exit (EXIT_FAILURE);
}
