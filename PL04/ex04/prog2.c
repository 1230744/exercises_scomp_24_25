#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>

typedef struct {
    int arr[10];
    int r; // Reader flag
    int w; // Writer flag
} shared_data_type;

// Program 2 (Reader)
int main() {
    int fd, data_size = sizeof(shared_data_type);
    shared_data_type *shared_data;

    fd = shm_open("/shmtest2", O_RDWR, 0);

    if (fd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    shared_data = (shared_data_type *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (shared_data == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    for (int sequence = 0; sequence < 5; sequence++) {
        printf("Waiting for sequence %d...\n", sequence + 1);

        // Wait until the writer has written data
        while (shared_data->w == 1);
        shared_data->r = 1;

        // Calculate the total sum and average
        int totalSum = 0;
        for (int i = 0; i < 10; i++) {
            totalSum += shared_data->arr[i];
        }

        float median = totalSum / 10.0;
        printf("The average of sequence %d is: %f\n", sequence + 1, median);

        // Reset the reader flag to indicate data has been processed
        shared_data->r = 0;
    }

    // Cleanup
    munmap(shared_data, data_size);

    return 0;
}
