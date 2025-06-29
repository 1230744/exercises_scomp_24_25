#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define SIZE 1000

void populateArray(int array[])
{
	for (int i = 0; i < CHILDS; i++)
	{
		array[i] = (rand() % 10) + 1;
		//printf("%d\n",array[i]);
	}
}

void cleanArray(int array[])
{
	for (int i = 0; i < CHILDS; i++)
	{
		array[i]=0;
	}
}

int main()
{
	int data[CHILDS], result[CHILDS];
	populateArray(data);
	cleanArray(result);
	pid_t pid = fork();

	if (pid == -1)
	{
		printf("Error forking.");
	}

	if (pid == 0)
	{
		for (int i = CHILDS / 2; i < CHILDS; i++)
		{
			result[i] = data[i] * 4 + 20;
		}
		exit(0);
	}
	else
	{
		for (int i = 0; i < CHILDS / 2; i++)
		{
			result[i] = data[i] * 4 + 20;
		}
	}

	for (int i = 0; i < CHILDS; i++)
	{
		printf("%d - %d\n", i, result[i]);
	}

	return 0;
}
