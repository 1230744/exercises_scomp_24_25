#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void foo(int sig, siginfo_t *info, void *context)
{
    char msg[80];
    int length;

    length = snprintf(msg, 80, "I captured a SIGUSR1 sent by the process with PID %d.\n", info->si_pid);
    write(STDOUT_FILENO, msg, length);
}

int main()
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_sigaction = foo;
    sa.sa_flags = SA_SIGINFO | SA_RESTART;
    sigaction(SIGUSR1, &sa, NULL);

    // Wait for signal
    pause();

    return 0;
}
