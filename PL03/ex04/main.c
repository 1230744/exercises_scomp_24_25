#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#define MESSAGE_SIZE 50

void trocarMaiusculasMinusculas(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (isupper(str[i])) {
            str[i] = tolower(str[i]);
        } else if (islower(str[i])) {
            str[i] = toupper(str[i]);
        }
    }
}

int main() {
	int pipeup[2], pipedown[2];
	char buffer[MESSAGE_SIZE];

	char message[MESSAGE_SIZE];
	printf("Enter the message: ");
	scanf("%s", message);

	if ((pipe(pipeup) == -1)||(pipe(pipedown) == -1)) {
		perror("pipe");
		exit(1);
	}

	pid_t pid = fork();
	
	if (pid == -1) {
		perror("fork");
		exit(1);
	}

	int len = snprintf(buffer, MESSAGE_SIZE, "%s", message);
	if (pid > 0) {
		close(pipeup[1]);
		read(pipeup[0], buffer, sizeof(buffer));
		trocarMaiusculasMinusculas(buffer);
		close(pipeup[0]);
		
		close(pipedown[0]);
		write(pipedown[1], buffer, len);
		close(pipedown[1]);
	} else if (pid == 0) {
		close(pipeup[0]);
		write(pipeup[1], buffer, len);
		close(pipeup[1]);
		
		close(pipedown[1]);
		read(pipedown[0], buffer, sizeof(buffer));
		close(pipedown[0]);
				
		printf("\n%s\n", buffer);
	}

	return 0;
}
