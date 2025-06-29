#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

typedef struct {
	int arr[10];
	int available;
}shared_data_type;

void populate (int arr[]){
	srand(time(NULL)); // Seed the random number generator
	for(int i=0; i<10; i++){
		arr[i] = rand() % 20 + 1; // Generate random numbers between 1 and 20
	}
}
   
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

	shared_data->available=1;
	populate(shared_data->arr);
	for(int i=0; i<10; i++){
		printf("%d ", shared_data->arr[i]);
	}
	shared_data->available=0;
	
	return 0;
}
