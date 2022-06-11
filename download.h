#define IMPORTANT(x) (x == ELEMENT_INPUT || x == ELEMENT_TEXTAREA)

/* Vars */
extern char curl_strings[][16];

struct form_args_holder {
  char **args;
  size_t length;
};

char *curl(char *site, char **path, int method /* 1 = get, 0 = post*/, char **form_data);
struct form_args_holder *post_check(struct html_element * current, struct html_element * form ,struct form_args_holder *gen);
