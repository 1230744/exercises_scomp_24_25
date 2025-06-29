#include <stdio.h>

int main() {
    int number;
    if (scanf("%d", &number) != 1) {
        printf("-1\n");
        return 1;
    }

    if (number < 0) {
        printf("-1\n");
        return 1;
    }

    unsigned long long result = 1;
    for (int i = 2; i <= number; i++) {
        result *= i;
    }

    printf("%llu\n", result);
    return 0;
}
