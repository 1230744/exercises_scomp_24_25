#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>

#define SUSP_TIME 5

int main(int argc, char *argv[])
{
	// Verifica se o número de argumentos é correto
	if (argc != 2)
	{
		fprintf(stderr, "Uso: %s <programa>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int num_progs = argc - 1;

	pid_t pids[num_progs];

	for (int i = 0; i < num_progs; i++)
	{
		pids[i] = fork();

		// Código executado pelo filho apenas
		if (pids[i] == 0)
		{
			execlp(argv[i + 1], argv[i + 1], NULL);
			exit(EXIT_FAILURE);
		}
	}

	int active = num_progs;

	while (active > 0)
	{
		for (int i = 0; i < num_progs; i++)
		{
			if (pids[i] == -1)
			{
				continue;
			}

			int status;
			pid_t result = waitpid(pids[i], &status, WNOHANG);

			if (result == 0)
			{
				kill(pids[i], SIGCONT);
				sleep(SUSP_TIME);
				kill(pids[i], SIGSTOP);
			}
			else if (result == pids[i])
			{
				printf("Filho com PID %d terminou.\n", pids[i]);
				pids[i] = -1;
				active--;
			}
		}
	}
	return 0;
}
