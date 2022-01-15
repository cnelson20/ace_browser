all: download console
	
download: download.o
	gcc -o download download.o

download.o: download.c
	gcc -c download.c

console: console.o browse.o
	gcc console.o browse.o -o console -lncurses

console.o: console.c browse.h
	gcc -c console.c

browse.o: browse.c browse.h
	gcc -c browse.c

clean:
	rm *.o
	rm *~

