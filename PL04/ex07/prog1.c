#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define STR_SIZE 50
#define NR_COURSES 10

typedef struct{
	int number;
	char name[STR_SIZE];
	int courses[NR_COURSES];
	int access;
}student_t;

int main() {
	int fd, size = sizeof(student_t);
	fd = shm_open("/studentstest", O_CREAT|O_EXCL|O_RDWR, S_IRUSR | S_IWUSR);
	if (fd==-1)
	{
		perror("shm");
		exit(EXIT_FAILURE);
	}
	if (ftruncate(fd, size)==-1){
		perror("ftruncate");
		exit(EXIT_FAILURE);
	}

	student_t *std;
	std = (student_t*)mmap(NULL, size, PROT_WRITE,MAP_SHARED, fd, 0);

	std->access=1;

	for (int i = 0; i < 2; i++)
	{
		pid_t pid = fork();
		if (pid ==-1)
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}

		if(pid==0 && i ==0){
			while (std->access==1);
			std->access=1;			
			int highest = std->courses[0];
			int lowest=std->courses[0];
			for (int i = 1; i < NR_COURSES; i++)
			{
				if (std->courses[i]>highest)
				{
					highest = std->courses[i];
				}
				if (std->courses[i]<lowest)
				{
					lowest = std->courses[i];
				}				
			}
			std->access=0;		
			
			printf("The highest grade is %d and the lowest is %d.\n", highest, lowest);

			exit(EXIT_SUCCESS);
		}
		if(pid==0 && i ==1){
			while (std->access==1);
			std->access=1;			
			int sum = std->courses[0];
			for (int i = 1; i < NR_COURSES; i++)
			{
				sum = sum+std->courses[i];			
			}
			std->access=0;
			double average = sum/NR_COURSES;		
			
			printf("The average is %f.\n", average);

			exit(EXIT_SUCCESS);
		}
	}

	printf("Enter student number: ");
	scanf("%d", &std->number);

	printf("Enter student name: ");
	scanf(" %[^\n]", std->name);

	printf("Enter %d course grades:\n", NR_COURSES);
	for (int i = 0; i < NR_COURSES; i++) {
		printf("Grade %d: ", i + 1);
		scanf("%d", &std->courses[i]);
	}
	std->access=0;

	wait(NULL);
	wait(NULL);

	munmap(std, size);
	shm_unlink("/studentstest");

	return 0;
}
