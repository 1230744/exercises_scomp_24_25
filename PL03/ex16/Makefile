# A simple Makefile
prog: main.o
	gcc main.o -o prog

factorial: factorial.o
	gcc factorial.o -o factorial
	
main.o: main.c
	gcc -g -Wall -Wextra -fanalyzer -c main.c -o main.o
	
factorial.o: factorial.c
	gcc -g -Wall -Wextra -fanalyzer -c factorial.c -o factorial.o
	
clean:
	rm -f *.o
	
run: prog
	./prog
