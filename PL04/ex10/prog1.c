#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define EXCHAN 30
#define BUFF_LEN 10

typedef struct
{
	int buffer[BUFF_LEN];
	int elements;
	int access;
	int head;
	int tail;
} circ_buff;

int main()
{
	int fd, size = sizeof(circ_buff);
	fd = shm_open("/ex10", O_CREAT | O_RDWR | O_EXCL, S_IWUSR | S_IRUSR);
	if (fd == -1)
	{
		perror("shm_open");
		exit(EXIT_FAILURE);
	}
	if (ftruncate(fd, size) == -1)
	{
		perror("ftruncate");
		exit(EXIT_FAILURE);
	}

	circ_buff *buffer;
	buffer = (circ_buff *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	buffer->elements = 0;
	buffer->access = 0;
	buffer->head = 0;
	buffer->tail = 0;

	for (int i = 0; i < 2; i++)
	{
		pid_t pid = fork();
		if (pid == -1)
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}
		if (pid == 0 && i == 0)
		{
			// PRODUTOR
			for (int j = 0; j < EXCHAN; j++)
			{
				while (buffer->access == 1)
					;
				buffer->access = 1;

				if (buffer->elements < BUFF_LEN)
				{
					buffer->buffer[buffer->tail] = rand() % 100;
					buffer->tail = (buffer->tail + 1) % BUFF_LEN;
					buffer->elements++;
				}

				buffer->access = 0;
			}
			exit(EXIT_SUCCESS);
		}
		if (pid == 0 && i == 1)
		{
			// CONSUMIDOR
			for (int j = 0; j < EXCHAN; j++)
			{
				while (buffer->access == 1);
				buffer->access = 1;

				if (buffer->elements > 0)
				{
					// Imprime todo o buffer
					printf("Buffer: [");
					for (int k = 0; k < BUFF_LEN; k++)
					{
						printf("%d", buffer->buffer[k]);
						if (k < BUFF_LEN - 1)
							printf(", ");
					}
					printf("]\n");
				}

				buffer->access = 0;
			}
			exit(EXIT_SUCCESS);
		}
	}

	wait(NULL);
	wait(NULL);

	munmap(buffer, size);
	shm_unlink("/ex10");

	return 0;
}
