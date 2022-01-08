#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
	int ch;
	
	initscr();
	raw();
	keypad(stdscr,TRUE);
	noecho();
	
	idlok(stdscr,TRUE);
	scrollok(stdscr,TRUE);
	
	int maxx, maxy;
	getmaxyx(stdscr, maxy, maxx);
	
	printw("Hello World!\nHello World 2!\n");
	while(1) {
		int x,y;
		getyx(stdscr, y ,x);
		
		ch = getch();
		if (ch >= 0x20 && ch < 0x7F) {
			printw("%c",ch);
		} else {
			switch (ch) {
				case KEY_UP:
					move(y-1,x);
					break;
				case KEY_DOWN:
					move(y+1,x);
					break;
				case KEY_LEFT:
					move(y,x-1);
					break;			
				case KEY_RIGHT:
					move(y,x+1);
					break;
				case 127:
					printw("\b \b");
					break;
			}
		}
		
		refresh();
		if (ch == 330) {
			break;
		}
				
	}
	getch();
	endwin();
	
	return 0;
}