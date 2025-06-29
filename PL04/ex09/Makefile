# A simple Makefile
interface: interface.o
	gcc interface.o -o interface
	
interface.o: interface.c
	gcc -g -Wall -Wextra -fanalyzer -c interface.c -o interface.o -lrt
	
encrypt: encrypt.o
	gcc encrypt.o -o encrypt
	
encrypt.o: encrypt.c
	gcc -g -Wall -Wextra -fanalyzer -c encrypt.c -o encrypt.o -lrt

display: display.o
	gcc display.o -o display
	
display.o: display.c
	gcc -g -Wall -Wextra -fanalyzer -c display.c -o display.o -lrt
	
clean:
	rm -f *.o
	
run: interface encrypt display
	./interface
	./encrypt
	./display
