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
        close(piped[1]);

        dup2(piped[0], STDIN_FILENO);
        close(piped[0]);

        execlp("./factorial", "./factorial", (char *)NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    // pai apenas
    close(piped[0]);
    int num;
    char buffer[32];

    printf("Enter a number to calculate: ");
    if (scanf("%d", &num) != 1)
    {
        perror("scanf");
    }

    snprintf(buffer, sizeof(buffer), "%d\n", num);
    write(piped[1], buffer, strlen(buffer));

    close(piped[1]);

    waitpid(pid, NULL, 0);
    return 0;
}
