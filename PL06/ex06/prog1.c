#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREADS 5
#define NUMBERS_PER_THREAD 200
#define FILENAME "numbers.txt"

pthread_mutex_t file_mutex;

void *write_numbers(void *arg) {
    int thread_num = *(int *)arg;
    FILE *fp;

    pthread_mutex_lock(&file_mutex);
    fp = fopen(FILENAME, "a");
    if (!fp) {
        perror("fopen");
        pthread_mutex_unlock(&file_mutex);
        pthread_exit(NULL);
    }
    for (int i = 0; i < NUMBERS_PER_THREAD; i++) {
        fprintf(fp, "Thread %d: %d\n", thread_num, i);
    }
    fclose(fp);
    pthread_mutex_unlock(&file_mutex);

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[THREADS];
    int thread_ids[THREADS];

    FILE *fp = fopen(FILENAME, "w");
    if (!fp) {
        perror("fopen");
        return 1;
    }
    fclose(fp);

    pthread_mutex_init(&file_mutex, NULL);

    for (int i = 0; i < THREADS; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&threads[i], NULL, write_numbers, &thread_ids[i]);
    }

    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&file_mutex);

    fp = fopen(FILENAME, "r");
    if (!fp) {
        perror("fopen");
        return 1;
    }
    char line[128];
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    fclose(fp);

    return 0;
}
