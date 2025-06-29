#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_cond_t cond;
pthread_mutex_t mutex;
int ready = 0;

void *t1_work(void *arg)
{
    int threadIndex = (int)arg;
    buy_chips(threadIndex);
    
    pthread_mutex_lock(&mutex);
    ready++;

    if (ready < 2)
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

    if (ready < 2)
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
    pthread_t threads[2];

    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&threads[0], NULL, t1_work, (int)0);
    pthread_create(&threads[1], NULL, t2_work, (int)1);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);

    return 0;
}
