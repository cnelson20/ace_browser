#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

#include "browse.h"
#include "download.h"

#define MAX(a,b) ((a) > (b) ? (a) : (b))

extern char html_element_index_names[][16];
extern size_t html_element_index_names_len;

char text_text[] = "text";
char *site, *path;

int meta_sleep_pid = 0;
char *meta_site, *meta_path;

/* 
	strlen, ignoring DC1 ( and the succeeding character) and DC2 from parts of code
*/
int strlen_special(char *s) {
	int i = 0;
	while (*s) {
		if (*s == DC1 || *s == DC2) {
			s += 2;
		} else {
			i++;
			s++;
		}
	}
	return i;
}

int colors_array[] = {COLOR_WHITE, COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN};
/*
	Sets up ncurses colors
*/
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

/*
	Given a char[] link (a url),
	malloc() space for site and path and set them correctly
	Ex: get_site_path_from_url("google.com/search") => "google.com" , "/search"
*/
void get_site_path_from_url(char *link, char **site, char **path) {
    char *cur_link = link;
	if (strstr(link,"://")) {
	  cur_link = strstr(link,"://") + strlen("://");
	}
	if (strchr(cur_link,'/')) {
	  *path = malloc(strlen(strchr(cur_link,'/')) + 1);
	  strcpy(*path,strchr(cur_link,'/'));
	  *strchr(cur_link,'/') = '\0';
	  *site = strdup(link);
	  *strchr(cur_link,'\0') = '/';
	} else {
	  *path = strdup("/");
	  *site = strdup(link);
	}
   
}

/*
	Sets ncurses attributes given i
*/
void set_attributes(unsigned char i) {
    int val = 0;
	if (i & 64) {val |= A_UNDERLINE;}
	if (i & 128) {val |= A_BOLD;}
	val |= COLOR_PAIR(i%64);
	attrset(val);
	
	//printw("%hhu",i);
}

/*
	Searches a html tree for an element where x and y is within its bounds
	TODO: Make this binary search
*/
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

