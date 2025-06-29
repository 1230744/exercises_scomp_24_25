#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SIZE 2

int main(){
	pid_t pids[CHILDS];
	for(int i = 0; i < CHILDS; i++)
	{
		pids[i]=fork();
		if (pids[i] == -1)
		{
			printf("Error in fork.");
			return 1;
		}

		// APENAS OS FILHOS EXECUTAM
		if (pids[i] == 0 && i == 0)
		{
			printf("I am the first child %d.\n", getpid());
			sleep(5);
			exit(1);
		} else if (pids[i] == 0 && i == 1)
		{
			printf("I am the second child %d.\n", getpid());
			exit(2);
		}		
	}

	// APENAS O PAI CHEGA
	int status;
	printf("I am the father %d.\n", getpid());
	for (int i = 0; i < CHILDS; i++)
	{
		waitpid(pids[i], &status, 0);

		if (WIFEXITED(status))
		{
        printf("Child %d exited with status of %d.\n", pids[i],WEXITSTATUS(status));
		}
	}

	return 0;
}
