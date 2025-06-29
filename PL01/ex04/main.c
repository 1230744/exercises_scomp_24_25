#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
	pid_t pid[3];
	int i;
	for (i = 0; i < 3; i++)
	{
		pid[i] = fork();

		if (pid[i] == -1)
		{
			printf("Error in fork.");
			exit(1);
		}

		if (pid[i] == 0)
		{
			sleep(1);
			exit(i);
		}
	}

	int status;
	pid_t finished_pid;

	for (int i = 0; i < 3; i++)
	{
		finished_pid = waitpid(pid[i], &status, 0);
		if (WIFEXITED(status)) {
            int order = WEXITSTATUS(status);
            printf("Filho com PID %d terminou. Valor retornado (ordem): %d\n", finished_pid, order);
        } else {
            printf("Filho com PID %d terminou de forma anormal.\n", finished_pid);
        }
	}
	
	return 0;
}
