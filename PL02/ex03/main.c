#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/wait.h>

void handle_USR1(int sig) {
    write(STDOUT_FILENO, "I got signal SIGSEGV.\n", 22);
    exit(1);
}

int main () {
	int a;
	
	struct sigaction act;
 	/* Zeroes the sigaction structure */
 	memset(&act, 0, sizeof(struct sigaction));

 	act.sa_handler = handle_USR1;
 	act.sa_flags = SA_RESTART;
 	sigaction(SIGSEGV, &act, NULL);
 	
	a = *( int *)0;
 		
	return 0;
}
