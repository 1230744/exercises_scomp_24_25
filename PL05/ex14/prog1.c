#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <sys/mman.h>
#include "message.h"

int main()
{
	int fd = shm_open("/message", O_RDWR, 0);
	string_t *shared = mmap(NULL, sizeof(string_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	sem_t *mutex = sem_open("/mutex", 0);
	sem_t *wrt = sem_open("/wrt", 0);
	sem_t *readCountMutex = sem_open("/readCountMutex", 0);

	sem_wait(readCountMutex);
	shared->readerCount++;
	if (shared->readerCount == 1)
		sem_wait(wrt);
	sem_post(readCountMutex);

	sleep(8); // para dar tempo de rodar outro reader

	sem_wait(readCountMutex);
	int readers = shared->readerCount;
	sem_post(readCountMutex);

	if (readers >= 5)
	{
		printf("There are 5 readers executing now...");
		sem_close(mutex);
		sem_close(wrt);
		sem_close(readCountMutex);
		munmap(shared, sizeof(string_t));
		shm_unlink("/message");
		sem_unlink("/mutex");
		sem_unlink("/wrt");
		sem_unlink("/readCountMutex");
		exit(EXIT_FAILURE);
	}

	if (readers > 2)
	{
		printf("There are more than 2 readers waiting now...");
		sem_close(mutex);
		sem_close(wrt);
		sem_close(readCountMutex);
		munmap(shared, sizeof(string_t));
		shm_unlink("/message");
		sem_unlink("/mutex");
		sem_unlink("/wrt");
		sem_unlink("/readCountMutex");
		exit(EXIT_FAILURE);
	}

	printf("Reader PID %d: Message - %s (Readers: %d)\n", getpid(), shared->message, readers);
	fflush(stdout);

	sem_wait(readCountMutex);
	shared->readerCount--;
	if (shared->readerCount == 0)
		sem_post(wrt);
	sem_post(readCountMutex);

	sem_close(mutex);
	sem_close(wrt);
	sem_close(readCountMutex);
	munmap(shared, sizeof(string_t));
	shm_unlink("/message");
	sem_unlink("/mutex");
	sem_unlink("/wrt");
	sem_unlink("/readCountMutex");

	return 0;
}
