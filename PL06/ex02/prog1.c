#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

bool is_prime(int n) {
    if (n < 2) return false;
    for (int i = 2; i*i <= n; ++i) {
        if (n % i == 0) return false;
    }
    return true;
}

void* print_primes(void* arg) {
    int max = *(int*)arg;
    printf("Prime numbers up to %d:\n", max);
    for (int i = 2; i <= max; ++i) {
        if (is_prime(i)) {
            printf("%d ", i);
        }
    }
    printf("\n");
    pthread_exit(NULL);
}

int main() {
    int max;
    printf("Enter the highest positive value: ");
    scanf("%d", &max);

    pthread_t tid;
    pthread_create(&tid, NULL, print_primes, &max);
    pthread_join(tid, NULL);

    return 0;
}
