#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define MAX_SIZE 64
#define PLANES 3
#define FLIGHTS 9

typedef struct {
    char id[MAX_SIZE];
    int x;
    int y;
} vec;

typedef struct {
    vec *all_fi;
    int plane_idx;
    vec *vec_out;
    int flights;
} filter_args;

typedef struct {
    vec *vec_in;
    int flights;
    int plane_idx;
} dist_args;

void *filter_plane(void *arg) {
    filter_args *args = (filter_args *)arg;
    int count = 0;
    for (int k = 0; k < args->flights; k++) {
        int idx = k * PLANES + args->plane_idx;
        args->vec_out[count++] = args->all_fi[idx];
    }
    free(arg);
    pthread_exit(NULL);
}

void *calc_distance(void *arg) {
    dist_args *args = (dist_args *)arg;
    double dist = 0.0;
    for (int i = 1; i < args->flights; i++) {
        int dx = args->vec_in[i].x - args->vec_in[i-1].x;
        int dy = args->vec_in[i].y - args->vec_in[i-1].y;
        dist += sqrt(dx*dx + dy*dy);
    }
    printf("Plane %d traveled distance: %.2f\n", args->plane_idx+1, dist);
    free(arg);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[PLANES * 2];
    vec *all_fi;
    int numberFlights = FLIGHTS;

    all_fi = malloc(PLANES * numberFlights * sizeof(vec));
    if (all_fi == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));
    for (int k = 0; k < numberFlights; k++) {
        for (int i = 0; i < PLANES; i++) {
            int idx = k * PLANES + i;
            snprintf(all_fi[idx].id, MAX_SIZE, "PLANE%d", i + 1);
            all_fi[idx].x = rand() % 1000;
            all_fi[idx].y = rand() % 1000;
        }
    }

    vec *vecs[PLANES];
    for (int i = 0; i < PLANES; i++) {
        vecs[i] = malloc(numberFlights * sizeof(vec));
        if (!vecs[i]) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < PLANES; i++) {
        filter_args *args = malloc(sizeof(filter_args));
        args->all_fi = all_fi;
        args->plane_idx = i;
        args->vec_out = vecs[i];
        args->flights = numberFlights;
        pthread_create(&threads[i], NULL, filter_plane, args);
    }

    for (int i = 0; i < PLANES; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < PLANES; i++) {
        dist_args *args = malloc(sizeof(dist_args));
        args->vec_in = vecs[i];
        args->flights = numberFlights;
        args->plane_idx = i;
        pthread_create(&threads[i], NULL, calc_distance, args);
    }

    for (int i = 0; i < PLANES; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < PLANES; i++) {
        free(vecs[i]);
    }
    free(all_fi);

    return 0;
}
