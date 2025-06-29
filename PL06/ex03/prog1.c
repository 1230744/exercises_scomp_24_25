#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct {
    int number;
    char name[50];
    float grade;
} Student;

void* write_thr(void *arg) {
    Student* value = (Student*)arg;
    printf("Name: %s, Number: %d, Grade:%f\n",value->name,value->number,value->grade);
    pthread_exit(NULL);
}

int main() {
    Student students[5];
    pthread_t threads[5];

    for (int i = 0; i < 5; i++)
    {
        printf("Enter student %d name: ", i + 1);
        scanf("%s", students[i].name);
        printf("Enter student %d number: ", i + 1);
        scanf("%d", &students[i].number);
        printf("Enter student %d grade: ", i + 1);
        scanf("%f", &students[i].grade);
    }


    for (size_t i = 0; i < 5; i++)
    {
        pthread_create(&threads[i],NULL,write_thr,(void*)&students[i]);
        pthread_join(threads[i],NULL);
    }

    return 0;
}
