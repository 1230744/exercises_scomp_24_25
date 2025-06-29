#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include "shm.h"
#include <sys/stat.h>
#include <sys/mman.h>

#define NUMREQUESTS 10

int main()
{
    sem_t *sem = sem_open("/bank", O_CREAT | O_EXCL, 0644, 1);
    sem_t *sem1 = sem_open("/bank1", O_CREAT | O_EXCL, 0644, 0);

    int fd = shm_open("/banking", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(fd, SIZE);

    account *shared = (account *)mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // Initialize account
    shared->number = 12345;
    shared->amount = 1000.0;
    sem_init(&shared->semaphore, 1, 1);

    printf("Server started. Account %d initialized with balance: %.2f\n", 
           shared->number, shared->amount);

    for (int i = 0; i < NUMREQUESTS; i++)
    {
        printf("Waiting for request %d...\n", i + 1);
        sem_wait(sem1);
        printf("Request %d processed. Current balance: %.2f\n", 
               i + 1, shared->amount);
		fflush(stdout);
    }

    sem_close(sem);
    sem_close(sem1);
    sem_unlink("/bank");
    sem_unlink("/bank1");
    munmap(shared, SIZE);
    close(fd);
    shm_unlink("/banking");

    return 0;
}
