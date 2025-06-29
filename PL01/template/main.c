#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(){
	pid_t pid ;
	int i ;
	for ( i =0; i <3; i ++)
		pid = fork ();

	printf ("Concurrent Programming\n");
	return 0;
}
