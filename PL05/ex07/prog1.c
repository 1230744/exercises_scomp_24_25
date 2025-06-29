#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>

int main()
{
	pid_t pids[3];
	sem_t *semaphores[6];

	// Criação dos semáforos
	for (int i = 0; i < 6; i++)
	{
		char sem_name[16];
		snprintf(sem_name, sizeof(sem_name), "/semaphore%d", i);
		semaphores[i] = sem_open(sem_name, O_CREAT | O_EXCL, 0644, (i == 0) ? 1 : 0);
		if (semaphores[i] == SEM_FAILED)
		{
			perror("sem_open");
			exit(EXIT_FAILURE);
		}
	}

	const char *words[6] = {
		"Sistemas ", "de ",
		"Computadores ", "is ",
		"the ", "best!\n"};

	for (int i = 0; i < 3; i++)
	{
		pids[i] = fork();
		if (pids[i] == -1)
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}
		if (pids[i] == 0)
		{
			for (int k = 0; k < 2; k++)
			{
				int index = i * 2 + k;

				sem_wait(semaphores[index]);
				printf("%s", words[index]);
				fflush(stdout); // Garante impressão imediata
				if (index + 1 < 6)
					sem_post(semaphores[index + 1]);
			}
			exit(EXIT_SUCCESS);
		}
	}

	for (int i = 0; i < 3; i++)
	{
		waitpid(pids[i], NULL, 0);
	}

	for (int i = 0; i < 6; i++)
	{
		char sem_name[16];
		snprintf(sem_name, sizeof(sem_name), "/semaphore%d", i);
		sem_unlink(sem_name);
		sem_close(semaphores[i]);
	}

	return 0;
}
