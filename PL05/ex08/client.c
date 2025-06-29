#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include "shm.h"
#include <sys/mman.h>

void getTicket()
{
	sem_t *semaphore = sem_open("/controll", O_RDWR, 0644, 0);
	sem_t *semaphore1 = sem_open("/controll1", O_RDWR, 0644, 0);
	int fd = shm_open("/tickets", O_RDONLY, 0);
	ftruncate(fd, SIZE);

	ticket_t *shared;
	shared = (ticket_t *)mmap(NULL, SIZE, PROT_READ, MAP_SHARED, fd, 0);

	sem_post(semaphore1);
	sem_wait(semaphore);
	printf("\033[1;33mI bought the ticket: %d\033[0m\n", shared->number);

	munmap(shared, SIZE);
	sem_close(semaphore);
	sem_close(semaphore1);
}

int main()
{
	int option;

	do
	{
		printf("Menu:\n");
		printf("1. Get a ticket\n");
		printf("0. Exit\n");
		printf("Choose an option: ");
		if (scanf("%d", &option) != 1)
		{
			printf("Invalid input. Exiting.\n");
			break;
		}
		switch (option)
		{
		case 1:
			printf("Getting a ticket...\n");
			getTicket();
			break;
		case 0:
			printf("Exiting...\n");
			break;
		default:
			printf("Invalid option. Try again.\n");
		}
	} while (option != 0);

	return 0;
}
