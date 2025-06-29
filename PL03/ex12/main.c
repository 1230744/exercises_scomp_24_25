#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MACHINES 4
#define PIECES 1000

int main()
{
    pid_t pids[MACHINES];
    int pipes[MACHINES + 1][2];

    for (int i = 0; i < MACHINES + 1; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < MACHINES; i++)
    {
        pids[i] = fork();

        if (pids[i] == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0)
        {
            for (int j = 0; j < MACHINES + 1; j++)
            {
                if (j != i) close(pipes[j][0]);
                if (j != i + 1) close(pipes[j][1]);
            }

            int lot_size;
            if (i == 0) lot_size = 5;
            else if (i == 1) lot_size = 5;
            else if (i == 2) lot_size = 10;
            else lot_size = 100;

            if (i == 0) // M1 produz por conta própria
            {
                int produced = 0;
                while (produced < PIECES)
                {
                    int toSend = (PIECES - produced >= lot_size) ? lot_size : PIECES - produced;
                    write(pipes[i + 1][1], &toSend, sizeof(int));
                    printf("Machine %d produced and sent %d pieces.\n", i+1, toSend);
                    produced += toSend;
                }
                int endFlag = -1;
                write(pipes[i + 1][1], &endFlag, sizeof(int));
            }
            else
            {
                int buffer = 0;
                int received;
                while (1)
                {
                    read(pipes[i][0], &received, sizeof(int));
                    if (received == -1) break;

                    buffer += received;

                    while (buffer >= lot_size)
                    {
                        write(pipes[i + 1][1], &lot_size, sizeof(int));
                        printf("Machine %d processed and sent %d pieces.\n", i + 1, lot_size);
                        buffer -= lot_size;
                    }
                }
                // Finalizar próximo estágio
                if (buffer > 0)
                {
                    write(pipes[i + 1][1], &buffer, sizeof(int));
                    printf("Machine %d sent remaining %d pieces.\n", i + 1, buffer);
                }
                int endFlag = -1;
                write(pipes[i + 1][1], &endFlag, sizeof(int));
            }

            exit(EXIT_SUCCESS);
        }
    }

    // Processo principal: armazém
    // Fecha todos menos entrada da M1 e saída da M4
    for (int i = 1; i < MACHINES; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    close(pipes[0][0]); // não lê da M1
    close(pipes[MACHINES][1]); // não escreve no fim
    int total = 0;
    int received;

    while (1)
    {
        read(pipes[MACHINES][0], &received, sizeof(int));
        if (received == -1) break;

        printf("Storage received %d pieces. Adding to inventory...\n", received);
        total += received;
        printf("Total stock: %d\n", total);
    }

    for (int i = 0; i < MACHINES; i++)
    {
        wait(NULL);
    }

    close(pipes[0][1]);
    close(pipes[MACHINES][0]);
    return 0;
}
