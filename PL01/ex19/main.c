#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

int main() {
    char filename[256];

    // Ler nome do ficheiro
    printf("Nome do ficheiro a copiar: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        perror("Erro ao ler nome");
        exit(EXIT_FAILURE);
    }

    // Remover newline
    filename[strcspn(filename, "\n")] = '\0';

    mkdir("backup", 0755);

    execlp("cp", "cp", filename, "backup/", NULL);

    perror("Erro ao executar cp");
    return EXIT_FAILURE;
}
