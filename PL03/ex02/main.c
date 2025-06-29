#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MESSAGE_SIZE 50

typedef struct {
	int num;
	char name[MESSAGE_SIZE];
} Data;

int main() {
	int pipefd[2];
	Data data;
	
	printf("Enter a number: ");
	scanf("%d", &data.num);
	printf("Enter a name: ");
	scanf("%s", data.name);
	
	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(1);
	}
	
	pid_t pid = fork();
	
	if (pid > 0) {
		close(pipefd[0]);
		write(pipefd[1], &data, sizeof(Data));
		close(pipefd[1]);
	} else if (pid == 0) {
		close(pipefd[1]);
		read(pipefd[0], &data, sizeof(Data));
		printf("\nNumber received: %d\n", data.num);
		printf("Name received: %s\n", data.name);
		close(pipefd[0]);
	} else {
		perror("fork");
		exit(1);
	}
	
	return 0;
}
