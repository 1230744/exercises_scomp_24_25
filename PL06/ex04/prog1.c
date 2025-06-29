#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define LENGT 1000
#define THRE 5
#define PORTION (LENGT / THRE)

int array[LENGT];

void *search_num(void *num)
{
    int *number = (int *)num;

    for (int i = number[1] * PORTION; i < (number[1] + 1) * PORTION; i++)
    {
        if (array[i] == number[0])
        {
            printf("Found in position %d.\n", i);
            pthread_exit((void *)(int)number[1]);
        }
    }
    pthread_exit(NULL);
}

int main()
{
    pthread_t threads[THRE];

    for (int i = 0; i < LENGT; i++)
    {
        array[i] = rand()^i;
        //printf("%d\n", array[i]);
    }

    printf("Enter the num to search: ");
    int num;
    scanf("%d", &num);

    for (int i = 0; i < THRE; i++)
    {
        int *send = malloc(2 * sizeof(int));
        send[1] = i;
        send[0] = num;
        pthread_create(&threads[i], NULL, search_num, (void *)send);
    }

    for (int i = 0; i < THRE; i++)
    {
        void *ret_val;
        pthread_join(threads[i], &ret_val);
        if (ret_val != NULL)
        {
            printf("Thread %d exited with value %d\n", i, (int)ret_val);
        }
    }

    return 0;
}
