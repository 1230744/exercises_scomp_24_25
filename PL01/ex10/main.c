#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void populate(int arr[], int leng){
	for(int i=0; i<leng; i++){
		arr[i] = rand();
	}
}

int main(){
	int n = 100000;
	int arr[n];
	populate(arr, n);
	pid_t pids[5];
	
	int numberToSearch= 324;
	
	for(int i = 0; i<5; i++){
		pids[i]=fork();
		
		//Código executado pelo filho apenas
		if(pids[i] == 0){
			for(int j = i*20000; j<i*20000+20000; j++){
				if(numberToSearch == arr[j]){
					exit(i+1);
				}
			}
			exit(0);
		}
	}
	
	pid_t p;
	int status;
	//Apenas o pai chega aqui
	for(int i = 0; i<5; i++){
		p=waitpid(pids[i], &status, 0);
		if(WIFEXITED(status)){
			printf("Child %d with PID %d ended with value %d\n", i+1, p, WEXITSTATUS(status));
		}
	}
}
