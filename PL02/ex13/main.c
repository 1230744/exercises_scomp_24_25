#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#define FILENAME "data.txt"

volatile sig_atomic_t sigusr1_received = 0;
volatile sig_atomic_t sigusr2_received = 0;
volatile sig_atomic_t sigint_received = 0;

void handle_sigusr1(int sig) { sigusr1_received = 1; }
void handle_sigusr2(int sig) { sigusr2_received = 1; }
void handle_sigint(int sig)  { sigint_received = 1; }

void block_signals(sigset_t *oldmask) {
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);
    sigdelset(&mask, SIGINT);
    sigprocmask(SIG_SETMASK, &mask, oldmask);
}

void restore_signals(sigset_t *oldmask) {
    sigprocmask(SIG_SETMASK, oldmask, NULL);
}

int main() {
    pid_t pid;
    struct sigaction sa_usr1, sa_usr2, sa_int;
    sigset_t oldmask;

    // Set up signal handlers
    memset(&sa_usr1, 0, sizeof(sa_usr1));
    sa_usr1.sa_handler = handle_sigusr1;
    sigaction(SIGUSR1, &sa_usr1, NULL);

    memset(&sa_usr2, 0, sizeof(sa_usr2));
    sa_usr2.sa_handler = handle_sigusr2;
    sigaction(SIGUSR2, &sa_usr2, NULL);

    memset(&sa_int, 0, sizeof(sa_int));
    sa_int.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa_int, NULL);

    block_signals(&oldmask);

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid > 0) { // Parent process
        while (!sigint_received) {
            // Wait for SIGUSR1 from child
            while (!sigusr1_received && !sigint_received)
                pause();
            if (sigint_received) break;
            sigusr1_received = 0;

            // Read number from file
            FILE *f = fopen(FILENAME, "r");
            int num = 0;
            if (f) {
                fscanf(f, "%d", &num);
                fclose(f);
            }
            printf("Parent read: %d\n", num);

            // Signal child with SIGUSR2
            kill(pid, SIGUSR2);
        }
        kill(pid, SIGINT); // Ensure child terminates
        wait(NULL);
    } else { // Child process
        int num = 0;
        while (!sigint_received) {
            // Clear file and write new number
            FILE *f = fopen(FILENAME, "w");
            if (f) {
                num++;
                fprintf(f, "%d\n", num);
                fclose(f);
            }

            // Signal parent with SIGUSR1
            kill(getppid(), SIGUSR1);

            // Wait for SIGUSR2 from parent
            while (!sigusr2_received && !sigint_received)
                pause();
            if (sigint_received) break;
            sigusr2_received = 0;
        }
    }

    restore_signals(&oldmask);
    return 0;
}
