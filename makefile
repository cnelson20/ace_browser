all: download browse console
	
download: download.c
	gcc -o download download.c

browse: browse.c
	gcc -o browse browse.c

console: console.c
	gcc -o console console.c -lncurses

clean:
	rm *~
	rm *.o
