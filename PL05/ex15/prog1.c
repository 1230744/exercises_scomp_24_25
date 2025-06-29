#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define PROCESSES 3
#define LOOP 10

typedef struct {
	int attended;
} line_wait_t;

int main() {
	pid_t pids[PROCESSES];

	sem_unlink("/desk1");
	sem_unlink("/desk2");

	sem_t *sem1 = sem_open("/desk1", O_CREAT | O_EXCL, 0644, 1);
	sem_t *sem2 = sem_open("/desk2", O_CREAT | O_EXCL, 0644, 0);

	if (sem1 == SEM_FAILED || sem2 == SEM_FAILED) {
		perror("sem_open");
		exit(EXIT_FAILURE);
	}

	int fd = shm_open("/clients-waiting", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1 || ftruncate(fd, sizeof(line_wait_t)) == -1) {
		perror("shm");
		exit(EXIT_FAILURE);
	}

	line_wait_t *shared = mmap(NULL, sizeof(line_wait_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	shared->attended = 0;

	for (int i = 0; i < PROCESSES; i++) {
		pids[i] = fork();
		if (pids[i] == -1) {
			perror("fork");
			exit(EXIT_FAILURE);
		}
		if (pids[i] == 0) {
			while (1) {
				sem_wait(sem2);

				sem_wait(sem1);
				if (shared->attended >= LOOP) {
					sem_post(sem1);
					break;
				}
				shared->attended++;
				int n = shared->attended;

				sleep((rand() % 10) + 1);

				printf("Cliente %d atendido no processo %d.\n", n, i);
				fflush(stdout);
				sem_post(sem1);
			}
			exit(EXIT_SUCCESS);
		}
	}

	int counter = 0;

	// Liberta os primeiros 3
	for (int i = 0; i < PROCESSES; i++) {
		sem_post(sem2);
	}

	while (counter < LOOP) {
		sem_wait(sem1);
		if (shared->attended > counter) {
			counter++;
			
			printf("Cliente %d saiu.\n", counter);
			sem_post(sem2);  // Liberta outro cliente
		}
		sem_post(sem1);
	}

	for (int i = 0; i < PROCESSES; i++) {
		waitpid(pids[i], NULL, 0);
	}

	sem_close(sem1);
	sem_close(sem2);
	sem_unlink("/desk1");
	sem_unlink("/desk2");
	munmap(shared, sizeof(line_wait_t));
	shm_unlink("/clients-waiting");

	return 0;
}
