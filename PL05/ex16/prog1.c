#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string.h>
#include <sys/wait.h>

#define PRIORITIES 4
#define QUEUE_SIZE 3
#define MSG_SIZE 32
#define PRODUCERS 2
#define CONSUMERS 3
#define MESSAGES_PER_PRODUCER 5

typedef struct {
	char data[MSG_SIZE];
} message_t;

typedef struct {
	message_t queue[QUEUE_SIZE];
	int head, tail, count;
	sem_t mutex;
	sem_t full;
	sem_t empty;
} msg_queue_t;

typedef struct {
	msg_queue_t queues[PRIORITIES];
} shm_t;

void producer(shm_t *shm, int id) {
	for (int m = 0; m < MESSAGES_PER_PRODUCER; m++) {
		int prio = rand() % PRIORITIES;
		msg_queue_t *q = &shm->queues[prio];

		sem_wait(&q->empty);
		sem_wait(&q->mutex);

		snprintf(q->queue[q->tail].data, MSG_SIZE, "P%d-M%d-PRIO%d", id, m, prio);
		q->tail = (q->tail + 1) % QUEUE_SIZE;
		q->count++;

		printf("Producer %d: Sent message '%s' to priority %d\n", id, q->queue[(q->tail + QUEUE_SIZE - 1) % QUEUE_SIZE].data, prio);

		sem_post(&q->mutex);
		sem_post(&q->full);

		//usleep(100000 + rand() % 200000);
	}
	exit(0);
}

void consumer(shm_t *shm, int id) {
	int consumed = 0;
	while (1) {
		int found = 0;
		for (int prio = 0; prio < PRIORITIES; prio++) {
			msg_queue_t *q = &shm->queues[prio];
			if (sem_trywait(&q->full) == 0) {
				sem_wait(&q->mutex);

				char msg[MSG_SIZE];
				strncpy(msg, q->queue[q->head].data, MSG_SIZE);
				q->head = (q->head + 1) % QUEUE_SIZE;
				q->count--;

				printf("Consumer %d: Got message '%s' from priority %d\n\n", id, msg, prio);

				sem_post(&q->mutex);
				sem_post(&q->empty);

				consumed++;
				found = 1;
				break;
			}
		}
		if (!found) {
			// Check if all producers are done and queues are empty
			int all_empty = 1;
			for (int prio = 0; prio < PRIORITIES; prio++) {
				msg_queue_t *q = &shm->queues[prio];
				sem_wait(&q->mutex);
				if (q->count > 0) all_empty = 0;
				sem_post(&q->mutex);
			}
			if (all_empty) break;
			usleep(100000);
		}
	}
	exit(0);
}

int main() {
	int fd = shm_open("/msgqueues", O_CREAT | O_RDWR, 0666);
	ftruncate(fd, sizeof(shm_t));
	shm_t *shm = mmap(NULL, sizeof(shm_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	for (int i = 0; i < PRIORITIES; i++) {
		shm->queues[i].head = shm->queues[i].tail = shm->queues[i].count = 0;
		sem_init(&shm->queues[i].mutex, 1, 1);
		sem_init(&shm->queues[i].full, 1, 0);
		sem_init(&shm->queues[i].empty, 1, QUEUE_SIZE);
	}

	for (int i = 0; i < PRODUCERS; i++) {
		if (fork() == 0) producer(shm, i);
	}
	for (int i = 0; i < CONSUMERS; i++) {
		if (fork() == 0) consumer(shm, i);
	}

	for (int i = 0; i < PRODUCERS + CONSUMERS; i++) wait(NULL);

	for (int i = 0; i < PRIORITIES; i++) {
		sem_destroy(&shm->queues[i].mutex);
		sem_destroy(&shm->queues[i].full);
		sem_destroy(&shm->queues[i].empty);
	}
	munmap(shm, sizeof(shm_t));
	shm_unlink("/msgqueues");
	return 0;
}