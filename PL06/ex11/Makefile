# A simple Makefile
prog1: prog1.o
	gcc prog1.o -o prog1 -lm -lpthread

prog1.o: prog1.c
	gcc -g -Wall -Wextra -fanalyzer -c prog1.c -o prog1.o

clean:
	rm -f *.o

run: prog1
	./prog1
