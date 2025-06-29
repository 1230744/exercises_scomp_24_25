#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>

#define ARRAY_SIZE 1000
#define NUM_CHILDREN 10
#define SEGMENT_SIZE (ARRAY_SIZE / NUM_CHILDREN)

typedef struct {
	int v[ARRAY_SIZE];
	int max[NUM_CHILDREN];
} shared_data_type;

int main() {
	int fd, data_size = sizeof(shared_data_type);
	shared_data_type *shared_data;

	fd = shm_open("/shmtest", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		perror("shm_open failed");
		exit(EXIT_FAILURE);
	}

	if (ftruncate(fd, data_size) == -1) {
		perror("ftruncate failed");
		exit(EXIT_FAILURE);
	}

	shared_data = (shared_data_type *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (shared_data == MAP_FAILED) {
		perror("mmap failed");
		exit(EXIT_FAILURE);
	}

	srand(time(NULL)+getpid());
	for (int i = 0; i < ARRAY_SIZE; i++) {
		shared_data->v[i] = rand() % 1000; // Random numbers between 0 and 999
	}

	for (int i = 0; i < NUM_CHILDREN; i++) {
		pid_t pid = fork();
		if (pid == -1) {
			perror("fork failed");
			exit(EXIT_FAILURE);
		}

		if (pid == 0) { // Child process
			int start = i * SEGMENT_SIZE;
			int end = start + SEGMENT_SIZE;
			int local_max = shared_data->v[start];

			for (int j = start + 1; j < end; j++) {
				if (shared_data->v[j] > local_max) {
					local_max = shared_data->v[j];
					printf("New local max founded %d in process %d.\n",local_max, i);
				}
			}

			shared_data->max[i] = local_max;
			munmap(shared_data, data_size);
			close(fd);
			exit(EXIT_SUCCESS);
		}
	}

	for (int i = 0; i < NUM_CHILDREN; i++) {
		wait(NULL);
	}


	// Calcula o valor máximo de entre os máximos guardados no array
	int global_max = shared_data->max[0];
	for (int i = 1; i < NUM_CHILDREN; i++) {
		if (shared_data->max[i] > global_max) {
			global_max = shared_data->max[i];
		}
	}

	printf("Global maximum: %d\n", global_max);

	munmap(shared_data, data_size);
	close(fd);
	if (shm_unlink("/shmtest") == -1) {
		perror("shm_unlink failed");
		exit(EXIT_FAILURE);
	}

	return 0;
}
