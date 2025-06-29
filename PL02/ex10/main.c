#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

pid_t child_pid;

void alarm_handler(int sig)
{
	printf("\nyou are too slow\n");
	if (child_pid > 0)
	{
		kill(child_pid, SIGKILL); // Termina o filho
	}
	exit(1);
}

int main()
{
	char input[100];

	child_pid = fork();
	if (child_pid < 0)
	{
		perror("Fork failed");
		exit(EXIT_FAILURE);
	}

	if (child_pid == 0)
	{
		printf("Insira uma sentença: ");
		if (fgets(input, sizeof(input), stdin) != NULL)
		{
			size_t len = strlen(input);
			if (len > 0 && input[len - 1] == '\n')
			{
				input[len - 1] = '\0';
			}
			exit(0);
		}
		exit(1);
	}
	else
	{
		struct sigaction sa;
		memset(&sa, 0, sizeof(sa));
		sa.sa_handler = alarm_handler;
		sigemptyset(&sa.sa_mask);
		sigaddset(&sa.sa_mask, SIGUSR1);
		sigaddset(&sa.sa_mask, SIGUSR2);
		sigaction(SIGALRM, &sa, NULL);
		
		alarm(10);

		int status;
		waitpid(child_pid, &status, 0);

		alarm(0);
	}
	return 0;
}
