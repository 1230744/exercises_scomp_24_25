#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>

#define REPS 1000

typedef struct {
	int int1;
	int int2;
	int available;
}shared_data_type;

int main() {
	int fd, data_size = sizeof(shared_data_type);
	pid_t pid;
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

	shared_data->int1=10000;
	shared_data->int2=500;
	
	pid = fork();
	
	if(pid>0){
		for(int i=0; i<REPS; i++){
			if(shared_data->available){
				shared_data->available = 1;
				shared_data->int1--;
				shared_data->int2++;
				shared_data->available = 0;
			}
		}
	} else {
		for(int i=0; i<REPS; i++){
			if(shared_data->available){				
				shared_data->available = 1;
				shared_data->int1++;
				shared_data->int2--;
				shared_data->available = 0;
			}
		}
		exit(0);
	}
	
	waitpid(pid, NULL, 0);

	if (shared_data->available==0)
	{
		printf("Final int 1: %d \n", shared_data->int1);
		printf("Final int 2: %d \n", shared_data->int2);
	}
	
	return 0;
}
