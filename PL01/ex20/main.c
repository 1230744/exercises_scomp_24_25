#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int number, generatedNum;
    double money = 25;

    while (money > 0) {
        char input[100];

        printf("Aposta: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Erro ao ler aposta");
            exit(EXIT_FAILURE);
        }
        int aposta = atoi(input);

        printf("Escolha um número entre 1 e 5: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Erro ao ler número");
            exit(EXIT_FAILURE);
        }
        number = atoi(input);

        if (number < 1 || number > 5) {
            printf("Número inválido. Escolha entre 1 e 5.\n");
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("Erro no fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            // Processo filho: executa o programa externo "cheat"
            execlp("./cheat", "./cheat", NULL);
            perror("Erro ao executar cheat");
            exit(EXIT_FAILURE);
        }

        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            generatedNum = WEXITSTATUS(status);
            printf("Número sorteado: %d\n", generatedNum);

            if (number == generatedNum) {
                money += aposta;
                printf("Acertaste o número!\n");
            } else {
                money -= aposta;
                printf("Falhaste!\n");
            }

            printf("Saldo atual: %.2f\n\n", money);
        } else {
            printf("O processo filho terminou de forma anormal.\n");
        }
    }

    printf("Jogo terminado. Ficou sem saldo.\n");
    return EXIT_SUCCESS;
}
