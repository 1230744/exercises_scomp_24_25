#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    int pipe1[2], pipe2[2];
    pid_t pid1, pid2;

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if ((pid1 = fork()) == 0) {
        // FILHO 1: executa "sort"
        dup2(pipe1[0], STDIN_FILENO);   // Lê do pipe1
        dup2(pipe2[1], STDOUT_FILENO);  // Escreve no pipe2

        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        execlp("sort", "sort", (char *)NULL);
        perror("execlp sort");
        exit(EXIT_FAILURE);
    }

    if ((pid2 = fork()) == 0) {
        // FILHO 2: executa "tr"
        dup2(pipe2[0], STDIN_FILENO);   // Lê do pipe2

        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        execlp("tr", "tr", "a-z", "A-Z", (char *)NULL);
        perror("execlp tr");
        exit(EXIT_FAILURE);
    }

    // PAI
    close(pipe1[0]); // Pai escreve no pipe1
    close(pipe2[0]); // Pai não lê do pipe2
    close(pipe2[1]); // Pai não escreve no pipe2

    FILE *f = fopen("fx.txt", "r");
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), f)) {
        write(pipe1[1], buffer, strlen(buffer));
    }
    fclose(f);
    close(pipe1[1]); // Sinaliza EOF para sort

    wait(NULL);
    wait(NULL);
    return 0;
}
