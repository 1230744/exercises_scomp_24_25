#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_PROCESSES 10
#define NUM_RECORDS 10

pid_t pids[NUM_PROCESSES];

typedef struct
{
	char name[50];
	char region[50];
	int phone;
} record_t;

record_t workshops[10] = {
	{"Workshop 1", "Region A", 123456789},
	{"Workshop 2", "Region B", 987654321},
	{"Workshop 3", "Region C", 456789123},
	{"Workshop 4", "Region D", 321654987},
	{"Workshop 5", "Region E", 789123456},
	{"Workshop 6", "Region F", 654321789},
	{"Workshop 7", "Region G", 159753486},
	{"Workshop 8", "Region H", 753159852},
	{"Workshop 9", "Region I", 951753486},
	{"Workshop 10", "Region J", 852963741}};

void handleSig(int sig)
{
	for (int i = 0; i < NUM_PROCESSES; i++)
	{
		kill(pids[i], SIGKILL); // Terminate all child processes
	}
}

int main()
{
	struct sigaction sa;
	memset(&sa, 0, sizeof(struct sigaction));
	sa.sa_handler = handleSig; // Ignore SIGUSR1 signal
	sigaction(SIGUSR1, &sa, NULL);

	char input[100];
	printf("Insira o nome da região: ");
	if (fgets(input, sizeof(input), stdin) != NULL)
	{
		size_t len = strlen(input);
		if (len > 0 && input[len - 1] == '\n')
		{
			input[len - 1] = '\0';
		}
	}

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
			int start = i * (NUM_RECORDS / NUM_PROCESSES);
			int end = (i == NUM_PROCESSES - 1) ? NUM_RECORDS : start + (NUM_RECORDS / NUM_PROCESSES);
			//printf("%d - %d ", start, end);
			for (int k = start; k < end; k++)
			{
				if (strcmp(workshops[k].region, input) == 0)
				{
					printf("Process %d found matching record: %s, %s, %d\n", getpid(), workshops[k].name, workshops[k].region, workshops[k].phone);
					kill(getppid(), SIGUSR1); // Notify parent process
					//exit(EXIT_SUCCESS);
				}
			}
			//printf("Process %d did not find a matching record.\n", getpid());
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < NUM_PROCESSES; i++)
	{
		waitpid(pids[i], NULL, 0); // Wait for all child processes to finish
	}

	return 0;
}
