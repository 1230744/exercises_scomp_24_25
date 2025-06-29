#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

int main()
{
	sem_t *sem= sem_open("/syncSem", O_CREAT|O_EXCL|O_RDWR, 0644,0);
	pid_t pid = fork();

	if (pid==0)
	{
		sem_wait(sem);
		sleep(2); // para verificar se o filho 'recebe' na altura certa o semáfoto
		printf("I'm the child.\n");
		sem_post(sem);
		exit(EXIT_SUCCESS);
	}

	sleep(2); // para verificar se o filho não imprime primeiro
	printf("I'm the parent.\n");
	sem_post(sem);
	waitpid(pid, NULL, 0);

	sem_unlink("/syncSem");
	sem_close(sem);
	
	return 0;
}
