#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define LENGT 1000
#define THRE 3
#define PORTION (LENGT / THRE)

int array[LENGT];
int lowest;
int highest;
float average;

void *find_lowest(void *arg)
{
    lowest = array[0];

    for (int i = 1; i < LENGT; i++)
    {
        if (array[i] < lowest)
        {
            lowest = array[i];
        }
    }
    pthread_exit(EXIT_SUCCESS);
}

void *find_highest(void *arg)
{
    highest = array[0];

    for (int i = 1; i < LENGT; i++)
    {
        if (array[i] > highest)
        {
            highest = array[i];
        }
    }
    pthread_exit(EXIT_SUCCESS);
}

void *calculate_average(void *arg)
{
    long long sum = 0;
    for (int i = 0; i < LENGT; i++)
    {
        sum = sum + array[i];
    }

    average = (float) sum / LENGT;

    pthread_exit(EXIT_SUCCESS);
}

int main()
{
    pthread_t threads[THRE];

    for (int i = 0; i < LENGT; i++)
    {
        array[i] = (rand() ^ i)%10;
        //printf("%d\n", array[i]);
    }

    pthread_create(&threads[0], NULL, find_lowest, NULL);
    pthread_create(&threads[1], NULL, find_highest, NULL);
    pthread_create(&threads[2], NULL, calculate_average, NULL);

    for (int i = 0; i < THRE; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("highest balance: %d\n", highest);
    printf("lowest balance: %d\n", lowest);
    printf("average balance: %.2f\n", average);
    return 0;
}
