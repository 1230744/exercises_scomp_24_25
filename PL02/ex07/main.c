#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_PROGS 50
#define ARRAYL 50
#define numberToSearch 50

int success = 0;
int error = 0;

void handleSg1()
{
	printf("Signal received: SIGUSR1\n");
	success++;
}

void handleSg2()
{
	printf("Signal received: SIGUSR2\n");
	error++;
}

void populate(int arr[], int leng)
{
	for (int i = 0; i < leng; i++)
	{
		arr[i] = 324;
	}
}

int simulate1()
{
	usleep((rand() % 500 + 500) * 1000); // 0.5s a 1s de delay
	return rand() % 2;					 // 50% chance de sucesso
}

int simulate2()
{
	int arr[ARRAYL];
	populate(arr, ARRAYL);

	for (int i = 0; i < ARRAYL; i++)
	{
		if (arr[i] == numberToSearch)
		{
			return 1;
		}
	}
	return 0;
}

int main()
{
	srand(time(NULL));

	pid_t pids[NUM_PROGS];
	struct sigaction sa1, sa2;
	memset(&sa1, 0, sizeof(struct sigaction));
	memset(&sa2, 0, sizeof(struct sigaction));
	sa1.sa_handler = handleSg1;
	sa2.sa_handler = handleSg2;
	sa1.sa_flags = SA_RESTART;
	sa2.sa_flags = SA_RESTART;
	sigaction(SIGUSR1, &sa1, NULL);
	sigaction(SIGUSR2, &sa2, NULL);

	for (int i = 0; i < NUM_PROGS; i++)
	{
		pids[i] = fork();

		if (pids[i] < 0)
		{
			perror("fork failed");
			exit(EXIT_FAILURE);
		}

		// Código executado pelo filho apenas
		if (pids[i] == 0)
		{
			int res = simulate1();
			if (res == 1)
			{
				kill(getppid(), SIGUSR1);
			}
			else if (res == 0)
			{
				kill(getppid(), SIGUSR2);
			};
			pause();
			simulate2();
			exit(0);
		}
	}

	for (;;)
	{
		if (success + error == NUM_PROGS / 2)
		{
			if (success == 0)
			{
				printf("Inefficiennt Algorithm!\n)");
				for (int i = 0; i < NUM_PROGS; i++)
				{
					kill(pids[i], SIGKILL);
				}
				exit(EXIT_FAILURE);
			}
			else if (success >= 1)
			{
				for (int i = 0; i < NUM_PROGS; i++)
				{
					kill(pids[i], SIGCONT);
				}
				break;
			}
		}
		usleep(10000); // evita busy waiting intenso
	}

	for (int i = 0; i < NUM_PROGS; i++)
	{
		waitpid(pids[i], NULL, 0);
	}
	printf("Success: %d, Error: %d\n", success, error);

	return 0;
}
