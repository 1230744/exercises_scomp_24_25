#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

#define NUM_MESSAGES 10

int main()
{
	sem_t *sem = sem_open("/syncSem1", O_CREAT | O_EXCL | O_RDWR, 0644, 1);
	sem_t *sem1 = sem_open("/syncSem2", O_CREAT | O_EXCL | O_RDWR, 0644, 0);
	pid_t pid = fork();

	if (pid == 0)
	{
		for (int i = 0; i < NUM_MESSAGES; i++)
		{
			sem_wait(sem1);
			printf("I'm the child.\n");
			sem_post(sem);
		}
		exit(EXIT_SUCCESS);
	}

	for (int i = 0; i < NUM_MESSAGES; i++)
	{
		sem_wait(sem);
		printf("I'm the parent.\n");
		sem_post(sem1);
	}

	waitpid(pid, NULL, 0);

	sem_unlink("/syncSem1");
	sem_close(sem);
	sem_unlink("/syncSem2");
	sem_close(sem1);

	return 0;
}
