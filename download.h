struct form_args_holder {
  char **args;
  size_t length;
}

int curl(char *site, char *path, int method /* 1 = get, 0 = post*/, char **form_data);
char** post_check(struct html_element * current, struct html_element * form);
