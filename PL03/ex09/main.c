#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include <time.h>

int generateRandom()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	srand((unsigned int)(ts.tv_nsec ^ ts.tv_sec ^ getpid()));
	return (rand() % 5) + 1;
}

int main()
{
	srand(time(NULL) * getpid());
	int toParent[2];
	int toSon[2];

	if (pipe(toParent) == -1 || pipe(toSon) == -1)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	pid_t pid = fork();
	double credit = 20;

	if (pid == -1)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (pid == 0)
	{
		// Child process
		close(toSon[1]); // Close unused write end
		close(toParent[0]);
		int buffer;
		double balance;
		while (read(toSon[0],&buffer, sizeof(int)) > 0)
		{
			if (buffer == 1)
			{
				int num = generateRandom();
				write(toParent[1], &num, sizeof(int));
			}
			else if (buffer == 0)
			{
				printf("Credits ended!\n");
				close(toSon[0]);
				close(toParent[1]);
				exit(EXIT_SUCCESS);
			}

			read(toSon[0],&balance, sizeof(double));
			printf("Your balance is: %f\n", balance);
		}
	}
	// Parent process
	close(toParent[1]);
	close(toSon[0]);
	while (credit > 0)
	{
		int generated = generateRandom();
		printf("Parent generated: %d\n", generated);

		int signal = 1;
		write(toSon[1],&signal, sizeof(int));

		int received;
		read(toParent[0],&received, sizeof(int)); // This will fail, as read end is closed. Fix: use two pipes.

		printf("Parent received: %d\n", received);

		if (received == generated)
		{
			credit = credit + 10;
		}
		else
		{
			credit = credit - 5;
		}

		write(toSon[1],&credit, sizeof(double));
	}

	int signal = 0;
	write(toSon[1],&signal, sizeof(int));

	close(toSon[1]);
	close(toParent[0]);

	waitpid(pid, NULL, 0);

	return 0;
}
