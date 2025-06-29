#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include "shm.h"
#include <sys/mman.h>

int main()
{
    sem_t *sem = sem_open("/bank", 0);
    sem_t *sem1 = sem_open("/bank1", 0);
    
    if (sem == SEM_FAILED || sem1 == SEM_FAILED) {
        printf("Error: Server not running\n");
        return 1;
    }

    int fd = shm_open("/banking", O_RDWR, 0);
    if (fd == -1) {
        printf("Error: Cannot access shared memory\n");
        return 1;
    }

    account *shared = (account *)mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    sem_wait(sem);
    sem_wait(&shared->semaphore);

    printf("=== Account Information ===\n");
    printf("Account Number: %d\n", shared->number);
    printf("Current Balance: %.2f\n", shared->amount);
    printf("==========================\n");

    // Release access
    sem_post(&shared->semaphore);
    sem_post(sem);
    sem_post(sem1);

    // Cleanup
    sem_close(sem);
    sem_close(sem1);
    munmap(shared, SIZE);
    close(fd);

    return 0;
}
