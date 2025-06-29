#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include "shm.h"

int generate_num()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	unsigned int seed = (unsigned int)(getpid() ^ ts.tv_nsec ^ ts.tv_sec);
	return (rand_r(&seed) % 5) + 1;
}

int main()
{
	int fd, size = sizeof(message);
	fd = shm_open("/messageEx9", O_RDWR, 0);

	if (fd == -1)
	{
		perror("shm");
		exit(EXIT_FAILURE);
	}

	message *msg;

	if (ftruncate(fd, size) == -1)
	{
		perror("ftruncate");
		exit(EXIT_FAILURE);
	};
	msg = (message *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	while (msg->access == 1)
		;
	msg->access = 1;

	int generated = generate_num();
	msg->integer = generated;

	for (int i = 0; i < MAXLENGH; i++)
	{
		if (msg->message[i] == '\0')
		{
			break;
		}
		msg->message[i] = msg->message[i] + generated;
	}

	printf("Encrypted message: %s\n", msg->message);

	msg->access = 0;

	munmap(msg, size);

	return 0;
}
