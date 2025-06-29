# A simple Makefile
prog: main.o
	gcc main.o -o prog

cheat: cheat.o
	gcc cheat.o -o cheat
	
main.o: main.c
	gcc -g -Wall -Wextra -fanalyzer -c main.c -o main.o
	
cheat.o: cheat.c
	gcc -g -Wall -Wextra -fanalyzer -c cheat.c -o cheat.o

clean:
	rm -f *.o
	
run: prog
	./prog
