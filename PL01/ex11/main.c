#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/wait.h>

char create_twins(pid_t list[2]) {
    for (int i = 0; i < 2; i++) {
        list[i] = fork();

        if (list[i] < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if (list[i] == 0) {
            // Child process
            char c = (i == 0) ? 'a' : 'b';
            printf("Child '%c' PID: %d, Parent PID: %d\n", c, getpid(), getppid());
            exit(c);  // Return char as int (ASCII value)
        }
    }

    return 'p';
}

int main() {
    pid_t list[2];
    int num_twins = 3;

    for (int i = 0; i < num_twins; i++) {
        char ret = create_twins(list);
        if (ret == 'p') {
            for (int j = 0; j < 2; j++) {
                int status;
                pid_t pid = waitpid(list[j], &status, 0);
                if (pid > 0 && WIFEXITED(status)) {
                    char c = (char) WEXITSTATUS(status);
                    printf("Parent: Child PID %d exited with '%c'\n", pid, c);
                }
            }
        }
    }

    return 0;
}
