#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "browse.h"

#define MAX(a,b) ((a) > (b) ? (a) : (b))

extern char html_element_index_names[][16];

int strlen_special(char *s) {
	int i = 0;
	while (*s) {
		if (*s == DC1) {
			s++;
			s += sizeof(char);
		} else if (*s == DC2) {
			s++;
		} else {
			i++;
			s++;
		}
	}
	return i;
}

int colors_array[] = {COLOR_WHITE, COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN};
void init_colors() {
  int i,j;
  for (j = 0; j < 8; j++) {
	for (i = 0; i < 8; i++) {
	  if (i != j) {
		  init_pair(j*8+i, colors_array[i], colors_array[j]);
	  }
	}
  }
  //init_pair(1,COLOR_WHITE, COLOR_BLACK);
}

void set_attributes(unsigned char i) {
    int val = 0;
	if (i & 64) {val |= A_UNDERLINE;}
	if (i & 128) {val |= A_BOLD;}
	val |= COLOR_PAIR(i%64);
	attrset(val);
	
	//printw("%hhu",i);
}

void print_html_element_xy(struct html_element *html) {
	printf("------------------------\n");
	printf("Element: Tag: (%d) %s\n",html->tag,html_element_index_names[html->tag]);
	printf("innerHTML: '%s'\n",html->innertext);
	printf("lx: %d  ly: %d\n",html->lx,html->ly);
	printf("rx: %d  ry: %d\n",html->rx,html->ry);
	if (html->num_children != 0) {	
		int i;
		printf("---- Children (%d): ----\n",html->num_children);
		for (i = 0; i < html->num_children; i++) {
			print_html_element_xy(html->children[i]);
		}
		printf("------------------------\n");
	}
}

struct html_element *search_html_xy(struct html_element *html, int x, int y) {
	if (html->ly > y || html->ry < y) {
		return NULL;
	} 
	if (html->num_children == 0) {
		if (html->lx <= x && x < html->rx) {
			return html;
		} else {
			return NULL;
		}
	} else {
		int i;
		struct html_element *temp;
		for (i = 0; i < html->num_children; i++) {
			temp = search_html_xy(html->children[i],x,y);
			if (temp != NULL) {
				return temp;
			}
		}
		return NULL;	
	}
}


int set_dims_x = 0;
int set_dims_y = 0;
char *html_set_dims(struct html_element *elem, char *text) {
	int child_no = 0;
	
	if (DONTPRINT(elem->tag)) {return text - 1;}
	
	elem->lx = set_dims_x;
	elem->ly = set_dims_y;
	
	//printf("\nelem: %p\n",elem);
	//printf("lx: %d  ly: %d\n",set_dims_x, set_dims_y);
	//print_html_structure(elem, 0);
	
	while(*text) {
		//printf("0x%hhx 0x%hhx 0x%hhx 0x%hhx\n",*text,*(text+1),*(text+2),*(text+3));
		
		switch (*text) {
			case DC1:
				//printf("%s: calling child\n", html_element_index_names[elem->tag]);
				text = html_set_dims(elem->children[child_no++],text+2);
				text++;
				//if (child_no >= elem->num_children) { return text; }
				break;
			case DC2:
				elem->rx = set_dims_x;
				elem->ry = set_dims_y;
				//print_element_path(elem);
				//printf("lx: %d  ly: %d\nrx: %d  ry: %d\n", elem->lx, elem->ly, set_dims_x, set_dims_y);
				return text;
			case '\n':
				set_dims_y++;
				set_dims_x = 0;
				text++;
				break;
			default:
				set_dims_x++;
				text++;
				break;
		}
	}
	
	return text;
}

