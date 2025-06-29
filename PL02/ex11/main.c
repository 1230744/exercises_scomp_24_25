#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

volatile sig_atomic_t done = 0;

void handler(int sig) {
    done = 1;
}

void my_sleep(int seconds) {
    sigset_t mask, oldmask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);

    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    // Define handler para SIGUSR1
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

	alarm(seconds);
    printf("Waiting up to %d seconds for SIGUSR1...\n", seconds);

    while (!done) {
        sigsuspend(&oldmask); // espera até sinal não-bloqueado chegar
    }

    sigprocmask(SIG_SETMASK, &oldmask, NULL);
}

int main(int argc, char* argv[])
{
	int seconds = atoi(argv[1]);

	if (seconds <= 0)
	{
		fprintf(stderr, "Usage: %s <seconds>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	my_sleep(seconds);

	return 0;
}
