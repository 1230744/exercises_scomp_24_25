# A simple Makefile
prog: main.o
	gcc main.o -o prog
	
main.o: main.c
	gcc -g -Wall -Wextra -fanalyzer -c main.c -o main.o
	
clean:
	rm -f *.o
	
run: prog
	./prog
