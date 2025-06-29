#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#define SIZE 100000
#define CHILDS 5
#define RANGE (SIZE / CHILDS)

pid_t pids[CHILDS];
int numberFound = 0; // flag to stop race
int foundChild = 0;
int foundPos = -1;

// Populate array with unique random numbers
void populate(int *arr, int size) {
    for (int i = 0; i < size; i++) arr[i] = i;
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

// Signal handler to kill all children
void handle_SIGUSR1(int sig) {
    if (!numberFound) {
        numberFound = 1;
        for (int i = 0; i < CHILDS; i++) {
            if (pids[i] > 0) {
                kill(pids[i], SIGKILL);
				printf("Child %d killed by signal %d\n", i + 1, sig);
            }
        }
    }
}

int main() {
    srand(time(NULL));
    int arr[SIZE];
    populate(arr, SIZE);

    int numberToSearch = 1; // or choose your own value
    printf("Searching for number: %d\n", numberToSearch);

    // Install signal handler
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = handle_SIGUSR1;
    act.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &act, NULL);

    for (int i = 0; i < CHILDS; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            int start = i * RANGE;
            int end = start + RANGE;
            for (int j = start; j < end; j++) {
                if (arr[j] == numberToSearch) {
                    printf("Child %d found number at position %d\n", i + 1, j);
                    kill(getppid(), SIGUSR1); // notify parent
                    exit(i + 1);
                }
            }
            exit(0);
        }
    }

    int status;
    for (int i = 0; i < CHILDS; i++) {
        pid_t p = wait(&status);
        for (int j = 0; j < CHILDS; j++) {
            if (pids[j] == p && WIFEXITED(status)) {
                int exitCode = WEXITSTATUS(status);
                if (exitCode != 0 && !foundChild) {
                    foundChild = exitCode;
                }
                printf("Child %d with PID %d exited with %d\n", j + 1, p, exitCode);
            }
        }
    }

    if (foundChild) {
        printf("Number %d found by child %d\n", numberToSearch, foundChild);
    } else {
        printf("Number %d not found in array.\n", numberToSearch);
    }

    return 0;
}
