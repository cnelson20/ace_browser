#include <stdlib.h>

/* Main root */
#define ELEMENT_HTML 0

/* Document metadata */
#define ELEMENT_BASE 1
#define ELEMENT_HEAD 2
#define ELEMENT_LINK 3
#define ELEMENT_META 4
#define ELEMENT_STYLE 5
#define ELEMENT_TITLE 6

/* Sectioning root */
#define ELEMENT_BODY 7

/* Content sectioning */
#define ELEMENT_ADDRESS 8
#define ELEMENT_ARTICLE 9
#define ELEMENT_ASIDE 10
#define ELEMENT_FOOTER 11
#define ELEMENT_HEADER 12
#define ELEMENT_H1 13
#define ELEMENT_H2 14
#define ELEMENT_H3 15
#define ELEMENT_H4 16
#define ELEMENT_H5 17
#define ELEMENT_H6 18
#define ELEMENT_MAIN 19
#define ELEMENT_NAV 20
#define ELEMENT_SECTION 21

/* Text content */
#define ELEMENT_BLOCKQUOTE 22
#define ELEMENT_DD 23
#define ELEMENT_DIV 24
#define ELEMENT_DL 25
#define ELEMENT_DT 26
#define ELEMENT_FIGCAPTION 27
#define ELEMENT_FIGURE 28
#define ELEMENT_HR 29
#define ELEMENT_LI 30
#define ELEMENT_OL 31
#define ELEMENT_P 32
#define ELEMENT_PRE 33
#define ELEMENT_UL 34

/* Inline text semantics */
#define ELEMENT_A 35
#define ELEMENT_ABBR 36
#define ELEMENT_B 37
#define ELEMENT_BDI 38
#define ELEMENT_BDO 39
#define ELEMENT_BR 40
#define ELEMENT_CITE 41
#define ELEMENT_CODE 42
#define ELEMENT_DATA 43
#define ELEMENT_DFN 44
#define ELEMENT_EM 45
#define ELEMENT_I 46
#define ELEMENT_KBD 47
#define ELEMENT_MARK 48
#define ELEMENT_Q 49
#define ELEMENT_RP 50
#define ELEMENT_RT 51
#define ELEMENT_RUBY 52
#define ELEMENT_S 53
#define ELEMENT_SAMP 54
#define ELEMENT_SMALL 55
#define ELEMENT_SPAN 56
#define ELEMENT_STRONG 57
#define ELEMENT_SUB 58
#define ELEMENT_SUP 59
#define ELEMENT_TIME 60
#define ELEMENT_U 61
#define ELEMENT_VAR 62
#define ELEMENT_WBR 63

/* Image and multimedia */
#define ELEMENT_AREA 64
#define ELEMENT_AUDIO 65
#define ELEMENT_IMG 66
#define ELEMENT_MAP 67
#define ELEMENT_TRACK 68
#define ELEMENT_VIDEO 69

/* Embedded content */
#define ELEMENT_EMBED 70
#define ELEMENT_IFRAME 71
#define ELEMENT_OBJECT 72
#define ELEMENT_PARAM 73
#define ELEMENT_PICTURE 74
#define ELEMENT_PORTAL 75
#define ELEMENT_SOURCE 76

/* SVG and MathML */
#define ELEMENT_SVG 77
#define ELEMENT_MATH 78

/* Scripting */
#define ELEMENT_CANVAS 79
#define ELEMENT_NOSCRIPT 80
#define ELEMENT_SCRIPT 81

/* Demarcating edits */
#define ELEMENT_DEL 82
#define ELEMENT_INS 83

/* Table content */
#define ELEMENT_CAPTION 84
#define ELEMENT_COL 85
#define ELEMENT_COLGROUP 86
#define ELEMENT_TABLE 87
#define ELEMENT_TBODY 88
#define ELEMENT_TD 89
#define ELEMENT_TFOOT 90
#define ELEMENT_TH 91
#define ELEMENT_THEAD 92
#define ELEMENT_TR 93

/* Forms */
#define ELEMENT_BUTTON 94 //IMPORTANT
#define ELEMENT_DATALIST 95
#define ELEMENT_FIELDSET 96
#define ELEMENT_FORM 97 //IMPORTANT
#define ELEMENT_INPUT 98 //IMPORTANT
#define ELEMENT_LABEL 99 
#define ELEMENT_LEGEND 100
#define ELEMENT_METER 101
#define ELEMENT_OPTGROUP 102
#define ELEMENT_OPTION 103
#define ELEMENT_OUTPUT 104
#define ELEMENT_PROGRESS 105
#define ELEMENT_SELECT 106 //IMPORTANT
#define ELEMENT_TEXTAREA 107 //IMPORTANT

/* Interactive elements */
#define ELEMENT_DETAILS 108
#define ELEMENT_DIALOG 109
#define ELEMENT_MENU 110
#define ELEMENT_SUMMARY 111

/* deprecated */
#define ELEMENT_NOFRAMES 112

