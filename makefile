all: download browse console
	
download: download.o
	gcc -o download download.o

download.o: download.c
	gcc -c download.c

browse: browse.o
	gcc -o browse browse.o

browse.o: browse.c browse.h
	gcc -c browse.c

console: console.o
	gcc -o console console.o -lncurses

console.o: console.c browse.h
	gcc -c console.c

clean:
	rm *.o
	rm *~

