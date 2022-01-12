#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

char curl_strings[][16] = {/* 0 */ "curl", /* 1 */ "-d", /* 2 */ "--get", /* 3 */ "-o", /* 4 */ "--cookie-jar", /* 5 */ "--cookie", /* 6 */"--create-dirs", /* 7 */"cookies.txt"};

int curl(char *site, char *path, int method /* 1 = get, 0 = post*/) {
  char *fullname = malloc(strlen(site) + strlen(path) + 1);
  strcpy(fullname,site);
  strcat(fullname,path);

  char *filename_tosave;
  if (strchr(path,'/') == strchr(path,'\0') - 1) {
	filename_tosave = malloc(strlen("/tmp/ace") + strlen(path) + strlen("index.html") + 1);
	strcpy(filename_tosave,"/tmp/ace");
	strcat(filename_tosave,path);
	strcat(filename_tosave,"index.html");
  } else {
	filename_tosave = malloc(strlen("/tmp/ace") + strlen(path) + 1);
	strcpy(filename_tosave,"/tmp/ace");
	strcat(filename_tosave,path);
  }

  printf("filename_tosave: '%s'\n",filename_tosave);
  
  char **toexec = malloc(10*sizeof(char *));
  toexec[0] = curl_strings[0]; // curl
  toexec[1] = fullname; // site.com/file.html
  toexec[2] = curl_strings[3]; // -o
  toexec[3] = strchr(path,'/')+1; // file.html
  toexec[4] = curl_strings[6];
  toexec[5] = curl_strings[4];
  toexec[6] = curl_strings[7];
  toexec[7] = curl_strings[5];
  toexec[8] = curl_strings[7];
  toexec[9] = NULL;
  
  int i;
  for (i = 0; toexec[i]; i++) {
	printf("[%d]: '%s'\n",i,toexec[i]);
  }
  // Seg fault in for loop 
  
  free(fullname);
  free(filename_tosave);
  free(toexec);
  
  return 0;
}

int main(int argc, char *argv[]) {
  curl(argv[1],argv[2],1);
}
