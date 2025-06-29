#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

typedef struct {
    int arr[10];
    int r; // Reader flag
    int w; // Writer flag
} shared_data_type;

void populate(int arr[]) {
    srand(time(NULL) ^ getpid()); // Seed the random number generator
    for (int i = 0; i < 10; i++) {
        arr[i] = rand() % 20 + 1; // Generate random numbers between 1 and 20
    }
}

// Program 1 (Writer)
int main() {
    int fd, data_size = sizeof(shared_data_type);
    shared_data_type *shared_data;

    fd = shm_open("/shmtest2", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);

    if (fd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, data_size) == -1) {
        perror("ftruncate failed");
        exit(EXIT_FAILURE);
    }

    shared_data = (shared_data_type *)mmap(NULL, data_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);

    if (shared_data == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    shared_data->r = 0;
    shared_data->w = 0; 

    for (int j = 0; j < 5; j++) {
        // Wait until the reader has finished reading
        while (shared_data->r == 1);

        shared_data->w = 1;

        // Populate the array with random numbers
        populate(shared_data->arr);

        // Print the sequence
        printf("Sequence %d: ", j + 1);
        for (int i = 0; i < 10; i++) {
            printf("%d ", shared_data->arr[i]);
        }
        printf("\n");

        // Set the reader flag to indicate data is ready
        shared_data->w = 0;
        shared_data->r =1;
    }

    munmap(shared_data, data_size);
    shm_unlink("/shmtest2");

    return 0;
}
