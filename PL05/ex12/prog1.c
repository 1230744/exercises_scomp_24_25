#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <sys/mman.h>

#define PROCESSES 2
#define LOOP 30

typedef struct
{
	int buffer[10];
	int head;
	int tail;
	sem_t *semaphore[2];
} circ_buff_t;

int main()
{
	pid_t pids[PROCESSES];
	int fd, size = sizeof(circ_buff_t);
	fd = shm_open("/circular-buff", O_CREAT | O_RDWR | O_EXCL, 0);

	if (fd == -1)
	{
		perror("shm");
		exit(EXIT_FAILURE);
	}

	if (ftruncate(fd, size) == -1)
	{
		perror("ftruncate");
		exit(EXIT_FAILURE);
	}

	circ_buff_t *shared = (circ_buff_t *)mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, 0);

	sem_t *sem = sem_open("/regu-circ-buff", O_CREAT | O_EXCL, 0644, 0);
	sem_t *sem1 = sem_open("/regu-circ-buff-write", O_CREAT | O_EXCL, 0644, 1);


	if (sem == SEM_FAILED)
	{
		perror("sem_open");
		exit(EXIT_FAILURE);
	}

	shared->semaphore[0]=sem;
	shared->semaphore[1]=sem1;
	shared->head=0;
	shared->tail=0;

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
			for (int k = 0; k < LOOP/PROCESSES; k++)
			{	
				sem_wait(shared->semaphore[1]);
				while (((shared->tail + 1) % 10) == shared->head); // wait if buffer is full
				int num = rand() % 100; 
				shared->buffer[shared->tail] = num;
				printf("Producer %d inserted %d in position %d\n", getpid(), num, shared->tail);
				shared->tail = (shared->tail + 1) % 10;
				sem_post(shared->semaphore[0]); 
			}
			exit(EXIT_SUCCESS);
		}
	}

	for (int i = 0; i < LOOP; i++)
	{
		sem_wait(shared->semaphore[0]);
		int consumed = shared->buffer[shared->head];
		printf("Consumer consumed %d from position %d\n", consumed, shared->head);
		shared->head = (shared->head + 1) % 10;

		printf("Consumer Buffer: [");
		for (int j = 0; j < 10; j++) {
			printf("%d", shared->buffer[j]);
			if (j < 9) printf(", ");
		}
		printf("]\n");

		sem_post(shared->semaphore[1]);
	}

	for (int i = 0; i < PROCESSES; i++)
	{
		waitpid(pids[i], NULL, 0);
	}

	sem_unlink("/regu-circ-buff");
	sem_close(sem);
	sem_unlink("/regu-circ-buff-write");
	sem_close(sem1);

	munmap(shared, size);
	shm_unlink("/circular-buff");
	return 0;
}
