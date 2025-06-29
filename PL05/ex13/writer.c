#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include "message.h"

int main()
{
	int fd = shm_open("/message", O_CREAT | O_RDWR, 0666);
	ftruncate(fd, sizeof(string_t));
	string_t *shared = mmap(NULL, sizeof(string_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	sem_unlink("/mutex");
	sem_unlink("/wrt");
	sem_unlink("/readCountMutex");

	sem_t *mutex = sem_open("/mutex", O_CREAT, 0644, 1);
	sem_t *wrt = sem_open("/wrt", O_CREAT, 0644, 1);
	sem_t *readCountMutex = sem_open("/readCountMutex", O_CREAT, 0644, 1);

	for (int i = 0; i < LOOP; i++)
	{
		sem_wait(wrt);

		sem_wait(mutex);
		shared->writerCount++;
		int writers = shared->writerCount;
		sem_post(mutex);

		sem_wait(readCountMutex);
		int readers = shared->readerCount;
		sem_post(readCountMutex);

		time_t now = time(NULL);
		struct tm *t = localtime(&now);
		snprintf(shared->message, MESSAGE, "PID: %d, Time: %02d:%02d:%02d", getpid(), t->tm_hour, t->tm_min, t->tm_sec);

		printf("Writer PID %d: %s (Writers: %d, Readers: %d)\n", getpid(), shared->message, writers, readers);
		fflush(stdout);

		sem_wait(mutex);
		shared->writerCount--;
		sem_post(mutex);

		sem_post(wrt);
		sleep(2);
	}

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
