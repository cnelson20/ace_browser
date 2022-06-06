#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "browse.h"
#include <ctype.h>

char html_symbols[][2][8] = {
	{"&nbsp;" , " "},
	{"&emsp;" , "    "},
	{"&raquo;" , ">>"},
	{"&laquo;" , "<<"},
	{"&lt;" , "<"},
	{"&gr;" , ">"},
	{"…" , "..."},
	{"’" , "'"},
};
size_t html_symbols_len = sizeof(html_symbols) / sizeof(html_symbols[0]);

char html_element_index_names[][16] = {
"html",  // 0

"base",  // 1
"head",  // 2
"link",  // 3
"meta",  // 4
"style",  // 5
"title",  //  6
 
"body",  // 7
 
"address",  // 8
"article",  // 9
"aside",  // 10
"footer",  // 11
"header", // 12
"h1",  // 13
"h2",  // 14
"h3",  // 15
"h4",  // 16
"h5",  // 17
"h6",  // 18
"main",  // 19
"nav",  // 20
"section",  // 21
 
"blockquote",  // 22
"dd",  // 23
"div",  // 24
"dl",  // 25
"dt",  // 26
"figcaption",  // 27
"figure",  // 28
"hr",  // 29
"li",  // 30
"ol",  // 31
"p",  // 32
"pre",  // 33
"ul",  // 34
 
"a",  // 35
"abbr",  // 36
"b",  // 37 
"bdi",  // 38
"bdo",  // 39
"br",  // 40
"cite",  // 41
"code",  // 42
"data",  // 43
"dfn",  // 44
"em",  // 45
"i",  // 46
"kbd",  // 47
"mark",  // 48
"q",  // 49
"rp",  // 50
"rt",  // 51
"ruby",  // 52
"s",  // 53
"samp",  // 54 
"small",  // 55
"span",  // 56
"strong",  // 57
"sub",  // 58
"sup",  // 59
"time",  // 60
"u",  // 61
"var",  // 62
"wbr",  // 63
 
"area",  // 64
"audio",  // 65
"img",  // 66
"map",  // 67
"track",  // 68
"video",  // 69
 
"embed",  // 70
"iframe",  // 71
"object",  // 72
"param",  // 73
"picture",  // 74
"portal",  // 75
"source",  // 76
 
"svg",  // 77
"math",  // 78
 
"canvas",  // 79
"noscript",  // 80
"script",  // 81
 
"del",  // 82
"ins",  // 83
 
"caption",  // 84
"col",  // 85
"colgroup",  // 86
"table",  // 87
"tbody",  // 88
"td",  // 89
"tfoot",  // 90 
"th",  // 91
"thead",  // 92 
"tr", // 93

"button", // 94
"datalist",  // 95
"fieldset",  // 96
"form",  // 97
"input",  // 98
"label",  // 99
"legend",  // 100
"meter",  // 101
"optgroup",  // 102
"option",  // 103
"output",  // 104
"progress", // 105
"select", // 106
"textarea", // 107

"details", // 108 
"dialog", // 109
"menu", // 110
"summary", // 111

"noframes" // 112
};
size_t html_element_index_names_len = sizeof(html_element_index_names) / sizeof(html_element_index_names[0]);

char *file;
char *output;
char *output_temp;
size_t output_size;

struct html_element *html;
int is_html;

/* 
	Given a string of the tag name,
	return an int corresponding to the tag
*/
int get_html_element_index(char *name) {
	int i;
	for (i = 0; i < sizeof(html_element_index_names) / sizeof(html_element_index_names[0]); i++) {
		if (!strcmp(html_element_index_names[i],name)) {
			return i;
		}
	}
	return -1;
}