struct html_element *list_form_inputs[100];
int list_form_inputs_len;
void clear_list_form_inputs() {
	list_form_inputs_len = 0;
}
void gen_list_form_inputs(struct html_element *html, int in_form) {
	
	if (in_form && IMPORTANT(html->tag)) {
		list_form_inputs[list_form_inputs_len++] = html;
		return;
	} else if (html->tag == ELEMENT_TITLE) {
	  printf("%c]0;%s : %s%c", '\033', is_html ? "html_doc" : "txt", html->innertext, '\007');
	} else if (html->tag == ELEMENT_META) {
	  	int i;
		char *http_equiv_value = NULL;
		char *content_value = NULL;
		for (i = 0; i < html->properties_length; i++) {
			if (!strcmp(html->properties[i]->key, "http-equiv")) {
				http_equiv_value = html->properties[i]->value;
			} else if (!strcmp(html->properties[i]->key, "content")) {
				content_value = html->properties[i]->value;
			}
		}
		printf("http_equiv_value: '%s'  content_value: '%s'\n", http_equiv_value, content_value);
		if (http_equiv_value != NULL && !stricmp(http_equiv_value, "refresh") && content_value != NULL) {
			/* Seg fault here: */
			char *path_temp = strchr(content_value, ';');
			if (path_temp != NULL) {*path_temp = '\0';}
			meta_sleep_pid = fork();
			printf("Forked! content_value: \"%s\"\n", content_value);
			if (!meta_sleep_pid) {
				printf("b4");
				sleep(atoi(content_value));
				printf("after");
				exit(0);
			}
			
			if (path_temp == NULL) {
				meta_site = strdup(site);
				meta_path = strdup(path);
			} else {
				*path_temp = ';';
				do {
					path_temp++;
				} while (*path_temp == ' ');
				printf("path_temp with URL=: '%s'\n", path_temp);
				static_tolowern(path_temp, 4);
				printf("static_tolower_string: '%s'\n", static_tolower_string);
				char *path_temp_quote = (strstr(static_tolower_string,"url=") - static_tolower_string) + strlen("url=") + path_temp; 
				char path_temp_quote_type = *path_temp_quote;
				if (path_temp_quote_type == '"' || path_temp_quote_type == '\'') {
					path_temp_quote++;
				} else {
					path_temp_quote_type = '\0';
				}
				int path_temp_i = 0;
				while (*path_temp_quote && *path_temp_quote != path_temp_quote_type) {
					path_temp[path_temp_i++] = *(path_temp_quote++);
				}
				path_temp[path_temp_i] = '\0';
				
				printf("path_temp: '%s'\n",path_temp);
				if (strstr(path_temp, "://")) {
					//strcpy(path_temp, path_temp + strlen("://"));
					char *t = strchr(strstr(path_temp,"://") + strlen("://"), '/');
					*t = '\0';
					meta_site = malloc(strlen(path_temp) + 1);
					strcpy(meta_site, path_temp);
					*t = '/';
					meta_path = malloc(strlen(t) + 1);
					strcpy(meta_path, t);
					
				} else if (*path_temp == '/') {
					// Absolute pathing
					meta_site = site;
					meta_path = malloc(strlen(path_temp) + 1);
					strcpy(meta_path, path_temp);
				} else {
					// Relative pathing
					meta_site = site;
					char *nul_temp = strrchr(path,'/') + 1;
					char ntemp = *nul_temp;
					*nul_temp = '\0';
					
					meta_path = malloc(strlen(path) + strlen(path_temp) + 1);
					strcpy(meta_path, path);
					strcat(meta_path, path_temp);

					*nul_temp = ntemp;
				}
			}
			printf("meta_site: '%s' meta_path: '%s'\n", meta_site, meta_path);
	  	}
	}
	in_form |= (html->tag == ELEMENT_FORM);
	int i;
	for (i = 0; i < html->num_children; i++) {
		gen_list_form_inputs(html->children[i], in_form);
	}

}

