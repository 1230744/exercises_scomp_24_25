# A simple Makefile
CC = gcc
CFLAGS = -g -Wall -Wextra -fanalyzer
LIBS = -lrt -lpthread

all: server withdraw consult

server: server.o
	$(CC) server.o -o server $(LIBS)

withdraw: withdraw.o
	$(CC) withdraw.o -o withdraw $(LIBS)

consult: consult.o
	$(CC) consult.o -o consult $(LIBS)

server.o: server.c shm.h
	$(CC) $(CFLAGS) -c server.c

withdraw.o: withdraw.c shm.h
	$(CC) $(CFLAGS) -c withdraw.c

consult.o: consult.c shm.h
	$(CC) $(CFLAGS) -c consult.c

clean:
	rm -f *.o server withdraw consult

run-server: server
	./server

run-withdraw: withdraw
	./withdraw

run-consult: consult
	./consult
