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
        perror("fork");
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
        close(piped[0]);

        // Redirect stdin to read from the pipe
        dup2(piped[1], STDOUT_FILENO);
        //close(piped[1]);

        execlp("sort", "sort", "fx.txt", (char *)NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    // pai apenas
    close(piped[1]);
    char buffer[1024];

    while (read(piped[0], buffer, strlen(buffer)))
    {
        printf("%s", buffer);
    }

    close(piped[0]);

    waitpid(pid, NULL, 0);

    return 0;
}
