#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MESSAGE_SIZE 50

int main() {
	int pipefd[2];
	char buffer[MESSAGE_SIZE];

	FILE *a = fopen("file.txt", "r");

	if (a == NULL) {
		perror("file");
		exit(1);
	}

	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(1);
	}

	pid_t pid = fork();
	
	if (pid == -1) {
		perror("fork");
		exit(1);
	}

	if (pid > 0) {
		close(pipefd[0]);

		while (fgets(buffer, sizeof(buffer), a) != NULL) {
			write(pipefd[1], buffer, strlen(buffer));
		}

		close(pipefd[1]);
		fclose(a);
	} else if (pid == 0) {
		close(pipefd[1]);

		while (read(pipefd[0], buffer, sizeof(buffer)) > 0) {
			printf("%s", buffer);
		}

		close(pipefd[0]);
	}

	return 0;
}
