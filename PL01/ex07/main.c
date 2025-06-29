#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define SIZE 1000

void M (char* s){
	printf("M('%s')\n", s);
}

int main()
{
	pid_t pid;
	for (int i = 0; i < 2; i++)
	{
		pid = fork();

		if (pid == 0){
			M("B");
			pid = fork();
		}
		if (pid != 0){
			M("A");
		}	
	}

	//printf("counter\n");
	
	return 0;
}