/* macros, yanderedev style */
#define ISBLOCKLEVEL(x) (x == ELEMENT_ADDRESS || x == ELEMENT_ARTICLE || x == ELEMENT_ASIDE || x == ELEMENT_BLOCKQUOTE || x == ELEMENT_DETAILS || x == ELEMENT_DIALOG || x == ELEMENT_DD || x == ELEMENT_DIV || x == ELEMENT_DL || x == ELEMENT_DT || x == ELEMENT_FIELDSET || x == ELEMENT_FIGCAPTION || x == ELEMENT_FIGURE || x == ELEMENT_FOOTER || x == ELEMENT_FORM || x == ELEMENT_H1 || x == ELEMENT_H2 || x == ELEMENT_H3 || x == ELEMENT_H4 || x == ELEMENT_H5 || x == ELEMENT_H6 || x == ELEMENT_HEADER || x == ELEMENT_HR || x == ELEMENT_LI || x == ELEMENT_MAIN || x == ELEMENT_NAV || x == ELEMENT_OL || x == ELEMENT_P || x == ELEMENT_PRE || x == ELEMENT_TABLE || x == ELEMENT_UL)

#define ISINLINE(x) (x == ELEMENT_A || x == ELEMENT_ABBR || x == ELEMENT_B || x == ELEMENT_BDI || x == ELEMENT_BDO || x == ELEMENT_BIG || x == ELEMENT_BR || x == ELEMENT_BUTTON || x == ELEMENT_CANVAS || x == ELEMENT_CITE || x == ELEMENT_CODE || x == ELEMENT_DATA || x == ELEMENT_DATALIST || x == ELEMENT_DEL || x == ELEMENT_DFN || x == ELEMENT_EM || x == ELEMENT_EMBED || x == ELEMENT_I || x == ELEMENT_IFRAME || x == ELEMENT_IMG || x == ELEMENT_INPUT || x == ELEMENT_INS || x == ELEMENT_KBD || x == ELEMENT_LABEL || x == ELEMENT_MAP || x == ELEMENT_MARK || x == ELEMENT_METER || x == ELEMENT_NOSCRIPT || x == ELEMENT_OBJECT || x == ELEMENT_OUTPUT || x == ELEMENT_PICTURE || x == ELEMENT_PROGRESS || x == ELEMENT_Q || x == ELEMENT_RUBY || x == ELEMENT_S || x == ELEMENT_SAMP || x == ELEMENT_SCRIPT || x == ELEMENT_SELECT || x == ELEMENT_SLOT || x == ELEMENT_SMALL || x == ELEMENT_SPAN || x == ELEMENT_STRONG || x == ELEMENT_SUB || x == ELEMENT_SUP || x == ELEMENT_SVG || x == ELEMENT_TEMPLATE || x == ELEMENT_TEXTAREA || x == ELEMENT_TIME || x == ELEMENT_U || x == ELEMENT_TT || x == ELEMENT_VAR || x == ELEMENT_VIDEO || x == ELEMENT_WBR)

#define ISVOIDELEMENT(x) (x == ELEMENT_AREA || x == ELEMENT_BASE || x == ELEMENT_BR || x == ELEMENT_COL || x == ELEMENT_EMBED || x == ELEMENT_HR || x == ELEMENT_IMG || x == ELEMENT_INPUT || x == ELEMENT_LINK || x == ELEMENT_META || x == ELEMENT_PARAM || x == ELEMENT_SOURCE || x == ELEMENT_TRACK || x == ELEMENT_WBR)

#define DONTPRINT(x) (x ==  ELEMENT_HEAD || x == ELEMENT_SCRIPT || x == ELEMENT_STYLE)

#define DC1 0x11
#define DC2 0x12
#define DC3 0x13
#define DC4 0x14

struct html_element {
	struct html_element *parent;
	struct html_element **children;
	int num_children;
	
	int tag;
	char *innertext;
	int innertext_length;
	int innertext_size;
	char *properties;
	
	int lx,ly,rx,ry;
};

/* Functions */
int get_html_element_index(char *name);
int init_html_element(struct html_element *html, struct html_element *f_parent, char *def, size_t def_length);
void free_html_element(struct html_element *html);

char gen_console_attributes_char(struct html_element *html);

int min(int a, int b);
void *minpointer_nnull(void *a, void *b);
int is_whitespace_char(int c);

char static_tolower_string[100];
char *static_tolowern(char *string, size_t n);
char *static_tolower(char *string);

unsigned char *geninquotes_html(char *html, size_t len);

void print_element_path_nonewline(struct html_element *html);
void print_element_path(struct html_element *html);
void test_print_structure(struct html_element *html);
void print_html_structure(struct html_element *html, unsigned char rec);

char *render_page(struct html_element *html, struct html_element *body);
char *render_html_file(char *filename, char *output_filename);

/* Global Variables */
extern char *file;
extern char *output;
extern char *output_temp;
extern size_t output_size;

extern struct html_element *html;
