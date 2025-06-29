#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

typedef struct {
    char cmd[32];
    int time;
} command_t;

command_t commands[] = {
    {"sleep", 2},
    {"ls", 3},
    {"whoami", 1},
    {"sleep", 5}
};

void execute_command(command_t cmd) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }
    
    if (pid == 0) {
        execlp(cmd.cmd, cmd.cmd, NULL);
        perror("execlp failed");
        exit(1);
    } else { 
        int status;
        pid_t result;
        
        int elapsed_time = 0;
        while ((result = waitpid(pid, &status, WNOHANG)) == 0) {
            sleep(1);
            elapsed_time++;
            if (elapsed_time >= cmd.time) {
                printf("Command %s exceeded %d seconds. Terminating...\n", cmd.cmd, cmd.time);
                kill(pid, SIGKILL);
                break;
            }
        }
        
        if (result > 0) {
            printf("Command %s completed successfully.\n", cmd.cmd);
        }
    }
}

int main() {
    int num_commands = sizeof(commands) / sizeof(commands[0]);
    for (int i = 0; i < num_commands; i++) {
        printf("Executing command: %s\n", commands[i].cmd);
        execute_command(commands[i]);
    }
    return 0;
}