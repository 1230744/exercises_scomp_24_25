#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>

#define FILENAMEMAX 128

void add_line_to_file(const char *filename, int process_id)
{
	FILE *file = fopen(filename, "a");
	if (file == NULL)
	{
		perror("Failed to open file");
		exit(1);
	}

	fprintf(file, "\nI am the process with PID %d.\n", process_id);

	fclose(file);
}

int main()
{
	sem_t *sem = sem_open("/mutuexsem", O_CREAT | O_EXCL, 0644, 1);
	if (sem == SEM_FAILED)
	{
		perror("Failed to create sem");
		exit(1);
	}
	sem_unlink("/mutuexsem");

	char filename[FILENAMEMAX];
	if (scanf("%s", filename)==-1)
	{
		perror("scanner");
		exit(EXIT_FAILURE);
	}

	sem_wait(sem);
	add_line_to_file(filename, getpid());
	sleep(2);
	sem_post(sem);

	sem_close(sem);
	return 0;
}
