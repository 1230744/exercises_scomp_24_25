#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "shm.h"

int main()
{
	int fd, size = sizeof(message);
	fd= shm_open("/messageEx9", O_CREAT |O_EXCL|O_RDWR, S_IRUSR | S_IWUSR);

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

	msg = (message*) mmap(NULL, size, PROT_WRITE,MAP_SHARED,fd,0);

	while (msg->access==1);
	msg->access=1;

	printf("Enter a message: ");
	fgets(msg->message, MAXLENGH, stdin);

	// Remove o '\n' no final se existir
	int len = strlen(msg->message);
	if (len > 0 && msg->message[len-1] == '\n') {
		msg->message[len-1] = '\0';
	}

	msg->access=0;

	munmap(msg, size);

	return 0;
}
