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
        close(piped[1]); 

        // Redirect stdin to read from the pipe
        dup2(piped[0], STDIN_FILENO);
        close(piped[0]);

        execlp("more", "more", (char *)NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }
    
    //pai apenas
    close(piped[0]);
    char buffer[1024];
    FILE *f=fopen("fx.txt", "r");

    while(fgets(buffer, sizeof(buffer),f)){
        write(piped[1], buffer, strlen(buffer));
    }

    fclose(f);
    close(piped[1]);

    waitpid(pid, NULL, 0);

    return 0;
}
