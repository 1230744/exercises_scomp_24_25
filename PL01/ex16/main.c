#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

int main()
{
    while (1)
    {
        char input[MAX_INPUT];
        char *args[MAX_ARGS];

        printf("MyShell> ");
        fflush(stdout);

        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            perror("fgets failed");
            continue;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "end") == 0) {
            break;
        }

        int argc = 0;
        char *token = strtok(input, " ");
        while (token != NULL && argc < MAX_ARGS - 1) {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }
        args[argc] = NULL;

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            continue;
        }

        if (pid == 0) {
            // Processo filho: executa o comando completo
            execvp(args[0], args);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        } else {
            // Processo pai: espera o filho terminar
            int status;
            waitpid(pid, &status, 0);
        }
    }

    printf("MyShell terminated.\n");
    return 0;
}
