#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>

#define NUM_PROCESSES 5
#define NUM_COUNT 200
#define FILENAME "output.txt"

void write_numbers_to_file(const char *filename, int process_id) {
	FILE *file = fopen(filename, "a");
	if (file == NULL) {
		perror("Failed to open file");
		exit(1);
	}

	for (int i = 1; i <= NUM_COUNT; i++) {
		fprintf(file, "Process %d: %d\n", process_id, i);
	}

	fclose(file);
}

int main() {
	pid_t pids[NUM_PROCESSES];
	sem_t *semaphores[NUM_PROCESSES];

	// Create and initialize semaphores
	for (int i = 0; i < NUM_PROCESSES; i++) {
		char sem_name[16];
		snprintf(sem_name, sizeof(sem_name), "/sem_%d", i);
		semaphores[i] = sem_open(sem_name, O_CREAT | O_EXCL, 0644, (i == 0) ? 1 : 0);
		if (semaphores[i] == SEM_FAILED) {
			perror("Failed to create semaphore");
			exit(1);
		}
		sem_unlink(sem_name);
	}

	for (int i = 0; i < NUM_PROCESSES; i++) {
		pids[i] = fork();

		if (pids[i] < 0) {
			perror("Fork failed");
			exit(1);
		}

		if (pids[i] == 0) {
			sem_wait(semaphores[i]);

			write_numbers_to_file(FILENAME, i + 1);

			if (i < NUM_PROCESSES - 1) {
				sem_post(semaphores[i + 1]);
			}

			exit(0);
		}
	}

	for (int i = 0; i < NUM_PROCESSES; i++) {
		waitpid(pids[i], NULL, 0);
	}

	// Close and destroy semaphores
	for (int i = 0; i < NUM_PROCESSES; i++) {
		sem_close(semaphores[i]);
	}

	// Display the contents of the file
	printf("Contents of the file:\n");
	FILE *file = fopen(FILENAME, "r");
	if (file == NULL) {
		perror("Failed to open file");
		exit(1);
	}

	char line[256];
	while (fgets(line, sizeof(line), file) != NULL) {
		printf("%s", line);
	}

	fclose(file);

	return 0;
}
