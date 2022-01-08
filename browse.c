#include "browse.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

char html_element_index_names[][16] = {
"html",
"base",
"head",
"link",
"meta",
"style",
"title",	

"body",

"address",
"article",
"aside",
"footer",
"h1",
"h2",
"h3",
"h4",
"h5",
"h6",
"main",
"nav",
"section",

"blockquote",
"dd",
"div",
"dl",
"dt",
"figcaption",
"figure",
"hr",
"li",
"ol",
"p",
"pre",
"ul",

"a",
"abbr",
"b",
"bdi",
"bdo",
"br",
"cite",
"code",
"data",
"dfn",
"em",
"i",
"kbd",
"mark",
"q",
"rp",
"rt",
"ruby",
"s",
"samp",
"small",
"span",
"strong",
"sub",
"sup",
"time",
"u",
"var",
"wbr",

"area",
"audio",
"img",
"map",
"track",
"video",

"embed",
"iframe",
"object",
"param",
"picture",
"portal",
"source",

"svg",
"math",

"canvas",
"noscript",
"script",

"del",
"ins",

"caption",
"col"
"colgroup",
"table",
"tbody",
"td",
"tfoot",
"th",
"thead",
"tr",

"button",
"datalist",
"fieldset",
"form",
"input",
"label",
"legend",
"meter",
"optgroup",
"option",
"output",
"progress",
"select",
"textarea",

"details",
"dialog",
"menu",
"summary",

"noframes"
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

//char file[] = " <html> <head> <title> Test </title> </head> <body style=\"text-decoration:underline\"> Hello World! </body> </html>"; 
/*char file[] = "<html>\
    <head>\
        <style>\
            .whitetext {\
                color:white;\
            }   \
            body {\
                background-color:black;\
                display:flex;\
                flex-direction:row;\
                align-items:stretch;\
                position:fixed;\
                padding:0;\
                margin:0;\
                left:0;\
                width:100%;\
                top:0;\
                height:100%;\
            }\
            .sidebar {\
                background-color:#606060;\
            }\
            .vertical-flex {\
                display:flex;\
                flex-direction:column;   \
                flex-grow:1;\
            }\
            .padding-2 {\
                padding-left:2%;\
                padding-top:2%;\
            }\
            .underline {\
                text-decoration:underline;\
            }\
            a {\
                color:#72bdd4;\
            }\
        </style>\
        <title> grixisutils home </title>\
    </head>\
    <body>\
        <div class=\"sidebar whitetext\">\
            <div style=\"padding-left:5%;padding-top:5%;\">\
                <span class=\"underline\"> Utilities </span>\
                <br>\
                <a href=\"mtgoextrabuilder/\"> MTGO extras binder builder </a>\
                <br>\
                <a href=\"aocmirror/\"> Advent of Code Mirror </a>\
                <br>\
                <a href=\"dw_slides_backup/\"> Backup of Systems slides </a>\
                <br>\
                <br>\
                <span class=\"underline\"> Subdomains </span>\
                <br>\
                <a href=\"https://www.bark.grixisutils.site\"> bark </a>\
                <br>\
                <a href=\"https://www.ovs.grixisutils.site\"> ovstutor </a>\
            </div>\
        </div>\
        <div class=\"vertical-flex\">\
            <div class=\"padding-2\">\
                <div class=\"whitetext\"> Hello! </div>\
            </div>\
        </div>\
    </body>\
</html>";
*/
char file[10000];

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
	char *temp = malloc(fileinfo.st_size+1);
	read(fd,temp,fileinfo.st_size);
	temp[fileinfo.st_size] = '\0';
	close(fd);
	memcpy(file,temp,fileinfo.st_size);
	
	char *cur;
	struct html_element *html = NULL;
	struct html_element *elem = NULL;
	
	cur = strchr(file,'<');
	static_tolowern(cur+1,4);
	printf("%s\n",static_tolower_string);
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
		
		cur = strchr(cur,'>')+1;
	}
	
	while (elem != NULL && cur < file + strlen(file)) {
		printf("curr: %c%c%c%c\n",*cur,*(cur+1),*(cur+2),*(cur+3));
		if (*cur == '<') {
			if (*(cur+1) != '/') {
				/* new element */
				elem->children = realloc(elem->children, (1+elem->num_children) * sizeof(struct html_element *));
				elem->children[elem->num_children] = calloc(1,sizeof(struct html_element));
				
				init_html_element(elem->children[elem->num_children],elem,cur+1,strchr(cur+1,'>') - (cur+1));
				elem = elem->children[elem->num_children++];
				
				elem->innertext = malloc(32);
				elem->innertext[0] = '\0';
				elem->innertext_size = 32;
				elem->innertext_length = 0;
					
				printf("------------------------\n");
				printf("Element: Tag: (%d) %s\n",elem->tag,html_element_index_names[elem->tag]);
				printf("Properties: '%s'\n",elem->properties);
				printf("------------------------\n");
				
				
				cur = strchr(cur,'>')+1;
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
	
	//free(file);
	return 0;
}