/* 
	Init an html element 
	Note: does not setup innertext field,
*/
int init_html_element(struct html_element *html, struct html_element *f_parent, char *def, size_t def_length) {
	//printf("---- define html->parent ----\n");
	html->parent = f_parent;
	//printf("---- define html->children ----\n");
	html->children = NULL;
	//printf("---- define html->num_children ----\n");
	html->num_children = 0;
	
	/* get tag name ('body', 'div', 'a', etc.) */
	//printf("---- define html->tag ----\n");
	static_tolowern(def,(char *)minpointer_nnull(strchr(def,' '),strchr(def,'>')) - def);
	int element_i = get_html_element_index(static_tolower_string);
	if (element_i == -1) {return -1;}
	html->tag = element_i;
	
	//printf("---- define html->x y ----\n");
	html->lx = -1;
	html->ly = -1;
	html->rx = -1;
	html->ry = -1;

	//printf("---- properties ----\n");
	if (strchr(def,' ') != NULL && strchr(def,' ') < strchr(def,'>')) {
		//printf("---- defining html->properties ----\n");
		//printf("strchr ' ': %p  strchr '>': %p\n",strchr(def,' '),strchr(def,'>'));
		char *copyfrom = strchr(def,' ') + 1;
		html->properties = malloc(0);
		html->properties_length = 0;
		while (1) {
			static_tolower(copyfrom);
			//printf("copyfrom: %s\n", static_tolower_string);
			if (*copyfrom == '>' || (*copyfrom == '/' && *(copyfrom+1) == '>') || !strchr(copyfrom,'=')) {break;}

			html->properties = realloc(html->properties, sizeof(struct key_value_pair *) * (1 + html->properties_length));
			html->properties[html->properties_length] = malloc(sizeof(struct key_value_pair));
			char *property_name_end = minpointer_nnull(strchr(copyfrom, '='), strchr(copyfrom, '>'));
			if (*property_name_end == '=') {
				*property_name_end = '\0';
				html->properties[html->properties_length]->key = malloc(strlen(copyfrom) + 1);
				strcpy(html->properties[html->properties_length]->key,copyfrom);
				strip_whitespace_inplace(html->properties[html->properties_length]->key);
				tolower_inplace(html->properties[html->properties_length]->key);
				*property_name_end = '=';
				copyfrom = minpointer_nnull(strchr(property_name_end, '"'),strchr(property_name_end,'\''));
				char quote_type = *copyfrom;
				copyfrom++; 
				int value_length = strchr(copyfrom,quote_type) - copyfrom;
				html->properties[html->properties_length]->value = malloc(value_length + 1);
				memcpy(html->properties[html->properties_length]->value, copyfrom, value_length);
				*(html->properties[html->properties_length]->value + value_length) = '\0';
				copyfrom += value_length + 1;
			} else {
				if (*(property_name_end - 1) == '/') {
					property_name_end--;
				}
				char temp_char = *property_name_end;
				*property_name_end = '\0';
				
				html->properties[html->properties_length]->key = malloc(strlen(copyfrom) + 1);
				strcpy(html->properties[html->properties_length]->key,copyfrom);
				strip_whitespace_inplace(html->properties[html->properties_length]->key);
				tolower_inplace(html->properties[html->properties_length]->key);
				html->properties[html->properties_length]->value = malloc(strlen("") + 1);
				strcpy(html->properties[html->properties_length]->value, "");
				*property_name_end = temp_char;
				copyfrom = strchr(property_name_end,'>');
			}
			html->properties_length++;
		}	
		//html->properties = malloc(min(strlen(copyfrom), strchr(copyfrom,'>') - copyfrom));
		//strncpy(html->properties,copyfrom, min(strlen(copyfrom),strchr(copyfrom,'>') - copyfrom));
	} else {
		//printf("---- html->properties is null ----\n");
		html->properties = NULL;
		html->properties_length = 0;	
	}
	//printf("---- end define ----\n");

	return 0;
}

/*
	Frees a html element recursively
*/
void free_html_element(struct html_element *html) {	
	if (html->num_children != 0) {
		int i;
		for (i = 0; i < html->num_children; i++) {
			free_html_element(html->children[i]);
		}
		free(html->children);
	}
	
	if (html->properties != NULL) {
		int i;
		for (i = 0; i < html->properties_length; i++) {
			free(html->properties[i]->key);
			free(html->properties[i]->value);
			free(html->properties[i]);
		}
		free(html->properties);
	}
	free(html->innertext);
	
	free(html);
}

/* 
	Generates a numerical char that represents attributes of an html for ncurses
*/
char gen_console_attributes_char(struct html_element *html) {
	unsigned char val = 0;
	unsigned char color = 0x01;
	if (html->tag == ELEMENT_A) { val |= 64; color = 0x05;}
	if (html->tag == ELEMENT_B || (html->tag >= ELEMENT_H1 && html->tag <= ELEMENT_H6) || html->tag == ELEMENT_STRONG) { val |= 128; }
	if (html->tag == ELEMENT_TEXTAREA || html->tag == ELEMENT_INPUT) {color = 0x08;}
	val |= color;
	
	return (char)val;
}

