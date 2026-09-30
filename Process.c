#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
    int pid, mypid, parentpid, status;
    char line[1024];
    int running = 1;

    while (running)
    {   
        printf("\x1b[38;2;35;255;139m""return0>""\x1b[0m");
        fgets(line, sizeof(line), stdin);

        char *termlines[100] = {NULL}; // 100 arguments
        char *token = strtok(line, " \t\n");
        for (int i = 0; token != NULL; i++)
        {
            termlines[i] = token;
            token = strtok(NULL, " \t\n");
            termlines[i + 1] = NULL; // end arry with null
        }

        if (termlines[0] == NULL)
        {
            continue; // empty line
        }

        if (strcmp(termlines[0], "exit") == 0)
        {
            running = 0; // stop shell
            continue;
        }

        if (strcmp(termlines[0], "help") == 0)
        { // help command
            printf("return0 built-in commands:\n");
            printf("  exit - quit the shell\n");
            printf("  help - show this message\n");
            continue;
        }

        if (strcmp(termlines[0], "cd") == 0) // so cd can change directories
        {
            if (termlines[1] == NULL)
            {
                fprintf(stderr, "cd: expected argument\n");
            }
            else if (chdir(termlines[1]) != 0)
            {
                perror("cd failed");
            }
            continue;
        }

        pid = fork();

        switch (pid)
        {
        case 0:
            mypid = getpid();
            parentpid = getppid();
            execvp(termlines[0], termlines);
            // if execvp fails
            printf("Error executing command: %s\n", termlines[0]);
            printf("[This is printf from child process where fork PID %d - mypid %d - parentpid %d]\n", pid, mypid, parentpid);
            kill(mypid, SIGTERM); // kill child
            break;
        case -1:
            printf("Error from parent process\n");
            break;
        default:
            mypid = getpid();
            parentpid = getppid();
            waitpid(pid, &status, 0);
            // printf("[This is printf from parent process where PID = %d - mypid %d - parentpid %d]\n", pid, mypid, parentpid);
            break;
        }
    }

    return 0;
}