#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define LOOP 30
#define CAPACITY 10

typedef struct
{
    int array[CAPACITY];
    int tail;
    int head;
    pthread_mutex_t mutex;
    pthread_cond_t new_data;
    pthread_cond_t data_read;
    int exchanges;
} circ_buffer_t;

void *producer(void *arg)
{
    circ_buffer_t *shared = (circ_buffer_t *)arg;
    while (1)
    {
        pthread_mutex_lock(&shared->mutex);

        if (shared->exchanges >= LOOP)
        {
            pthread_cond_broadcast(&shared->new_data); // Wake up consumer if waiting
            pthread_mutex_unlock(&shared->mutex);
            break;
        }

        while (((shared->tail + 1) % CAPACITY) == shared->head)
        {
            pthread_cond_wait(&shared->data_read, &shared->mutex);
        }

        int value = rand() % 100;
        shared->array[shared->tail] = value;
        shared->tail = (shared->tail + 1) % CAPACITY;
        shared->exchanges++;

        pthread_cond_signal(&shared->new_data);
        pthread_mutex_unlock(&shared->mutex);
    }
    pthread_exit(NULL);
}

void *consumer(void *arg)
{
    circ_buffer_t *shared = (circ_buffer_t *)arg;
    int consumed = 0;
    while (1)
    {
        pthread_mutex_lock(&shared->mutex);

        if (consumed >= LOOP)
        {
            pthread_mutex_unlock(&shared->mutex);
            break;
        }

        while (shared->head == shared->tail)
        {
            if (shared->exchanges >= LOOP)
            {
                pthread_mutex_unlock(&shared->mutex);
                pthread_exit(NULL);
            }
            pthread_cond_wait(&shared->new_data, &shared->mutex);
        }

        int value = shared->array[shared->head];
        printf("Consumed: %d\n", value);
        shared->head = (shared->head + 1) % CAPACITY;
        consumed++;

        pthread_cond_signal(&shared->data_read);
        pthread_mutex_unlock(&shared->mutex);
    }
    pthread_exit(NULL);
}

int main()
{
    pthread_t threads[3];
    circ_buffer_t *shared = malloc(sizeof(circ_buffer_t));

    pthread_mutex_init(&shared->mutex, NULL);
    pthread_cond_init(&shared->new_data, NULL);
    pthread_cond_init(&shared->data_read, NULL);
    shared->tail = 0;
    shared->head = 0;
    shared->exchanges=0;

    pthread_create(&threads[0], NULL, producer, (void *)shared);
    pthread_create(&threads[1], NULL, producer, (void *)shared);
    pthread_create(&threads[2], NULL, consumer, (void *)shared);

    for (int i = 0; i < 3; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_cond_destroy(&shared->data_read);
    pthread_cond_destroy(&shared->new_data);
    pthread_mutex_destroy(&shared->mutex);
    free(shared);

    return 0;
}