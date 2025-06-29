#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define STR_SIZE 50
#define NR_COURSES 10
#define STD_NUMBER 20

typedef struct
{
	int number;
	char name[STR_SIZE];
	int courses[NR_COURSES];
	int access;
} student_t;

typedef struct
{
	student_t students[STD_NUMBER];
	int access;
} class;

int main()
{
	int fd, size = sizeof(class);
	fd = shm_open("/studentstest", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1)
	{
		perror("shm");
		exit(EXIT_FAILURE);
	}
	if (ftruncate(fd, size) == -1)
	{
		perror("ftruncate");
		exit(EXIT_FAILURE);
	}

	class *std;
	std = (class *)mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, 0);

	std->access = 1;

	for (int i = 0; i < 2; i++)
	{
		pid_t pid = fork();
		if (pid == -1)
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}

		if (pid == 0 && i == 0)
		{
			while (std->access == 1);
			std->access = 1;
			for (int i = 0; i < STD_NUMBER; i++)
			{
				int highest = std->students[i].courses[0];
				int lowest = std->students[i].courses[0];
				for (int j = 1; j < NR_COURSES; j++)
				{
					if (std->students[i].courses[j] > highest)
					{
						highest = std->students[i].courses[j];
					}
					if (std->students[i].courses[j] < lowest)
					{
						lowest = std->students[i].courses[j];
					}
				}
				printf("The highest grade of %s is %d and the lowest is %d.\n", std->students[i].name,highest, lowest);
				std->students[i].access = 0;
			}
			std->access = 0;

			exit(EXIT_SUCCESS);
		}
		if (pid == 0 && i == 1)
		{
			while (std->access == 1);
			std->access = 1;
			for (int i = 0; i < STD_NUMBER; i++)
			{
				std->students[i].access = 1;
				int sum = std->students[i].courses[0];
				for (int j = 1; j < NR_COURSES; j++)
				{
					sum = sum + std->students[i].courses[j];
				}
				double average = (double)sum / NR_COURSES;
				
				printf("The average of %s is %f.\n", std->students[i].name,average);
				std->students[i].access = 0;
			}
			std->access = 0;

			exit(EXIT_SUCCESS);
		}
	}

	for (int i = 0; i < STD_NUMBER; i++)
	{
		printf("Enter student number: ");
		scanf("%d", &std->students[i].number);

		printf("Enter student name: ");
		scanf(" %[^\n]", std->students[i].name);

		printf("Enter %d course grades:\n", NR_COURSES);
		for (int j = 0; j < NR_COURSES; j++)
		{
			printf("Grade %d: ", j + 1);
			scanf("%d", &std->students[i].courses[j]);
		}
	}
	std->access = 0;

	wait(NULL);
	wait(NULL);

	munmap(std, size);
	shm_unlink("/studentstest");

	return 0;
}
