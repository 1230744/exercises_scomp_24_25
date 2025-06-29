#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#define NUM_CHILDS 5

#define NAME_SIZE 64

struct product
{
	int barcode;
	char name[NAME_SIZE];
	double price;
};

struct product products[10] = {
	{1001, "Product A", 10.99},
	{1002, "Product B", 20.49},
	{1003, "Product C", 15.75},
	{1004, "Product D", 30.00},
	{1005, "Product E", 25.50},
	{1006, "Product F", 12.99},
	{1007, "Product G", 18.25},
	{1008, "Product H", 22.10},
	{1009, "Product I", 14.99},
	{1010, "Product J", 29.99}};

int main()
{
	pid_t pids[NUM_CHILDS];
	int toParent[2];
	int toChild[NUM_CHILDS][2];

	if (pipe(toParent) == -1)
	{
		perror("pipe child");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < NUM_CHILDS; i++)
	{
		if (pipe(toChild[i]) == -1)
		{
			perror("pipe child");
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < NUM_CHILDS; i++)
	{
		pids[i] = fork();
		if (pids[i] == -1)
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}
		if (pids[i] == 0)
		{
			int code[2];
			code[0] = 1002+i; // apenas para teste
			code[1] = i;

			close(toParent[0]);
			write(toParent[1], code, sizeof(code));

			struct product prod;
			close(toChild[i][1]);
			read(toChild[i][0], &prod, sizeof(struct product));
			printf("Product received: %d - %s - %.2f\n", prod.barcode, prod.name, prod.price);
			exit(EXIT_SUCCESS);
		}
	}

	// para o pai
	close(toParent[1]);
	for (int i = 0; i < NUM_CHILDS; i++)
	{
		close(toChild[i][0]);
	}

	int barcode;
	int processIndex;

	int code[2];

	while (read(toParent[0], &code, sizeof(code)) > 0)
	{
		barcode = code[0];
		processIndex = code[1];

		int total = (int)(sizeof(products) / sizeof(struct product));

		for (int i = 0; i < total; i++)
		{
			if (products[i].barcode == barcode)
			{
				write(toChild[processIndex][1], &products[i], sizeof(struct product));
			}
		}
	}
	for (int i = 0; i < NUM_CHILDS; i++)
	{
		wait(NULL);
	}
	return 0;
}
