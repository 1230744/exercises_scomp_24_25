#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main()
{
    int piped[2];
    if (pipe(piped) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        // Child process
        close(piped[1]);

        char dir[100];
        ssize_t n = read(piped[0], dir, sizeof(dir) - 1);
        if (n < 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        dir[n] = '\0'; // Null-terminate the string

        close(piped[0]);

        execlp("ls", "ls", dir, (char *)NULL);
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    // Parent process
    close(piped[0]);
    char buffer[100];

    printf("Enter the directory: ");
    if (scanf("%99s", buffer) != 1)
    {
        perror("scanf");
    }

    write(piped[1], buffer, strlen(buffer));
    close(piped[1]);

    waitpid(pid, NULL, 0);
    return 0;
}
