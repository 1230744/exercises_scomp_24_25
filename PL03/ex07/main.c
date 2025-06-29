#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define NUM_CHILDS 10

struct win{
	char message[5];
	int round;
};

int main() {
	pid_t arrayPIDS[NUM_CHILDS];
	
	int pipefd[2];
	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(1);
	}

	for (int i = 0; i < NUM_CHILDS; i++) {
		arrayPIDS[i] = fork();

		if (arrayPIDS[i] == -1) {
			perror("fork");
			exit(1);
		}

		if (arrayPIDS[i] == 0) { // Child process
			close(pipefd[1]);
			struct win received;
			while (read(pipefd[0], &received, sizeof(received)) > 0) {
				printf("Child PID: %d, Message: %s, Round: %d\n", getpid(), received.message, received.round);
			}
			close(pipefd[0]);
			if (sizeof(received) > 0) {
				exit(received.round);
			} else {
				exit(11);
			}
		}
	}

	// Parent process
	for(int i = 0; i<10; i++){
		sleep(2);
		struct win strwin;
		strncpy(strwin.message, "Win!", sizeof(strwin.message) - 1);
		strwin.message[sizeof(strwin.message) - 1] = '\0';
		strwin.round = i; 
		close(pipefd[0]);
		write(pipefd[1], &strwin, sizeof(strwin));
	}
	close(pipefd[1]);
	
	// Receive the win round of each child
	printf("\n\n");
	int status;
	for (int i = 0; i < 10; i++){
		pid_t pid2;
        pid2 = wait(&status);
        if(WIFEXITED(status))
        printf("Parent: child %d returned %d\n", pid2, WEXITSTATUS(status));
    }
	return 0;
}
