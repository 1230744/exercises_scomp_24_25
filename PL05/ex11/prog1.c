#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>

void buy_chips()
{
	printf("Process %d bought chips.\n", getpid());
	fflush(stdout);
}

void buy_beer()
{
	printf("Process %d bought beer.\n", getpid());
	fflush(stdout);
}

void eat_and_drink()
{
	printf("Process %d eating and drinking.\n", getpid());
	fflush(stdout);
}

#define PROCESSES 6

int main()
{
	pid_t pids[PROCESSES];
	sem_t *semaphores[PROCESSES];

	// Criação dos semáforos
	for (int i = 0; i < PROCESSES; i++)
	{
		char sem_name[16];
		snprintf(sem_name, sizeof(sem_name), "/sem11%d", i);
		semaphores[i] = sem_open(sem_name, O_CREAT | O_EXCL, 0644, 0);
		if (semaphores[i] == SEM_FAILED)
		{
			perror("sem_open");
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < PROCESSES; i++)
	{
		pids[i] = fork();
		if (pids[i] == -1)
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}
		if (pids[i] == 0)
		{
			if (i % 2 == 0)
			{
				buy_chips();
			}
			else
			{
				buy_beer();
			}

			for (int k = 0; k < PROCESSES; k++){
				sem_post(semaphores[i]);
			}

			for (int k = 0; k < PROCESSES; k++)
			{
				sem_wait(semaphores[k]);
			}
			
			eat_and_drink();
			
			exit(EXIT_SUCCESS);
		}
	}

	for (int i = 0; i < PROCESSES; i++)
	{
		waitpid(pids[i], NULL, 0);
	}

	for (int i = 0; i < PROCESSES; i++)
	{
		char sem_name[16];
		snprintf(sem_name, sizeof(sem_name), "/sem11%d", i);
		sem_unlink(sem_name);
		sem_close(semaphores[i]);
	}

	return 0;
}