/* 
	Print out an struct html_element's innertext replacing DEL characters with a baby emoji (symbolizes child)
*/
void print_innertext(char *s) {
	if (s == NULL) {
		printf("~~(null)~~");	
		return;
	}
	while (s && *s) {
		if (*s == 127) {
			printf("👶");
		} else {
			putchar(*s);
		}
		s++;
	}
}
/* 
	Recursively prints out data of a html tree
*/
void print_html_structure(struct html_element *html, unsigned char rec) {
	printf("------------------------\n");
	printf("Element: Tag: (%d) %s\n",html->tag,html_element_index_names[html->tag]);
	if (html->properties != NULL) {
		int i;
		for (i = 0; i < html->properties_length; i++) {
			printf("Property (%d): '%s' = '%s'\n",i,html->properties[i]->key, html->properties[i]->value);
		}
	}
	printf("innerHTML: '");
	print_innertext(html->innertext);
	printf("'\n");
	printf("lx: %d  ly: %d rx: %d  ry: %d\n\n", html->lx, html->ly, html->rx, html->ry);

	if (html->parent != NULL) {
		printf("Parent: %s\n",html_element_index_names[html->parent->tag]);
	}
	if (html->num_children != 0) {
		int i;		
		printf("Children (%d):\n",html->num_children);
		for (i = 0; i < html->num_children; i++) {
			printf("[%d]:  %s\n",i,html_element_index_names[html->children[i]->tag]);
		}
		for (i = 0; i < html->num_children; i++) {
			if (rec) {
				print_html_structure(html->children[i],1);
			} else {
				printf("%s\n",html_element_index_names[html->children[i]->tag]);
			}
		}
	} else {
		printf("No children\n");
	}
}

/* 
	Prints the path of a specific html element
	ex: "html.body.div.div.img"
*/
void print_element_path_nonewline(struct html_element *html) {
	if (html->parent == NULL) {
		printf("%s",html_element_index_names[html->tag]);
	} else {
		print_element_path_nonewline(html->parent);
		printf(".%s",html_element_index_names[html->tag]);
	}
}

/* 
	Calls above function, adds newline 
*/
void print_element_path(struct html_element *html) {
	print_element_path_nonewline(html);
	putchar('\n');
}

/*
	Calls print_element_path for each element in a tree,
	recursively if rec = 1
*/
void test_print_structure(struct html_element *html) {
	print_element_path(html);
	if (html->num_children != 0) {
		int i;		
		for (i = 0; i < html->num_children; i++) {
			test_print_structure(html->children[i]);
		}
	}
}


/* 
	Kinda renders the html dir structure 
*/
char *render_page(struct html_element *html, struct html_element *body) {
	if (output == NULL) {
		output_size = 1024;
		output = malloc(output_size);
		output_temp = output;
		*output_temp = '\0';
	}
	char *s = body->innertext;
	int i = 0;
	//print_html_structure(body,0);
	
	if (!DONTPRINT(body->tag)) {
		if (output_temp - output + sizeof(struct html_element *) >= output_size) {
			output_size *= 2;
			output = realloc(output,output_size);
			output_temp = output + strlen(output);
		}
		*(output_temp++) = 0x11;
		
		*(output_temp++) = gen_console_attributes_char(body);
		
		*output_temp = '\0';
		
		while (*s) {
			if (*s == 127) {
				if (ISBLOCKLEVEL(body->children[i]->tag)) {
					if (output_temp > output) {
						char *ot = output_temp - 1;
						while (ot > output) {
							//printf("*ot: #%d '%c'\n", *ot, *ot);
							if (*ot == DC2) {
								ot--;
							} else if (*(ot - 1) == DC1) {
								ot -= 2;
							} else {
								break;
							}
						}
						if (ot <= output && *ot != '\n') {
							if (output_temp - output >= output_size) {
								output_size *= 2;
								output = realloc(output,output_size);
								output_temp = output + strlen(output);
							}
							*(output_temp++) = '\n';
							*(output_temp) = '\0';
						}
					}
					render_page(html,body->children[i]);
					output_temp = output + strlen(output);
					if (output_temp > output && *(output_temp-1) != '\n') {
						if (output_temp - output >= output_size) {
							output_size *= 2;
							output = realloc(output,output_size);
							output_temp = output + strlen(output);
						}
						*(output_temp++) = '\n';
					}
					
					i++;
				} else {
					render_page(html,body->children[i]);
					output_temp = output + strlen(output);
					i++;
				}
			} else {
				if (output_temp - output >= output_size) {
					output_size *= 2;
					output = realloc(output,output_size);
					output_temp = output + strlen(output);
				}
				*(output_temp++) = *s;
				*(output_temp) = '\0';
			}
			s++;
		}
		
		if (output_temp - output >= output_size - 1) {
			output_size *= 2;
			output = realloc(output,output_size);
			output_temp = output + strlen(output);
		}
		*(output_temp++) = DC2;
		if (body->parent != NULL) {
			*(output_temp++) = gen_console_attributes_char(body->parent);
		} else {
			*(output_temp++) = 1;
		}
		*output_temp = '\0';
	}
	
	return output_temp;
}

