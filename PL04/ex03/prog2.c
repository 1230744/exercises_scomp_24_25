#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>

typedef struct
{
	int arr[10];
	int available;
} shared_data_type;

// programa 2
int main()
{
	int fd, data_size = sizeof(shared_data_type);
	shared_data_type *shared_data;

	fd = shm_open("/shmtest2", O_RDWR, 0);

	if (fd == -1)
	{
		perror("shm_open failed");
		exit(EXIT_FAILURE);
	}

	if (ftruncate(fd, data_size) == -1)
	{
		perror("ftruncate failed");
		exit(EXIT_FAILURE);
	}

	shared_data = (shared_data_type *)mmap(NULL, data_size, PROT_READ, MAP_SHARED, fd, 0);

	if (shared_data == MAP_FAILED)
	{
		perror("mmap failed");
		exit(EXIT_FAILURE);
	}

	int totalSum = 0;
	int dataAvailable = 0;

	// Check if data is available (non-zero values in the array)
	if (shared_data->available == 0)
	{
		for (int i = 0; i < 10; i++)
		{
			if (shared_data->arr[i] != 0)
			{
				dataAvailable = 1;
				break;
			}
		}

		if (dataAvailable)
		{
			for (int i = 0; i < 10; i++)
			{
				totalSum += shared_data->arr[i];
			}
			float median = totalSum / 10;
			printf("The average is: %f \n", median);
		}
		else
		{
			printf("No data available to read.\n");
		}
	}

	return 0;
}
