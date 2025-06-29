#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREAD_COUNT 6

pthread_cond_t cond;
pthread_mutex_t mutex;
int ready = 0;

void *t1_work(void *arg)
{
    int threadIndex = (int)arg;
    buy_chips(threadIndex);

    pthread_mutex_lock(&mutex);
    ready++;

    if (ready < THREAD_COUNT)
    {
        pthread_cond_wait(&cond, &mutex);
    }
    else
    {
        pthread_cond_broadcast(&cond);
    }
    
    pthread_mutex_unlock(&mutex);

    eat_and_drink(threadIndex);

    pthread_exit(NULL);
}

void *t2_work(void *arg)
{
    int threadIndex = (int)arg;
    buy_beer(threadIndex);

    pthread_mutex_lock(&mutex);
    ready++;

    if (ready < THREAD_COUNT)
    {
        pthread_cond_wait(&cond, &mutex);
    }
    else
    {
        pthread_cond_broadcast(&cond);
    }

    pthread_mutex_unlock(&mutex);

    eat_and_drink(threadIndex);

    pthread_exit(NULL);
}

void buy_chips(int in)
{
    printf("Thread %d bought chips.\n", in);
}
void buy_beer(int in)
{
    printf("Thread %d bought beer.\n", in);
}
void eat_and_drink(int in)
{
    printf("Thread %d eating and drinking.\n", in);
}

int main()
{
    pthread_t threads[THREAD_COUNT];

    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < THREAD_COUNT; i++)
    {
        if (i%2==0)
        {
            pthread_create(&threads[i], NULL, t1_work, (int)i);
        } else{
            pthread_create(&threads[i], NULL, t2_work, (int)i);
        }
    }
    
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        pthread_join(threads[i], NULL);
    }
    
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);

    return 0;
}
