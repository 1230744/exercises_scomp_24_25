#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>

typedef struct {
	char string[100];
}shared_data_type;
   
// programa 2
int main() {
	int fd, data_size = sizeof(shared_data_type);
	shared_data_type *shared_data;

	fd = shm_open("/shmtest2", O_RDWR, 0);

	if (fd == -1) {
		perror("shm_open failed");
		exit(EXIT_FAILURE);
	}

	if (ftruncate (fd, data_size)==-1) {
		perror("ftruncate failed");
		exit(EXIT_FAILURE);
	}

	shared_data = (shared_data_type *)mmap(NULL, data_size, PROT_READ, MAP_SHARED, fd, 0);
	while (shared_data->string[0] == '\0');
 	printf("%s\n", shared_data->string);

	return 0;
}
