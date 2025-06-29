#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define MAXIMUM 64
#define NUM_READERS 3
#define NUM_WRITERS 2

typedef struct {
    char string[MAXIMUM];
    pthread_mutex_t mutex;
    pthread_cond_t can_read;
    pthread_cond_t can_write;
    int readers;
    int writers_waiting;
    int writers_active;
} shared;

void *reader(void *arg) {
    shared *sh = (shared *)arg;

    while (1) {
        pthread_mutex_lock(&sh->mutex);

        while (sh->writers_active > 0 || sh->writers_waiting > 0) {
            pthread_cond_wait(&sh->can_read, &sh->mutex);
        }
        sh->readers++;
        pthread_mutex_unlock(&sh->mutex);

        printf("Reader %lu: %s", pthread_self(), sh->string);
        printf("Number of readers: %d\n", sh->readers);

        pthread_mutex_lock(&sh->mutex);
        sh->readers--;
        if (sh->readers == 0) {
            pthread_cond_signal(&sh->can_write);
        }
        pthread_mutex_unlock(&sh->mutex);
    }
    return NULL;
}

void *writer(void *arg) {
    shared *sh = (shared *)arg;

    while (1) {
        pthread_mutex_lock(&sh->mutex);
        sh->writers_waiting++;
        while (sh->readers > 0 || sh->writers_active > 0) {
            pthread_cond_wait(&sh->can_write, &sh->mutex);
        }
        sh->writers_waiting--;
        sh->writers_active = 1;
        pthread_mutex_unlock(&sh->mutex);

        time_t now = time(NULL);
        snprintf(sh->string, MAXIMUM, "Writer %lu at %s", pthread_self(), ctime(&now));
        printf("Writer %lu wrote. Writers waiting: %d, Readers: %d\n",
               pthread_self(), sh->writers_waiting, sh->readers);


        pthread_mutex_lock(&sh->mutex);
        sh->writers_active = 0;
        pthread_cond_broadcast(&sh->can_read);
        pthread_cond_signal(&sh->can_write);
        pthread_mutex_unlock(&sh->mutex);
    }
    return NULL;
}

int main() {
    pthread_t readers[NUM_READERS], writers[NUM_WRITERS];
    int fd, size = sizeof(shared);

    fd = shm_open("/sharedT15", O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        fd = shm_open("/sharedT15", O_RDWR, S_IRUSR | S_IWUSR);
        if (fd == -1) {
            perror("shm_open");
            exit(1);
        }
    } else {
        if (ftruncate(fd, size) == -1) {
            perror("ftruncate");
            exit(1);
        }
    }

    shared *sh = (shared *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (sh == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    pthread_mutexattr_t mattr;
    pthread_condattr_t cattr;
    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_init(&cattr);
    pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&sh->mutex, &mattr);
    pthread_cond_init(&sh->can_read, &cattr);
    pthread_cond_init(&sh->can_write, &cattr);
    sh->readers = 0;
    sh->writers_waiting = 0;
    sh->writers_active = 0;
    snprintf(sh->string, MAXIMUM, "Initial message\n");

    for (int i = 0; i < NUM_READERS; i++)
        pthread_create(&readers[i], NULL, reader, sh);
    for (int i = 0; i < NUM_WRITERS; i++)
        pthread_create(&writers[i], NULL, writer, sh);

    for (int i = 0; i < NUM_READERS; i++)
        pthread_join(readers[i], NULL);
    for (int i = 0; i < NUM_WRITERS; i++)
        pthread_join(writers[i], NULL);

    pthread_mutex_destroy(&sh->mutex);
    pthread_cond_destroy(&sh->can_read);
    pthread_cond_destroy(&sh->can_write);

    munmap(sh, size);
    shm_unlink("/sharedT15");

    return 0;
}