all: download browse

browse: browse.o
	gcc -o browse browse.o
	
browse.o: browse.c browse.h
	gcc -c browse.c
	
download: download.o
	gcc -o download download.o

download.o: download.c
	gcc -c download.c