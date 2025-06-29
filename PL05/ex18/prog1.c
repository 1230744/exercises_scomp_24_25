#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string.h>
#include <sys/wait.h>

#define N 10
#define PLAYERS 25

typedef struct
{
	int players;
} table_t;

typedef struct
{
	table_t tables[N]
} table_room_t;

int find_table_w_one(table_room_t *tablesShared)
{
	for (int i = 0; i < N; i++)
	{
		if (tablesShared->tables[i].players == 1)
		{
			return i;
		}
	}

	for (int i = 0; i < N; i++)
	{
		if (tablesShared->tables[i].players == 0)
		{
			return i;
		}
	}
	return (rand()%N);
}

int main()
{
	pid_t pids[PLAYERS];
	int size = sizeof(table_room_t);
	int fd = shm_open("/ballpool", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1 || ftruncate(fd, size) == -1)
	{
		perror("shared memory");
		exit(1);
	}

	table_room_t *shared = (table_room_t *)mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);

	for (int i = 0; i < N; i++) {
		shared->tables[i].players = 0;
	}

	sem_t *semaphore = sem_open("/tablesControl", O_CREAT | O_EXCL, 0644, 1);

	if (semaphore == SEM_FAILED)
	{
		perror("semaphore");
		exit(1);
	}

	for (int i = 0; i < PLAYERS; i++)
	{
		pids[i] = fork();
		if (pids[i] == -1)
		{
			perror("fork");
			exit(1);
		}

		if (pids[i] == 0)
		{
			for (int k = 0; k < N; k++)			
			{
				sem_wait(semaphore);
				int table_to_join = find_table_w_one(shared);
				printf("Player %d is trying to join table %d\n", i, table_to_join);
				
				if (shared->tables[table_to_join].players >= 2)
				{
					printf("Player %d can't join table %d - IT'S FULL\n", i, table_to_join);
					sem_post(semaphore);
					continue;
				}
				else
				{
					shared->tables[table_to_join].players++;
					printf("Player %d joined the table %d\n", i, table_to_join);
					sem_post(semaphore);
					break;
				}
			}
			exit(EXIT_SUCCESS);
		}
	}

	for (int i = 0; i < PLAYERS; i++) {
		waitpid(pids[i], NULL, 0);
	}

	sem_close(semaphore);
	sem_unlink("/tablesControl");

	munmap(shared, size);
	close(fd);
	shm_unlink("/ballpool");

	return 0;
}
