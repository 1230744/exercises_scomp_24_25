#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define CHILDS 6
#define TOWRITE 100
#define END 600

int main()
{
	pid_t pids[CHILDS];

	for (int i = 0; i < CHILDS; i++)
	{
		pids[i]=fork();
		if (pids[i]==-1)
		{
			perror("Error on fork.");
			exit(EXIT_FAILURE);
		}
		if (pids[i]==0)
		{
			for (int j = i*TOWRITE; j < (i+1)*TOWRITE; j++)
			{
				printf("%d\n", j);
			}
			exit(EXIT_SUCCESS);
		}	
	}
	
	int status;
	for (int i = 0; i < CHILDS; i++)
	{
		waitpid(pids[i], &status, 0);
	}
	
	return 0;
}
