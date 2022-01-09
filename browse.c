#include "browse.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

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
"col"  // 85
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
	Note: does not setup inner_text field,
*/
int init_html_element(struct html_element *html, struct html_element *f_parent, char *def, size_t def_length) {
	printf("start init\n");
	printf("def: '%s'\n",def);
	
	html->parent = f_parent;
	html->children = NULL;
	html->num_children = 0;
	
	static_tolowern(def,(char *)minpointer_nnull(strchr(def,' '),strchr(def,'>')) - def);
	printf("static_tolower_string: '%s'\n",static_tolower_string);
	int element_i = get_html_element_index(static_tolower_string);
	if (element_i == -1) {return -1;}
	html->tag = element_i;
	printf("element_i: %d\n", element_i);
	
	if (strchr(def,' ') < strchr(def,'>')) {
		char *copyfrom = strchr(def,' ') +1;
		html->properties = malloc(min(strlen(copyfrom), strchr(copyfrom,'>') - copyfrom));
		strncpy(html->properties,copyfrom, min(strlen(copyfrom),strchr(copyfrom,'>') - copyfrom));
	} else {
		html->properties = NULL;	
	}
	
	printf("done initing\n");
	
	return 0;
}

void print_html_structure(struct html_element *html) {
	printf("------------------------\n");
	printf("Element: Tag: (%d) %s\n",html->tag,html_element_index_names[html->tag]);
	printf("Properties: '%s'\n",html->properties);
	printf("innerHTML: '%s'\n",html->innertext);
	
	if (html->parent != NULL) {
		printf("Parent: %s\n",html_element_index_names[html->parent->tag]);
	}
	if (html->num_children != 0) {
		int i;		
		printf("Children (%d):\n",html->num_children);
		for (i = 0; i < html->num_children; i++) {
			print_html_structure(html->children[i]);
		}
	}
}

int min(int a, int b) {
	return (a < b ? a : b);
}
void *minpointer_nnull(void *a, void *b) {
	if (!a) { return b; }
	if (!b) { return a; }
	
	return (a < b ? a : b);
}
int is_whitespace_char(int c) {
	return c == 0xa || c == 0xc || c == 0xd || c == 0x20;
}

char static_tolower_string[100];
char *static_tolowern(char *string, size_t n) {
	char *t;
	strncpy(static_tolower_string,string,min(sizeof(static_tolower_string)-1,n));
	if (strlen(string) > n) {
		static_tolower_string[n] = '\0';
	}
	while (*t) {
		if (*t >= 'A' && *t <= 'Z') {
			*t = *t + ('a' - 'A');
		}
		t++;
	}
	
	return static_tolower_string;
}
char *static_tolower(char *string) {
	return static_tolowern(string,strlen(string)+1);
}

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

char *file;

int main(int argc, char *argv[]) {
	/*
	int i;
	for (i = 0; i < sizeof(html_element_index_names) / sizeof(html_element_index_names[0]); i++) {
		printf("[%d]: %s\n",i,html_element_index_names[i]); 
	}
	*/
	
	if (argc <= 1) {printf("give file\n"); return 1;}
	struct stat fileinfo;
	stat(argv[1],&fileinfo);
	int fd = open(argv[1], O_RDONLY);
	if (fd == -1) {printf("fuck\n");}
	file = malloc(fileinfo.st_size+1);
	read(fd,file,fileinfo.st_size);
	close(fd);
	
	char *qts = geninquotes_html(file,strlen(file));
	char *cur = file;
	
	struct html_element *html = NULL;
	struct html_element *elem = NULL;
	
	while (1) {
		cur = strchr(cur,'<');	
		static_tolowern(cur+1,4);
		if (!strcmp(static_tolower_string,"html")) {
			break;
		} else {
			cur++;
		}
	}
	if (!strcmp(static_tolower_string,"html")) {
		html = calloc(1,sizeof(struct html_element));
		printf("cur: '%s'\n",cur);
		init_html_element(html,NULL,cur+1,strchr(cur+1,'>') - (cur+1));
		html->innertext = malloc(32);
		html->innertext[0] = '\0';
		html->innertext_size = 32;
		html->innertext_length = 0;
		
		elem = html;
		printf("------------------------\n");
		printf("Element: Tag: (%d) %s\n",elem->tag,html_element_index_names[elem->tag]);
		printf("Properties: '%s'\n",elem->properties);
		printf("------------------------\n");
		
		cur = strchr(cur,'>');
		while (qts[cur - file]) {
			cur = strchr(cur+1,'>');
		}
		cur++;
	}
	
	while (elem != NULL && cur < file + strlen(file)) {
		//printf("curr: %c%c%c%c\n",*cur,*(cur+1),*(cur+2),*(cur+3));
		if (*cur == '<') {
			if (*(cur+1) != '/') {
				/* new element */
				if (elem->innertext_length >= elem->innertext_size - 1) {
					elem->innertext = realloc(elem->innertext, elem->innertext_size * 2);
					elem->innertext_size *= 2;
				}	
				elem->innertext[(elem->innertext_length)++] = 127;
				elem->innertext[elem->innertext_length] = '\0';				
				
				elem->children = realloc(elem->children, (1+elem->num_children) * sizeof(struct html_element *));
				elem->children[elem->num_children] = calloc(1,sizeof(struct html_element));
				
				init_html_element(elem->children[elem->num_children],elem,cur+1,strchr(cur+1,'>') - (cur+1));
				elem = elem->children[elem->num_children++];
				
				if (ISVOIDELEMENT(elem->tag)) {
					elem->innertext = NULL;
					elem->innertext_size = 0;
					elem->innertext_length = 0;
				} else {
					elem->innertext = malloc(32);
					elem->innertext[0] = '\0';
					elem->innertext_size = 32;
					elem->innertext_length = 0;
				}
					
				printf("------------------------\n");
				printf("Element: Tag: (%d) %s\n",elem->tag,html_element_index_names[elem->tag]);
				//printf("Parent: '%s'\n",html_element_index_names[elem->parent->tag]);
				printf("Properties: '%s'\n",elem->properties);
				printf("------------------------\n");
				
				
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
				elem = elem->parent;
				cur = strchr(cur,'>')+1;
			}		
		} else {
			if (!is_whitespace_char(*cur) || (elem->innertext_length != 0 && !is_whitespace_char(*(cur-1)))) {
				if (elem->innertext_length >= elem->innertext_size - 1) {
					elem->innertext = realloc(elem->innertext, elem->innertext_size * 2);
					elem->innertext_size *= 2;
				}	
				elem->innertext[(elem->innertext_length)++] = *(cur++);
				elem->innertext[elem->innertext_length] = '\0';
			} else {
			  cur++;	
			}
		}
	}
	
	print_html_structure(html);
	
	free(file);
	free(qts);
	
	printf("ELEMENT_BR: %d\tget_html_element_index(\"br\"): %d\n",ELEMENT_BR,get_html_element_index("br"));
	return 0;
}