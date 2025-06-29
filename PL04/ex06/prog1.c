#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define MESSAGE_SIZE 100

typedef struct {
	char message[MESSAGE_SIZE];
	int turn;
} shared_data_type;

int main() {
	int fd, data_size = sizeof(shared_data_type);
	shared_data_type *shared_data;
	int N;

	printf("Enter the number of child processes: ");
	scanf("%d", &N);

	// Create shared memory
	fd = shm_open("/shm_message", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
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

	shared_data->message[0] = '\0';
	shared_data->turn = 0;

	// Create child processes
	for (int i = 0; i < N; i++) {
		pid_t pid = fork();
		if (pid == -1) {
			perror("fork failed");
			exit(EXIT_FAILURE);
		}

		if (pid == 0) { 
			char word[MESSAGE_SIZE/N];

			while (shared_data->turn != i);

			printf("Child %d: Enter a word to append: ", i + 1);
			scanf("%s", word);

			strcat(shared_data->message, word);
			strcat(shared_data->message, " ");
			shared_data->turn++;

			munmap(shared_data, data_size);
			exit(EXIT_SUCCESS);
		}
	}

	// Wait for all child processes
	for (int i = 0; i < N; i++) {
		wait(NULL);
	}

	// Print the final message
	printf("Final message: %s\n", shared_data->message);

	// Cleanup
	munmap(shared_data, data_size);
	close(fd);
	shm_unlink("/shm_message");

	return 0;
}
