#include <unistd.h>
#include <termios.h>
#include <stdio.h>

struct termios oldt, newt = 0;

int stdin_disable_buffer_echo() {
	if (newt == 0) {
		tcgetattr(0, &oldt);
		newt = oldt;
		newt.c_lflag &= ~ICANON;
		newt.c_lflag &= ~ECHO;
		newt.c_cc[VMIN] = 0;
		newt.c_cc[VTIME] = 0;
		tcsetattr(0, TCSANOW, &newt);
	}
}
int_stdin_reset_modes() {
	if (newt != 0) {
		tcsetattr(0, TCSANOW, &oldt);
		newt = 0;
	}	
}

int main() {
	int i;
	for (i = 0; i < 10; i++) {	
		int ch = 0;
		read(STDIN_FILENO,&ch,1);
		if (ch) {
		printf("%x '%c'\n",ch);
		} else {i--;}
	}
	
}