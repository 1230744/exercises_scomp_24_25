#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    pid_t pids[argc];
    int nmrCommands = argc-1;
    for (int i = 0; i < nmrCommands; i++)
    {
        pids[i] = fork();

        if (pids[i] == -1)
        {
            perror("Error during fork!");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0)
        {
            execlp(argv[i+1], argv[i+1], NULL);
            perror("EXEC FUNCTION FAILED!");
            exit(EXIT_FAILURE);
        }
    }

    int status;
    for (int i = 0; i < nmrCommands; i++)
    {
        waitpid(pids[i], &status, 0);
    }
    return 0;
}
