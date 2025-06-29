#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    int pipe1[2], pipe2[2];
    pid_t pid1, pid2, pid3;

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // First child: ls -la
    if ((pid1 = fork()) == 0) {
        dup2(pipe1[1], STDOUT_FILENO);  // Write to pipe1

        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        execlp("ls", "ls", "-la", (char *)NULL);
        perror("execlp ls");
        exit(EXIT_FAILURE);
    }

    // Second child: sort
    if ((pid2 = fork()) == 0) {
        dup2(pipe1[0], STDIN_FILENO);   // Read from pipe1
        dup2(pipe2[1], STDOUT_FILENO);  // Write to pipe2

        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        execlp("sort", "sort", (char *)NULL);
        perror("execlp sort");
        exit(EXIT_FAILURE);
    }

    // Third child: wc -l
    if ((pid3 = fork()) == 0) {
        dup2(pipe2[0], STDIN_FILENO);   // Read from pipe2

        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        execlp("wc", "wc", "-l", (char *)NULL);
        perror("execlp wc");
        exit(EXIT_FAILURE);
    }

    // Parent closes all pipe ends
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    wait(NULL);
    wait(NULL);
    wait(NULL);
    return 0;
}
