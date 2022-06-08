all: console

console: console.o browse.o download.o
	gcc console.o browse.o download.o -o console -lncurses

console.o: console.c browse.h
	gcc -c console.c

browse.o: browse.c browse.h
	gcc -c browse.c

download.o: download.c
	gcc -c download.c

clean:
	-rm *.o
	-rm *~
	-rm -rf files/

