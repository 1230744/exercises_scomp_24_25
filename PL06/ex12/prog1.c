#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define SNAILS 3
#define MAX_POINTS 16

pthread_mutex_t mutex;

int matrix[15][16] = {
    {-1, 5, 5, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, 3, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, 5, 5, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, 6, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, 6, 4, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, 4, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 5, -1, -1, 5, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 5, 5, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 5, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3}
};

typedef struct
{
    int id;
    int path[16];
} caracol_path;

int times[SNAILS];

void *voyage(void *arg)
{
    caracol_path *snail = (caracol_path *)arg;
    int travel_time = 0;

    for (int i = 0; i < MAX_POINTS - 1; i++)
    {
        int from = snail->path[i];
        int to = snail->path[i + 1];

        if (from == -1 || to == -1)
            break;

        if (matrix[from][to] != -1)
        {
            travel_time += matrix[from][to];
        }
        else if (matrix[to][from] != -1)
        {
            travel_time += matrix[to][from];
        }
        else
        {
            printf("Caracol %d: caminho inválido de %d para %d\n", snail->id, from, to);
        }
    }

    printf("Travel time of %d: %ds\n", snail->id, travel_time);
    pthread_mutex_lock(&mutex);
    times[snail->id - 1] = travel_time;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main()
{
    pthread_t threads[SNAILS];

    caracol_path *caracoles[SNAILS];

    for (int i = 0; i < SNAILS; i++)
    {
        caracoles[i] = malloc(sizeof(caracol_path));
    }

    caracoles[0]->id = 1;
    int path0[16] = {0, 1, 4, 5, 6, 8, 9, 10, 11, 15, -1, -1, -1, -1, -1, -1};
    for (int j = 0; j < 16; j++)
        caracoles[0]->path[j] = path0[j];

    caracoles[1]->id = 2;
    int path1[16] = {0, 2, 4, 5, 7, 8, 9, 13, 14, 15, -1, -1, -1, -1, -1, -1};
    for (int j = 0; j < 16; j++)
        caracoles[1]->path[j] = path1[j];

    caracoles[2]->id = 3;
    int path2[16] = {0, 3, 4, 5, 7, 9, 10, 12, 15, -1, -1, -1, -1, -1, -1, -1};
    for (int j = 0; j < 16; j++)
        caracoles[2]->path[j] = path2[j];

    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < SNAILS; i++)
    {
        pthread_create(&threads[i], NULL, voyage, (void *)(caracoles[i]));
    }

    for (int i = 0; i < SNAILS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    int ranking[SNAILS];
    for (int i = 0; i < SNAILS; i++) {
        ranking[i] = i;
    }

    for (int i = 0; i < SNAILS - 1; i++) {
        for (int j = i + 1; j < SNAILS; j++) {
            if (times[ranking[i]] > times[ranking[j]]) {
                int tmp = ranking[i];
                ranking[i] = ranking[j];
                ranking[j] = tmp;
            }
        }
    }

    printf("\nClassificação dos caracóis:\n");
    for (int i = 0; i < SNAILS; i++) {
        printf("%dº lugar: Caracol %d com %ds\n", i + 1, ranking[i] + 1, times[ranking[i]]);
    }

    pthread_mutex_destroy(&mutex);

    for (int i = 0; i < SNAILS; i++)
    {
        free(caracoles[i]);
    }

    return 0;
}
