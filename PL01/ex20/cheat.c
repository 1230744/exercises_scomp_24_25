#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

int main() {
    int number = (rand() % 5) + 1;
    exit(number);
}
