#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void handle_USR1() {
    write(STDOUT_FILENO, "Divisor não pode ser 0.\n", 24);
    exit(1);
}

int main(){
	int n, d;
		
	printf("Indique o dividendo: ");
	scanf("%d", &n);
	
	printf("Indique o divisor: ");
	scanf("%d", &d);
	
	struct sigaction act;
 	/* Zeroes the sigaction structure */
 	memset(&act, 0, sizeof(struct sigaction));

 	act.sa_handler = handle_USR1;
 	act.sa_flags = SA_RESTART;
 	sigaction(SIGFPE, &act, NULL);
 	
 	int res = n/d;

	printf("O resultado é: %d\n", res);
	
	return 0;
}
