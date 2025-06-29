#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#define NUM_CHILDS 5
#define ARRAYLEN 1000

void populate(int arr[])
{
	for (int i = 0; i < ARRAYLEN; i++)
	{
		arr[i] = rand() % 100/3*3478;
	}
}

int main()
{
	pid_t arrayPIDS[NUM_CHILDS];
	int pipefd[2];

	int arr1[ARRAYLEN], arr2[ARRAYLEN];
	populate(arr1);
	populate(arr2);

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		exit(1);
	}

	for (int i = 0; i < NUM_CHILDS; i++)
	{
		arrayPIDS[i] = fork();

		if (arrayPIDS[i] == -1)
		{
			perror("fork");
			exit(1);
		}

		if (arrayPIDS[i] == 0)
		{
			int soma = 0;
			for (int j = i * 200; j < i * 200 + 200; j++)
			{
				soma = soma + arr1[j] + arr2[j];
			}
			close(pipefd[0]);
			write(pipefd[1], &soma, sizeof(int));
			close(pipefd[1]);
			exit(0);
		}
	}

	// Parent process
	close(pipefd[1]); // Close unused write end

	pid_t p;
	int status;
	int somaTotal = 0;
	for (int i = 0; i < NUM_CHILDS; i++)
	{
		int soma;
		read(pipefd[0], &soma, sizeof(int));
		//printf("%d\n", soma);
		somaTotal = somaTotal + soma;

		p = waitpid(arrayPIDS[i], &status, 0);
		if (WIFEXITED(status))
		{
			printf("Child %d with PID %d ended with value %d\n", i + 1, p, WEXITSTATUS(status));
		}
	}
	close(pipefd[0]);

	printf("Total sum: %d\n", somaTotal);

	return 0;
}
