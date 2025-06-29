#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_SIZE 64
#define CONNECTIONS 3
#define TRAINS 5

typedef struct {
    char origin[MAX_SIZE];
    char destination[MAX_SIZE];
    pthread_mutex_t mutex;
} connection_t;

typedef struct {
    connection_t conns[CONNECTIONS];
} network_t;

typedef struct {
    int train_num;
    network_t *network;
} train_arg_t;

void *voyage(void *arg) {
    train_arg_t *targ = (train_arg_t *)arg;
    int train_num = targ->train_num;
    network_t *net = targ->network;

    int conn_idx = rand() % CONNECTIONS;
    connection_t *conn = &net->conns[conn_idx];

    int duration = 1 + rand() % 5;

    pthread_mutex_lock(&conn->mutex);
    printf("Train %d: %s -> %s\n", train_num, conn->origin, conn->destination);
    sleep(duration);
    printf("Train %d: Trip duration %d seconds\n", train_num, duration);
    pthread_mutex_unlock(&conn->mutex);

    free(targ);
    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));
    pthread_t threads[TRAINS];
    network_t network;

    snprintf(network.conns[0].origin, MAX_SIZE, "Cidade A");
    snprintf(network.conns[0].destination, MAX_SIZE, "Cidade B");
    pthread_mutex_init(&network.conns[0].mutex, NULL);

    snprintf(network.conns[1].origin, MAX_SIZE, "Cidade B");
    snprintf(network.conns[1].destination, MAX_SIZE, "Cidade C");
    pthread_mutex_init(&network.conns[1].mutex, NULL);

    snprintf(network.conns[2].origin, MAX_SIZE, "Cidade B");
    snprintf(network.conns[2].destination, MAX_SIZE, "Cidade D");
    pthread_mutex_init(&network.conns[2].mutex, NULL);

    for (int i = 0; i < TRAINS; i++) {
        train_arg_t *targ = malloc(sizeof(train_arg_t));
        targ->train_num = i + 1;
        targ->network = &network;
        pthread_create(&threads[i], NULL, voyage, targ);
    }

    for (int i = 0; i < TRAINS; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < CONNECTIONS; i++) {
        pthread_mutex_destroy(&network.conns[i].mutex);
    }

    return 0;
}
