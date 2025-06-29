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

#define FILENAMEMAX 128

void add_line_to_file(const char *filename, int process_id)
{
	FILE *file = fopen(filename, "a+");
	if (file == NULL)
	{
		perror("Failed to open file");
		exit(1);
	}

	int lines = 0;
	int ch;
	rewind(file);
	while ((ch = fgetc(file)) != EOF)
	{
		if (ch == '\n')
			lines++;
	}

	if (lines < 10)
	{
		fprintf(file, "\nI am the process with PID %d.\n", process_id);
	}

	fclose(file);
}

int main()
{
	sem_t *sem = sem_open("/mutuexsem", O_CREAT, 0644, 1);
	if (sem == SEM_FAILED)
	{
		perror("Failed to create sem");
		exit(1);
	}
	//sem_unlink("/mutuexsem");

	char filename[FILENAMEMAX];
	if (scanf("%s", filename)==-1)
	{
		perror("scanner");
		exit(EXIT_FAILURE);
	}

	struct timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
		perror("clock_gettime");
		exit(EXIT_FAILURE);
	}
	ts.tv_sec += 10;  // Espera máxima de 10 segundos

	if (sem_timedwait(sem, &ts) == -1)
	{
		if (errno == ETIMEDOUT)
		{
			fprintf(stderr, "Waited 10 seconds without access to the file! Terminating...\n");
		}
		else
		{
			perror("sem_timedwait failed");
		}
		exit(EXIT_FAILURE);
	}

	add_line_to_file(filename, getpid());
	sleep(50);
	sem_post(sem);

	sem_close(sem);
	return 0;
}
