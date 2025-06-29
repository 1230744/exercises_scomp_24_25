#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>

typedef struct {
	char string[100];
}shared_data_type;
   
// programa 1
int main() {
	int fd, data_size = sizeof(shared_data_type);
	shared_data_type *shared_data;

	fd = shm_open("/shmtest2", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);

	if (fd == -1) {
		perror("shm_open failed");
		exit(EXIT_FAILURE);
	}

	if (ftruncate (fd, data_size)==-1) {
		perror("ftruncate failed");
		exit(EXIT_FAILURE);
	}

	shared_data = (shared_data_type *)mmap(NULL, data_size, PROT_WRITE, MAP_SHARED, fd, 0);

	printf("Enter a message: ");
	if (fgets(shared_data->string, sizeof(shared_data->string), stdin) == NULL) {
		perror("Error reading input");
		exit(EXIT_FAILURE);
	}
	shared_data->string[strcspn(shared_data->string, "\n")] = '\0';

	return 0;
}
