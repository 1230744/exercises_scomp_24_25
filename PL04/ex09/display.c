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

int main()
{
	int fd, size = sizeof(message);
	fd= shm_open("/messageEx9", O_RDWR, 0);

	if (fd==-1)
	{
		perror("shm");
		exit(EXIT_FAILURE);
	}
	
	message *msg;

	if(ftruncate(fd, size)==-1){
		perror("ftruncate");
		exit(EXIT_FAILURE);
	};
	msg = (message*) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	while (msg->access==1);
	msg->access=1;
	
	for (int i = 0; i < MAXLENGH; i++)
	{
		if (msg->message[i] == '\0')
		{
			break;
		}
		msg->message[i] = msg->message[i] - msg->integer;
	}

	printf("Final message: %s\n", msg->message);

	memset(msg->message, 0, MAXLENGH);

	msg->access=0;

	munmap(msg, size);
	shm_unlink("/messageEx9");

	return 0;
}