/* 
   Compares strings discarding case 
*/
int stricmp(const char *s1, const char *s2) {
   while (toupper(*s1) == toupper(*s2)) {
     if (*s1 == 0)
       return 0;
     s1++;
     s2++;
   }
   return toupper(*(unsigned const char *)s1) - toupper(*(unsigned const char *)(s2));
 }

/*
	strips whitespace in a string in-place
*/
size_t strip_whitespace_inplace(char *s) {
	size_t og_strlen = strlen(s);
	while (*s) {
		if (is_whitespace_char(*s)) {
			char *t = s;
			while (*t) {
				*t = *(t + 1);
				t++;
			}
		} else {
			s++;
		}
	}
	return og_strlen;
}

/* 
	Converts string to lowercase in-place (overwrites characters)
*/
void tolower_inplace(char *t) {
	while (*t) {
        if (*t >= 'A' && *t <= 'Z') {
	    *t = *t + ('a' - 'A');
	}
	t++;
    }
}

/* 
	returns the minimum of two ints
*/
int min(int a, int b) {
	return (a < b ? a : b);
}

/*
	return the maximum of two ints
*/
int max(int a, int b) {
	return (a >= b ? a : b);
}

/*
	returns the smallest power of 2 >= n
*/
int smallest_pow2(int n) {
	int log2 = 0;
	n--;
	while (n > 1) {
		n /= 2;
		log2++;
	}
	log2++;
	n = 1;
	while (log2 > 0) {
		n *= 2;
		log2--;
	}

	return n;
}

/*
	returns the lesser pointer that is not null
*/
void *minpointer_nnull(void *a, void *b) {
	if (!a) { return b; }
	if (!b) { return a; }
	
	return (a < b ? a : b);
}

/*
	strchr, but if the find fails return the null byte at the end of s
*/
char *mystrchrnul(const char *s, int c) {
	char *r = strchr(s,c);
	if (r) {return r;}
	else {
		return (char *)s + strlen(s);
	}
}

/*
	Returns whether is an char is considered whitespace
*/
int is_whitespace_char(int c) {
	return c == '\n' || c == '\f' || c == '\r' || c == '\t' || c == ' ';
}

/* 
	Copies n - 1 bytes of string to static_tolower_string (+ a null byte),
	then converts it to lowercase
*/
char static_tolower_string[100];
char *static_tolowern(char *string, size_t n) {
    char *t = static_tolower_string;
    strncpy(static_tolower_string,string,min(sizeof(static_tolower_string)-1,n));
    if (strlen(string) > n) {
        static_tolower_string[n] = '\0';
    }
    tolower_inplace(static_tolower_string);
    
    return static_tolower_string;
}
/* 
	Calls static_tolowern with n as strlen(string) + 1
*/
char *static_tolower(char *string) {
	return static_tolowern(string,strlen(string)+1);
}

/*
	Generates a unsigned char[] of size len with 
	whether each byte of html is in quotes
*/
unsigned char *geninquotes_html(char *html, size_t len) {
	int i, inbrackets, inquotes;
	char quotetype;
	unsigned char *quotes = malloc(len);
	
	inbrackets = 0;
	inquotes = 0;
	for (i = 0; i < len; i++) {
		if (inbrackets) {
			if (inquotes) {
				if (html[i] == quotetype) {
					inquotes = 0;
				}
			} else if (html[i] == '"' || html[i] == '\'') {
				inquotes = 1;
				quotetype = html[i];
			} else if (html[i] == '>') {
				inbrackets = 0;
			}
		} else if (!inquotes && html[i] == '<') {
			inbrackets = 1;
		}
		quotes[i] = inquotes;
	}
	
	return quotes;
}

