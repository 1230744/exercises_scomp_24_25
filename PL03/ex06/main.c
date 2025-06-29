#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define NUM_CHILDS 5
#define ARRAYLEN 1000
#define CHUNK_SIZE (ARRAYLEN / NUM_CHILDS)

void populate(int arr[]) {
	for (int i = 0; i < ARRAYLEN; i++) {
		arr[i] = rand() % 100; // Populate with random values between 0 and 99
	}
}

int main() {
	pid_t arrayPIDS[NUM_CHILDS];
	int pipefd[NUM_CHILDS][2]; // One pipe per child
	int arr1[ARRAYLEN], arr2[ARRAYLEN], result[ARRAYLEN];

	populate(arr1);
	populate(arr2);

	for (int i = 0; i < NUM_CHILDS; i++) {
		if (pipe(pipefd[i]) == -1) {
			perror("pipe");
			exit(1);
		}

		arrayPIDS[i] = fork();

		if (arrayPIDS[i] == -1) {
			perror("fork");
			exit(1);
		}

		if (arrayPIDS[i] == 0) { // Child process
			close(pipefd[i][0]); // Close read end of the pipe
			int partial_result[CHUNK_SIZE];

			for (int j = 0; j < CHUNK_SIZE; j++) {
				partial_result[j] = arr1[i * CHUNK_SIZE + j] + arr2[i * CHUNK_SIZE + j];
			}

			write(pipefd[i][1], &partial_result, sizeof(partial_result));
			close(pipefd[i][1]); // Close write end of the pipe
			exit(0);
		}
	}

	// Parent process
	for (int i = 0; i < NUM_CHILDS; i++) {
		close(pipefd[i][1]); // Close write end of the pipe
	}

	for (int i = 0; i < NUM_CHILDS; i++) {
		int partial_result[CHUNK_SIZE];
		read(pipefd[i][0], partial_result, sizeof(partial_result));
		close(pipefd[i][0]); // Close read end of the pipe

		for (int j = 0; j < CHUNK_SIZE; j++) {
			result[i * CHUNK_SIZE + j] = partial_result[j];
		}
	}

	for (int i = 0; i < NUM_CHILDS; i++) {
		int status;
		waitpid(arrayPIDS[i], &status, 0);
		if (WIFEXITED(status)) {
			printf("Child %d with PID %d ended successfully.\n", i + 1, arrayPIDS[i]);
		}
	}

	// Print the result array (optional)
	printf("Result array:\n");
	for (int i = 0; i < ARRAYLEN; i++) {
		printf("%d ", result[i]);
		if ((i + 1) % 20 == 0) printf("\n"); // Print 20 numbers per line
	}

	return 0;
}