int set_dims_x = 0;
int set_dims_y = 0;
/* 
	Sets the lx,ly,rx,ry fields of a html_element tree given text
	
	Note: always set_dims_x and set_dims_y to 0 before calling this function.
*/
char *html_set_dims(struct html_element *elem, char *text) {
    int child_no = 0;

    elem->lx = set_dims_x;
    elem->ly = set_dims_y;
    
	printf("html_set_dims: ");
	print_element_path(elem);
    
    while(*text) {    
        switch (*text) {
			case DC1:
				while (elem->children[child_no] != NULL && DONTPRINT(elem->children[child_no]->tag)) {
					child_no++;
				}
				if (elem->children[child_no] != NULL) {
					text = html_set_dims(elem->children[child_no++],text+2);
				} else {
					//exit(0);
					text++;
				}
				text++;
				break;
			case DC2:
				elem->rx = set_dims_x;
				elem->ry = set_dims_y;
				return text + 1;
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

	elem->rx = set_dims_x;
	elem->ry = set_dims_y;
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
	char *dwld_file;

	printf("argv: Array(%d)\n", argc);
	for (i = 0; i < argc; i++) {
	  printf("argv[%d] = \"%s\"\n", i, argv[i]);
	}
	
	if (argc < 2) {
		printf("Usage:\n./console URL\n./console -f FILENAME\n./console -s SITE PATH\n");
		exit(1);
	}
	if (!strcmp(argv[1],"-f")) {
	    site = strdup("_file");
	    path = strdup(argv[2]);
	} else if (!strcmp(argv[1],"-s")) {
 	    site = strdup(argv[2]);
	    path = strdup(argv[3]);	    
	} else if (!strcmp(argv[1],"--site-with-file")) {
		site = strdup(argv[2]);
	    path = strdup(argv[3]);
		dwld_file = strdup(argv[4]);
	} else {
	    /* Download file */
	    get_site_path_from_url(argv[1],&site,&path);
	}
	printf("site: '%s'  path: '%s'\n",site,path);
	
	if (strcmp(argv[1],"--site-with-file")) {
		if (stricmp(site,"_file") == 0) {
		    dwld_file = strdup(path);
		} else {
	    	dwld_file = curl(site,&path,1,NULL);
		}
	}
	printf("dwld_file: '%s'\n", dwld_file);
	  
	printf("site: '%s', path: '%s'\n",site,path);
	/* 
		Render the html page
		gets saved to output
		html is the parent for the whole page
	*/

	if (stat(dwld_file,&std) == -1) {
	  printf("Error locating file, reason: '%s'\nthe requested URL might not exist, check that\n",strerror(errno));
	  exit(1);
	}

	//printf("calling render_html_file()\n");
	render_html_file(dwld_file, "output.dat");
	//printf("render_html_file() finished\n");
	
	if (is_html) {
		set_dims_x = 0;
		set_dims_y = 0;
		html_set_dims(html,strchr(output,DC1)+2);
		clear_list_form_inputs();
		gen_list_form_inputs(html, 0);
	}

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
	timeout(0);
	is_not_firstloop = 0;
	getyx(stdscr, y ,x);
	while(1) {
		int old_scroll_x = scroll_x;
		int old_scroll_y = scroll_y;
		int child_status_sleep;
		if (waitpid(meta_sleep_pid, &child_status_sleep, WNOHANG) > 0) {
		    endwin();
		  
		    /* Sleep Finished */
		    char **self_args = malloc(6 * sizeof(char *));
		    self_args[0] = "./console";
		    //self_args[1] = strdup("--site-with-file");
			self_args[1] = "-s";
		    self_args[2] = meta_site;
		    self_args[3] = meta_path;
		    //self_args[4] = dwld_file;
			self_args[4] = NULL;
		    execvp(self_args[0], self_args);
			printf("Error: %s\n", strerror(errno));
			exit(0);
		}
		
		if (is_not_firstloop) {
			ch = getch();
		} else {
			ch = 0;
		}
		if (ch >= 0x20 && ch < 0x7F) {
			if (ch == 'S' || ch == 's' || ch == 'N' || ch == 'n') {
				char typeflag = tolower(ch);
				char querypath[256] = "";
				int query_length = 0;
				while ((ch = getch()) != '\n') {
					if (ch >= 0x20 && ch <= 0x79 && query_length < 256) {
						querypath[query_length] = ch;
						query_length++;
						querypath[query_length] = '\0';
					} else if (ch == 263 || ch == 127 && query_length > 0) {
						query_length--;
						querypath[query_length] = '\0';
					}
					move((max_y - 1), 0);
					attrset(COLOR_PAIR(8));
					//printw("%d ", max_y);
					printw("%s: %-40s", typeflag == 's' ? "URL" : "Path",querypath);
					refresh();
				}
				endwin();
				if (strstr(querypath, "://") || typeflag == 's') {
					free(site);
					free(path);
					get_site_path_from_url(querypath, &site, &path);
				} else if (querypath[0] == '/') {
					path = strdup(querypath);
				} else if (querypath[0] != '\0') {
					*(strrchr(path, '/') + 1) = '\0';
					path = realloc(path, strlen(path) + strlen(querypath));
					strcat(path, querypath);
				}
				printw("Site: %s Path: %s", site, path);
				char **toexec = malloc(5 * sizeof(char *));
				toexec[0] = "./console";
				toexec[1] = "-s";
				toexec[2] = site;
				toexec[3] = path;
				toexec[4] = NULL;
				execvp(toexec[0], toexec);
				printf("Error: %s\n", strerror(errno));
				exit(0);
			} else if (1) {
				break;
			}
		} else {
			struct html_element *selected;
			switch (ch) {
				case '\n':
					if (!is_html) {
						break;
					}
					selected = search_html_xy(html,scroll_x + x, scroll_y + y /* + 1*/);
					if (selected == NULL) {
					  selected = search_html_xy(html, scroll_x + x, scroll_y + y + 1);
					}
					if (selected != NULL) {
						//printw("hit!: selected = %s       ",html_element_index_names[selected->tag]);
						char *type_value = NULL;
						char *value_value = NULL;
						int i_2;
						switch (selected->tag) {
							case ELEMENT_A:
								endwin();
								char **self_exec = malloc(5 * sizeof(char));
								char *href_value = NULL;
								int i;
								for (i = 0; i < selected->properties_length; i++) {
									if (!stricmp(selected->properties[i]->key, "href")) {
										href_value = selected->properties[i]->value;
										break;
									}
								}
								if (!href_value) {break;}
								printf("href found!: '%s'\n",href_value);
								if (strstr(href_value, "://")) {
									// New site
									char *past_href_value = strstr(href_value,"://") + strlen("://");
									printf("copied href: '%s'\n",href_value);
									free(site);
									free(path);
									char *temp = strchr(past_href_value, '/');
									if (temp == NULL) {
										site = malloc(strlen(href_value) + 1);
										strcpy(site, href_value);
										path = malloc(2);
										strcpy(path,"/");
									} else {
										path = malloc(strlen(temp) + 1);
										strcpy(path,temp);
										*temp = '\0';
										site = malloc(strlen(href_value) + 1);
										strcpy(site, href_value);
										*temp = '/';
									}
								} else if (*href_value == '/') {
									// Absolute pathing
									free(path);
									path = malloc(strlen(href_value) + 1);
									strcpy(path, href_value);
									if (!strcmp(site, "_file") && *(path + strlen(path) - 1) != '/') {
										path = realloc(path, strlen(path) + strlen("index.html") + 1);
										strcat(path, "index.html");
									}
								} else {
									// Relative pathing
									if (*(path + strlen(path) - 1) != '/') {
										*(strrchr(path, '/') + 1) = '\0';
									}
									path = realloc(path, strlen(path) + strlen(href_value) + 1);
									strcat(path, href_value);
									if (!strcmp(site, "_file") && *(path + strlen(path) - 1) != '/') {
										path = realloc(path, strlen(path) + strlen("index.html") + 1);
										strcat(path, "index.html");
									}
								}
								printf("new site: '%s' new path: '%s'\n",site,path);
								self_exec[0] = "./console";
								self_exec[1] = "-s";
								self_exec[2] = site;
								self_exec[3] = path;
								self_exec[4] = NULL;
								execvp(self_exec[0], self_exec);
								printf("Error: %s\n", strerror(errno));
								exit(0);
								break;
							case ELEMENT_TEXTAREA:
							case ELEMENT_INPUT:
							   /*
								 char *type_value = NULL;
								 char *value_value = NULL;
								 int i_2;
							   */
							   if (selected->tag == ELEMENT_INPUT) {
								   for (i_2 = 0; i_2 < selected->properties_length; i_2++) {
									   if (!stricmp(selected->properties[i_2]->key, "type")) {
										 type_value = selected->properties[i_2]->value;
										 break;
									   }
								   }
							   } else {
								 type_value = text_text;
							   }
							   if (!stricmp(type_value, "submit")) {
									endwin();
									printf("Submit!\n");
									struct html_element *element_form = selected;
									while (element_form != NULL && element_form->tag != ELEMENT_FORM) {
										element_form = element_form->parent;
									}
									if (element_form != NULL) {
										struct form_args_holder to_post;
										to_post.args = malloc(sizeof(char *));
										to_post.args[0] = NULL;
										to_post.length = 0;
										post_check(element_form, element_form, &to_post);
										printf("after post_check\n");
										char *action_value = NULL;
										int method = 1;
										int i;
										for (i = 0; i < element_form->properties_length; i++) {
											if (method && !strcmp(element_form->properties[i]->key, "method")) {
												method = (strcmp(element_form->properties[i]->value, "POST") != 0);
											} else if (action_value == NULL && !strcmp(element_form->properties[i]->key, "action")) {
												action_value = element_form->properties[i]->value;
											}
										}
										printf("action_value: '%s'\n", action_value);
										printf("old site: '%s' old path: '%s'\n", site, path);
										if (*action_value == '/') {
											// Absolute pathing
											free(path);
											path = malloc(strlen(action_value) + 1);
											strcpy(path, action_value);
										} else {
											// Relative pathing
											*(strrchr(path,'/') + 1) = '\0';
											path = realloc(path, strlen(path) + strlen(action_value) + 1);
										        //printf("path: '%s'\taction_value: '%s'\n", path, action_value);
											strcat(path, action_value);
										}
										printf("\nMethod: '%s'\n",method ? "GET" : "POST");
										for (i = 0; i <= to_post.length; i++) {
											printf("[%d]: '%s'\n", i, to_post.args[i]);
										}
										printf("\n");
										printf("action site: '%s' action path: '%s'\n", site, path);
										printf("\n");
										char *dwld_file = curl(site, &path, method, to_post.args);
										char **self_exec = malloc(5 * sizeof(char *));
										self_exec[0] = "./console";
										self_exec[1] = "-s";
										self_exec[2] = site;
										self_exec[3] = path;
										//self_exec[4] = dwld_file;
										self_exec[4] = NULL;
									
										execvp(self_exec[0], self_exec);
										printf("Error: %s\n", strerror(errno));
										exit(0);
									}
								} else if (!stricmp(type_value, "text") || !stricmp(type_value, "password")) {
									for (i_2 = 0; i_2 < selected->properties_length; i_2++) {
										//move(i_2,80);
										//printw("%s",selected->properties[i_2]->key);
										if (!stricmp(selected->properties[i_2]->key, "value")) {
											value_value = selected->properties[i_2]->value;
											break;
										}
									}
									if (value_value == NULL) {
										i_2 = selected->properties_length;
										selected->properties = realloc(selected->properties, sizeof(struct key_value_pair *) * (i_2 + 1));
										selected->properties[i_2] = malloc(sizeof(struct key_value_pair));
										selected->properties[i_2]->key = malloc(strlen("value") + 1); 
										strcpy(selected->properties[i_2]->key, "value");

										selected->properties[i_2]->value = malloc(strlen("") + 1); 
										strcpy(selected->properties[i_2]->value, "");
										value_value = selected->properties[i_2]->value;

										selected->properties_length++;
									}
									value_value = realloc(value_value,512);
									selected->properties[i_2]->value = value_value;
									int ch = getch();
									while (ch != -1 && ch != '\n') {
										ch = getch();
									}
									while (ch != '\n') {
										move(selected->ly - scroll_y, selected->lx - scroll_x);
										int i;
										set_attributes(gen_console_attributes_char(selected));
										for (i = 0; i < selected->innertext_length; i++) {
											if (i < strlen(value_value)) {
												addch(value_value[max(0, strlen(value_value) - selected->innertext_length + 1) + i]);
											} else {
												addch(' ');
											}
										}
										move(selected->ly - scroll_y, selected->lx - scroll_x + min(selected->innertext_length - 1,strlen(value_value)));
										if (0x20 <= ch && ch <= 0x79) {
											i = strlen(value_value);
											if (i + 1 < 512) {
												value_value[i] = (char)ch;
												value_value[i+1] = '\0';
											}
										} else if (ch == 263 || ch == 127) {
											if (*value_value != '\0' /* check if strlen != 0 */) {
												value_value[strlen(value_value) - 1] = '\0';
											}
										}
										ch = getch();
									}
								}
								break;
							default:
								break;
						}
						//endwin();
						//printf("%s\n",html_element_index_names[selected->tag]);
						//printf("innertext: '%s'\n",selected->innertext);
						//exit(0);
					} else {
						//attrset(COLOR_PAIR(1));
						//printw("nul");
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
				case 339: /* PAGE UP */
					if (y != 0) {
						y = 0;
					} else {
						scroll_y -= max_y;
						if (scroll_y < 0) {
							scroll_y = 0;
						}
					}
					break;
				case 338: /* PAGE DOWN */
					if (y != max_y - 1) {
						y = max_y - 1;
					} else {
						scroll_y = min(max_scroll_y - 1, scroll_y + max_y);
					}
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

						move(max_y - 1, x);
						cur_row = old_scroll_y + max_y - 1;
						for (i = scroll_x; i < scroll_x + max_x && i < strlen(lines[cur_row]) && lines[cur_row][i];) {
							if (lines[cur_row][i] == DC1 || lines[cur_row][i] == DC2) {
								i++;
								set_attributes((unsigned char)lines[cur_row][i++]);
								continue;
							}
							addch(lines[cur_row][i++]);
						}
					}
				} else {
					int i;
					while (scroll_y < old_scroll_y) {
						scrl(-1);
						old_scroll_y--;

						move(0,0);
						for (i = scroll_x; i < scroll_x + max_x && i < strlen(lines[old_scroll_y]) && lines[old_scroll_y][i];) {
							if (lines[old_scroll_y][i] == DC1 || lines[old_scroll_y][i] == DC2) {
								i++;
								set_attributes((unsigned char)lines[old_scroll_y][i++]);
								continue;
							}
							addch(lines[old_scroll_y][i++]);
						}
					}
				}
			}
		} else {
			wbkgd(stdscr,COLOR_PAIR(1));
			clear();
			move(0,0);
			int i,j;
			for (j = scroll_y; j < scroll_y + max_y && lines[j]; j++) {
				for (i = scroll_x; i < scroll_x + max_x && i < strlen(lines[j]) && lines[j][i];) {
					if (lines[j][i] == DC1 || lines[j][i] == DC2) {
						i++;
						set_attributes((unsigned char)lines[j][i++]);
						continue;
					}
					addch(lines[j][i++]);
				}
				addch('\n');
			}
			if (!is_not_firstloop) {
			  //if (is_html) { scrl(-1); }
				is_not_firstloop = 1;
			}
		}		
		if (is_html) {
			int i;
			for (i = 0; i < list_form_inputs_len; i++) {
				if (IMPORTANT(list_form_inputs[i]->tag)) {
					struct html_element *cur = list_form_inputs[i];
					if (cur->lx >= scroll_x && cur->ly >= scroll_y && cur->rx < scroll_x + max_x && cur->ry < scroll_y + max_y) {
						move(cur->ly - scroll_y, cur->lx - scroll_x);
						set_attributes(gen_console_attributes_char(cur));
						int j;
						for (j = 0; j < cur->properties_length; j++) {
							if (!strcmp(cur->properties[j]->key, "value")) {
								printw("%s",cur->properties[j]->value);
								break;
							}
						}
					}
				}
			}
		}

		move(y,x);
		refresh();		
	}
	timeout(-1);
	getch();
	endwin();
	
	if (is_html) {
		print_html_structure(html, 1);
		free_html_element(html);

		printf("list_form_inputs_len: %d\n",list_form_inputs_len);
		int i;
		for (i = 0; i < list_form_inputs_len; i++) {
			printf("list_form_inputs[%d]: %s\n",i,html_element_index_names[list_form_inputs[i]->tag]);
		}

	} else {
		printf("max_scroll_x: %d\n", max_scroll_x);
		printf("max_scroll_y: %d\n", max_scroll_y);
		printf("max_x: %d\n", max_x);
		printf("max_y: %d\n", max_y);
		printf("scroll_x: %d\n", scroll_x);
		printf("scroll_y: %d\n", scroll_y);
	}
	free(lines);
	free(output);	

	/*
	printf("---- end ----\n");
	int index;
	for (index = 0; index < html_element_index_names_len; index++) {
		printf("%s -> %d\n",html_element_index_names[index], index);
	}
	*/

	free(site);
	free(path);
	free(dwld_file);
	
	return 0;
}
