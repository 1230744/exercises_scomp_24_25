#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define NUM_CHILDS 10
#define RECORDS 10
#define PORTION (RECORDS / NUM_CHILDS)

typedef struct
{
	int customer_code;
	int product_code;
	int quantity;
} record_t;

record_t records[RECORDS] = {
	{1001, 2001, 5},
	{1002, 2002, 10},
	{1003, 2003, 15},
	{1004, 2004, 20},
	{1005, 2005, 25},
	{1006, 2006, 30},
	{1007, 2007, 35},
	{1008, 2008, 40},
	{1009, 2009, 45},
	{1010, 2010, 50}};

int main()
{
	pid_t pids[NUM_CHILDS];
	int larger[RECORDS];
	int pipefd[2];

	if (pipe(pipefd) == -1)
	{
		perror("Pipe failed");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < NUM_CHILDS; i++)
	{
		pids[i] = fork();
		if (pids[i] < 0)
		{
			perror("Fork failed");
			exit(EXIT_FAILURE);
		}
		if (pids[i] == 0)
		{
			for (int k = i * PORTION; k < (i + 1) * PORTION; k++)
			{
				if (records[k].quantity > 20)
				{
					close(pipefd[0]); // Close read end in child
					write(pipefd[1], &records[k].product_code, sizeof(int));
					close(pipefd[1]); // Close write end after writing
				}
			}
			exit(EXIT_SUCCESS);
		}
	}

	// apenas pai chega
	close(pipefd[1]);
	int inputed = 0;
	for (int i = 0; i < NUM_CHILDS; i++)
	{
		int product_code;
		while (read(pipefd[0], &product_code, sizeof(int)) > 0)
		{
			larger[inputed] = product_code;
			inputed++;
			// printf("Parent received product code: %d\n", product_code);
		}

		waitpid(pids[i], NULL, 0);
	}
	close(pipefd[0]); 

	for (int i = 0; i < RECORDS; i++)
	{
		if (larger[i] != 0)
		{
			printf("Product code larger than 20: %d\n", larger[i]);
		}
	}

	return 0;
}
