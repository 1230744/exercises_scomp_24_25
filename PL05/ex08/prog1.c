#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "shm.h"
#include <time.h>

int main()
{
	sem_t *sem = sem_open("/controll", O_CREAT | O_EXCL, 0644, 0);
	sem_t *sem1 = sem_open("/controll1", O_CREAT | O_EXCL, 0644, 0);
	int fd = shm_open("/tickets", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(fd, SIZE);

	ticket_t *shared;
	shared = (ticket_t *)mmap(NULL, SIZE, PROT_WRITE, MAP_SHARED, fd, 0);

	int sold = 0;
	for (int i = 0; i < NUMBER_OF_TICKETS; i++)
	{
		if (sold <= NUMBER_OF_TICKETS)
		{
			sem_wait(sem1);
			shared->number = (unsigned int)time(NULL) ^ (unsigned int)getpid() ^ (unsigned int)i;
			printf("Sold ticket - %d\n", shared->number);
			fflush(stdout);
			sem_post(sem);
			sold++;
		}
	}

	munmap(shared, SIZE);
	shm_unlink("/tickets");

	sem_unlink("/controll");
	sem_close(sem);
	sem_unlink("/controll1");
	sem_close(sem1);

	return 0;
}