/*
	Given a int representing a html element tag,
	return a string of a default innerhtml
	ex: get_default_innerhtml(ELEMENT_BR) -> "\n"
*/
char *get_default_innerhtml(struct html_element *elem) {
	switch (elem->tag) {
		case ELEMENT_BR:
			return (char *)"\n";
		case ELEMENT_HR:
			return (char *)"------------------------\n";
		case ELEMENT_TEXTAREA:
			return (char *)"                            ";
		case ELEMENT_INPUT:
			;
			int i;
			for (i = 0; i < elem->properties_length; i++) {
				if (!strcmp(elem->properties[i]->key, "type")) {
					char *name_value = elem->properties[i]->value;
					if (!stricmp(name_value, "text") || !stricmp(name_value, "password")) {
						return (char *)"            ";
					} else if (!stricmp(name_value, "submit")) {
						for (i = 0; i < elem->properties_length; i++) {
							if (!strcmp(elem->properties[i]->key, "value")) {
								return elem->properties[i]->value;
							}
						}
						return (char *)"Submit";
					} else if (!stricmp(name_value, "hidden")) {
						return (char *)"";
					}
					return "~~INPUT~~";
				}
			}
			return NULL;
		default:
			return NULL;
	}
}

char *render_html_file(char *filename, char *output_filename) {
	struct stat fileinfo;

	//printf("---- render_html_file() ----\n");
	if (html) {
	    free_html_element(html);
	}
	
	//printf("---- reading from [downloaded] file ----\n");
	stat(filename,&fileinfo);
	int fd = open(filename, O_RDONLY);
	if (fd == -1) {printf("fuck\n");}
	file = malloc(fileinfo.st_size+1);
	read(fd,file,fileinfo.st_size);
	close(fd);
	//printf("---- done with file ----\n");

	//printf("---- calling geninquotes_html() ----\n");
	char *qts = geninquotes_html(file,strlen(file));
	char *cur = file;
	
	html = NULL;
	is_html = 1;
	if (!strchr(file,'<') || !strchr(file,'>')) {
		is_html = 0;
	} 
	if (is_html) {
		static_tolowern(strchr(file,'<'),strchr(file,'>') - strchr(file,'<'));
		printf("Declaration: '%s'\n",static_tolower_string);
		is_html = (memcmp(static_tolower_string,"<!doctype html", strlen("<!doctype html")) == 0 || memcmp(static_tolower_string,"<html", strchr(file,'>') - strchr(file,'<')) == 0);
	}
	if (is_html == 0) {
		is_html = 0;
		output = file;
		output_size = strlen(output);
		return output;
	}
	struct html_element *elem = NULL;
	
	while (1) {
		cur = strchr(cur,'<');	
		static_tolowern(cur+1,4);
		if (!stricmp(static_tolower_string,"html")) {
			break;
		} else {
			cur++;
		}
	}
	if (!stricmp(static_tolower_string,"html")) {
		html = calloc(1,sizeof(struct html_element));
		//printf("cur: '%s'\n",cur);
		init_html_element(html,NULL,cur+1,strchr(cur+1,'>') - (cur+1));
		html->innertext = malloc(32);
		html->innertext[0] = '\0';
		html->innertext_size = 32;
		html->innertext_length = 0;
		
		elem = html;
		
		cur = strchr(cur,'>');
		while (qts[cur - file]) {
			cur = strchr(cur+1,'>');
		}
		cur++;
	}
	
	int strlen_file = strlen(file);
	while (elem != NULL && cur < file + strlen_file) {
		int de_cur_equals = (*cur == '<');
		if (de_cur_equals) {
			if (elem->tag == ELEMENT_SCRIPT) {
			  	if (*(cur+1) == '/') {
					static_tolowern(cur+2,strlen("script"));
					//printf("~~CUR~~: '%s'\n",static_tolower_string);
					if (!strcmp(static_tolower_string,"script") && !qts[cur - file + 2]) {
						elem = elem->parent;
						cur = strchr(cur,'>')+1;
					} else {
						de_cur_equals = 0;
					}
			  	} else {
					de_cur_equals = 0;
			  	}
			} else if (*(cur+1) != '/') {
				/* new element */
				if (elem->innertext_length >= elem->innertext_size - 1) {
					elem->innertext = realloc(elem->innertext, elem->innertext_size * 2);
					elem->innertext_size *= 2;
				}	
				elem->innertext[(elem->innertext_length)++] = 127;
				elem->innertext[elem->innertext_length] = '\0';				
				
				elem->children = realloc(elem->children, (2+elem->num_children) * sizeof(struct html_element *));
				elem->children[elem->num_children] = calloc(1,sizeof(struct html_element));
				elem->children[elem->num_children + 1] = NULL;
				
				if (init_html_element(elem->children[elem->num_children],elem,cur+1,strchr(cur+1,'>') - (cur+1)) == -1) {
					free(elem->children[elem->num_children]);
					elem->children[elem->num_children] = NULL;
				} else {
				elem = elem->children[elem->num_children++];
				}
				
				char *t = get_default_innerhtml(elem);
				if (ISVOIDELEMENT(elem->tag)) {
					//printf("%s (%d): is a voidelement\n",html_element_index_names[elem->tag],elem->tag);
					if (t != NULL) {
						elem->innertext_size = strlen(t)+1;
						elem->innertext = malloc(elem->innertext_size);
						strcpy(elem->innertext,t);
						elem->innertext_length = strlen(elem->innertext);
					} else {
						elem->innertext = NULL;
						elem->innertext_size = 0;
						elem->innertext_length = 0;
					}
				} else {
					//printf("%s (%d): is not a voidelement\n",html_element_index_names[elem->tag], elem->tag);
					if (t != NULL) {
						elem->innertext_size = smallest_pow2(strlen(t) + 1);
						elem->innertext = malloc(elem->innertext_size);
						//elem->innertext[0] = '\0';
						strcpy(elem->innertext, t);
						elem->innertext_length = strlen(elem->innertext);
 					} else {
						elem->innertext_size = 32;
						elem->innertext = malloc(elem->innertext_size);
						elem->innertext[0] = '\0';
						elem->innertext_length = 0;
					}
				}
				
				cur = strchr(cur,'>');
				while (qts[cur - file]) {
					cur = strchr(cur+1,'>');
				}
				if (ISVOIDELEMENT(elem->tag)) {
					elem = elem->parent;
				}
				cur++;
			} else {
				/* closing tag */
				if (elem->innertext_length > 0 && is_whitespace_char(elem->innertext[elem->innertext_length-1])) {
					elem->innertext[--elem->innertext_length] = '\0';	
				}
				elem = elem->parent;
				cur = strchr(cur,'>')+1;
			}		
		} 
		if (!de_cur_equals) {
			if (!is_whitespace_char(*cur) || (elem->innertext_length != 0 && elem->innertext[elem->innertext_length - 1] != 127 && !is_whitespace_char(*(cur-1)))) {
				if (*cur == '&' && mystrchrnul(cur,';') < strchr(cur,' ')) {
					static_tolowern(cur, strchr(cur,';') + 1 - cur);
					//printf("static_tolower_string: '%s'\n", static_tolower_string);
					int i;
					for (i = 0; i < html_symbols_len; i++) {
						if (!strcmp(static_tolower_string, html_symbols[i][0])) {
							if (elem->innertext_length >= elem->innertext_size - 1 - strlen(html_symbols[i][1])) {
								elem->innertext = realloc(elem->innertext, elem->innertext_size * 2);
								elem->innertext_size *= 2;
							}	
							strcpy(elem->innertext + elem->innertext_length, html_symbols[i][1]);
							elem->innertext_length += strlen(html_symbols[i][1]);
							cur = strchr(cur,';') + 1;
							goto endLabel;
						}
					}
				}
				
				if (elem->innertext_length >= elem->innertext_size - 1) {
					elem->innertext = realloc(elem->innertext, elem->innertext_size * 2);
					elem->innertext_size *= 2;
				}
				if (*cur == '\n' || *cur == '\r' || *cur == '\t') {
					elem->innertext[(elem->innertext_length)++] = ' ';
					cur++;
				} else {
					elem->innertext[(elem->innertext_length)++] = *(cur++);
				}
				elem->innertext[elem->innertext_length] = '\0';

				endLabel:
				;
			} else {
			  cur++;	
			}
		}
	}
	
	//print_html_structure(html,1);
	
	printf("\n------------------------\n");
	test_print_structure(html);
	printf("\n------------------------\n");
	
	//printf("---- calling render_page() ----\n");
	render_page(html,html);
	if (output_filename != NULL) {
    	fd = open(output_filename,O_WRONLY | O_CREAT | O_TRUNC, 0644);
     	write(fd,output,strlen(output));
		close(fd);
	}

	free(file);
	free(qts);
	
	//output = realloc(output,strlen(output)+1);
	return output;
}
