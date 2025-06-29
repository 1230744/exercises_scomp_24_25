#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_PROCESSES 10

pid_t pids[NUM_PROCESSES];

void handleSig(int sig)
{
	printf("Handled SIGUSR1.\n");
}

int main()
{
	for (int i = 0; i < NUM_PROCESSES; i++)
	{
		pids[i] = fork();
		if (pids[i] < 0)
		{
			perror("Fork failed");
			exit(EXIT_FAILURE);
		}

		if (pids[i] == 0)
		{
			struct sigaction sa;
			memset(&sa, 0, sizeof(struct sigaction));
			sa.sa_handler = handleSig;
			sigemptyset(&sa.sa_mask);
			sigaddset(&sa.sa_mask, SIGUSR2); // Block SIGUSR2 during handler
			sigaction(SIGUSR1, &sa, NULL);

			pause();

			exit(EXIT_SUCCESS);
		}
	}

	for (int i = 0; i < NUM_PROCESSES; i++)
	{
		sleep(2);
		kill(pids[i], SIGUSR1);
	}

	for (int i = 0; i < NUM_PROCESSES; i++)
	{
		waitpid(pids[i], NULL, 0); // Wait for all child processes to finish
	}

	return 0;
}