int main(int argc, char *argv[]) {
	int ch;
	
	struct stat std;
	struct unschar_stack *attr_stack;
	
	char *temp; 
	int max_x, max_y;
	int x,y;
	int scroll_x, scroll_y;
	int max_scroll_y, max_scroll_x;
	int i, fd , is_not_firstloop;
	char **lines;
	
	/* 
		Render the html page
		gets saved to output
		html is the parent for the whole page
	*/
	render_html_file(argv[1],"output.dat");
	/* This would read from a file 
	stat(argv[1],&std);
	fd = open(argv[1], O_RDONLY);
	output = malloc(std.st_size+1);
	read(fd,output,std.st_size);
	close(fd);
	*/
	
	temp = output;
	set_dims_x = 0;
	set_dims_y = 0;
	html_set_dims(html,temp);
	
	initscr();
	raw();
	keypad(stdscr,TRUE);
	noecho();
	
	idlok(stdscr,TRUE);
	scrollok(stdscr,TRUE);
	start_color();
	init_colors();
	
	struct html_element *elem = html;
	int ci = 0;
	
	temp = output;
	for (i = 0; *temp; temp++) {
		if (*temp == '\n') {i++;}
	}
	if (*(strchr(output,'\0') - 1)) {
		i++;
	}
	lines = malloc((i+1)*sizeof(char *));
	temp = output;
	for (i = 0; temp && *temp; i++) {
		char *curr = strsep(&temp,"\n");
		lines[i] = curr;
	}
	lines[i] = NULL;
	max_scroll_y = i;
	
	
	max_scroll_x = 0;
	/* Find maximum strlen of a line to know where to limit scrolling */
	for (i = 0; lines[i]; i++) {
		max_scroll_x = MAX(max_scroll_x,strlen_special(lines[i]));		
	}
	getmaxyx(stdscr, max_y, max_x);
	if (max_scroll_x > max_x) {
		max_scroll_x -= max_x;
	} else { 
		max_scroll_x = 0;
	}
	
	if (max_scroll_y > max_y) {
		max_scroll_y -= max_y;
	} else {
		max_scroll_y = 0;
	}
	scroll_x = 0;
	scroll_y = 0;

	attrset(COLOR_PAIR(1));
	is_not_firstloop = 0;
	getyx(stdscr, y ,x);
	while(1) {
		int old_scroll_x = scroll_x;
		int old_scroll_y = scroll_y;
		
		if (is_not_firstloop) {
			ch = getch();
		} else {
			ch = 0;
		}
		if (ch >= 0x20 && ch < 0x7F) {
			break;
		} else {
			struct html_element *selected;
			switch (ch) {
				case '\n':
					move(scroll_y + y, scroll_x + x);
					//printw("Enter");
					selected = search_html_xy(html,scroll_x + x, scroll_y + y);
					if (selected != NULL) {
						switch (selected->tag) {
						case ELEMENT_A:
							endwin();
							printf("%s\n",html_element_index_names[selected->tag]);
							printf("innertext: '%s'\n",selected->innertext);
							exit(0);
						default:
							break;
						}
					}
					break;
				case KEY_UP:
					if (y == 0) {
						if (scroll_y > 0) { scroll_y--; }
					} else {
						y--;
					}
					break;
				case KEY_DOWN:
					if (y == max_y - 1) {
						if (scroll_y < max_scroll_y) { scroll_y++; }
					} else {
						y++;
					}
					break;
				case KEY_LEFT:
					if (x == 0) {
						if (scroll_x > 0) { scroll_x--; }
					} else {
						x--;
					}
					break;			
				case KEY_RIGHT:
					if (x == max_x - 1) {
						if (scroll_x < max_scroll_x) { scroll_x++; }
					} else {
						x++;
					}
					break;
				default:
					break;
			}
		}
		if (scroll_x == old_scroll_x && is_not_firstloop) {
			if (scroll_y != old_scroll_y) {
				if (scroll_y > old_scroll_y) {
					int i, cur_row; 
					
					while (scroll_y > old_scroll_y) {
						scrl(1);
						old_scroll_y++;
					}
					cur_row = max_y - 1;
					move(cur_row,0);
					cur_row += scroll_y;
					for (i = scroll_x; i < scroll_x + max_x && i < strlen(lines[cur_row]) && lines[cur_row - 1][i];) {
						if (lines[cur_row][i] == DC1) {
							i++;
							set_attributes((unsigned char)lines[cur_row][i++]);
							continue;
						} else if (lines[cur_row][i] == DC2) {
							i++;
							continue;
						}
						addch(lines[cur_row][i++]);
					}
				} else {
					int i;
					while (scroll_y < old_scroll_y) {
						scrl(-1);
						old_scroll_y--;
					}
					move(0,0);
					for (i = scroll_x; i < scroll_x + max_x && i < strlen(lines[scroll_y]) && lines[scroll_y][i];) {
						if (lines[scroll_y][i] == DC1) {
							i++;
							set_attributes((unsigned char)lines[scroll_y][i++]);
							i++;
							continue;
						} else if (lines[scroll_y][i] == DC2) {
							i++;
							continue;
						}
						addch(lines[scroll_y][i++]);
					}
				}
			}
		} else {
			wbkgd(stdscr,COLOR_PAIR(1));
			clear();
			is_not_firstloop = 1;
			move(0,0);
			int i,j;
			for (j = scroll_y; j < scroll_y + max_y && lines[j]; j++) {
				for (i = scroll_x; i < scroll_x + max_x && i < strlen(lines[j]) && lines[j][i];) {
					if (lines[j][i] == DC1) {
						i++;
						set_attributes((unsigned char)lines[j][i++]);
						continue;
					} else if (lines[j][i] == DC2) {
						i++;
						continue;
					}
					addch(lines[j][i++]);
				}
				addch('\n');
			}
		}
		move(y,x);
		refresh();		
	}
	getch();
	endwin();
	
	print_html_element_xy(html);
	printf("ch: %d\n",ch);
	
	free_html_element(html);
	free(lines);
	free(output);	
	
	return 0;
}
