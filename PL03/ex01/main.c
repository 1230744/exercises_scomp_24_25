#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/wait.h>

#define MESSAGE_SIZE 50

int main(){
	int pipefd[2];
	char buffer[MESSAGE_SIZE];
	int len;
	
	printf("My PID is %d\n", getpid());
	
	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(1);
	}
	
	pid_t pid = fork();
	
	if(pid>0){
		close(pipefd[0]); 
		len = snprintf(buffer, MESSAGE_SIZE, "%d", getpid());
		write(pipefd[1], buffer, len);
		close(pipefd[1]);
		exit(0);
    } else if(pid==0){
		close(pipefd[1]);
		int bytesRead = read(pipefd[0], buffer, MESSAGE_SIZE - 1);
		if(bytesRead > 0) {
			buffer[bytesRead] = '\0';
			printf("\nParent received: %s\n", buffer);
		}
	}
	 
	close(pipefd[0]); 

	return 0;
}
