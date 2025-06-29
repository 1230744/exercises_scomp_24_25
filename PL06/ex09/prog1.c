#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREAD_COUNT 5
#define TOTAL_VALUES 1000
#define PORTION (TOTAL_VALUES/THREAD_COUNT)

pthread_cond_t cond;
pthread_mutex_t mutex;
int data[TOTAL_VALUES];
int result[TOTAL_VALUES];

void *thread_calc(void *arg)
{
    int thread_index = (int) arg;

    for (int i = PORTION*thread_index; i < PORTION *(thread_index+1); i++)
    {
        pthread_mutex_lock(&mutex);
        result[i] = data[i]*10+2;
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_lock(&mutex);
    for (int i = PORTION*thread_index; i < PORTION *(thread_index+1); i++)
    {
        printf("Thread %d in position %d: %d\n", thread_index, i, result[i]);
    }
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main()
{
    pthread_t threads[THREAD_COUNT];

    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < TOTAL_VALUES; i++)
    {
        data[i] = (rand()^(i*2))%100;
    }
    

    for (int i = 0; i < THREAD_COUNT; i++)
    {
        pthread_create(&threads[i], NULL, thread_calc, (int)i);
    }
    
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        pthread_join(threads[i], NULL);
    }
    
    pthread_mutex_destroy(&mutex);

    return 0;
}
