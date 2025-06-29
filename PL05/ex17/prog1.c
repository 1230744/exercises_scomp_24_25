#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string.h>
#include <sys/wait.h>

#define ROOM_MAX_VISITORS 5
#define VISITORS_TODAY 23
#define ROOMS 10

typedef struct
{
	int visitors;
} room_t;

typedef struct
{
	room_t showrooms[ROOMS];
} showrooms_t;

int main()
{
	pid_t pids[VISITORS_TODAY];
	int fd = shm_open("/showrooms", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1 || ftruncate(fd, sizeof(showrooms_t)))
	{
		perror("shm");
		exit(1);
	}
	sem_t *access = sem_open("/showroomacess", O_CREAT | O_EXCL, 0644, 1);

	if (access == SEM_FAILED)
	{
		perror("semaphore");
		exit(1);
	}

	showrooms_t *shared = (showrooms_t *)mmap(NULL, sizeof(showrooms_t), PROT_WRITE, MAP_SHARED, fd, 0);

	for (int i = 0; i < VISITORS_TODAY; i++)
	{
		pids[i] = fork();
		if (pids[i] == -1)
		{
			perror("fork");
			exit(1);
		}

		if (pids[i] == 0)
		{
			for (int k = 0; k < VISITORS_TODAY; k++)
			{
				int room_to_visit = (rand() % ROOMS);
				printf("Visitor %d is trying to enter room %d\n", i, room_to_visit);

				sem_wait(access);
				if (shared->showrooms[room_to_visit].visitors > ROOM_MAX_VISITORS)
				{
					printf("The room %d is full.\n", room_to_visit);
					sem_post(access);
					continue;
				}
				else
				{
					shared->showrooms[room_to_visit].visitors++;
				}
				sem_post(access);

				printf("Visitor %d entered the room %d.\n", i, room_to_visit);

				srand(getpid() ^ time(NULL));
				int sleep_time = (rand() % 10) + 1;
				sleep(sleep_time);

				sem_wait(access);
				shared->showrooms[room_to_visit].visitors--;
				sem_post(access);
				printf("Visitor %d exited the room %d.\n", i, room_to_visit);
				exit(EXIT_SUCCESS);
			}
		}
	}

	for (int i = 0; i < VISITORS_TODAY; i++)
	{
		waitpid(pids[i], NULL, 0);
	}

	munmap(shared, sizeof(showrooms_t));
	close(fd);
	shm_unlink("/showrooms");
	sem_close(access);
	sem_unlink("/showroomacess");

	return 0;
}
