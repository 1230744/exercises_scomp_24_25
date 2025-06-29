#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pthread.h>

void* write_thr(void *arg) {
    char* value = (char*)arg;
    printf("%s",value);
    pthread_exit(NULL);
}

int main() {
    char *first = "Ricardo ";
    char *last = "Meireles ";

    pthread_t threads[2];
    pthread_create(&threads[0],NULL,write_thr,(void*)first);
    pthread_create(&threads[1],NULL,write_thr,(void*)last);

    pthread_join(threads[0],NULL);
    pthread_join(threads[1],NULL);
    printf("\n");
}
