#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>

#include "browse.h"
#include "download.h"

char curl_strings[][16] = {
	/* 0 */ "curl", 
	/* 1 */ "-d", 
	/* 2 */ "--get", 
	/* 3 */ "-o", 
	/* 4 */ "--cookie-jar", 
	/* 5 */ "--cookie", 
	/* 6 */"--create-dirs", 
	/* 7 */"cookies.txt", 
	/* 8 */ "-s"
	};

char *curl(char *site, char *path, int method /* 1 = get, 0 = post*/, char **form_data) {
	char *fullname = malloc(strlen(site) + strlen(path) + 1);
	strcpy(fullname,site);
	strcat(fullname,path);

	char *filename_tosave;
	if (strrchr(path,'/') == strchr(path,'\0') - 1) {
		filename_tosave = malloc(strlen("files") + strlen(path) + strlen("index.html") + 1);
		strcpy(filename_tosave,"files");
		strcat(filename_tosave,path);
		strcat(filename_tosave,"index.html");
	} else {
		filename_tosave = malloc(strlen("files") + strlen(path) + 1);
		strcpy(filename_tosave,"files");
		strcat(filename_tosave,path);
	}

	printf("filename_tosave: '%s'\n",filename_tosave);
	
	char **toexec = malloc(11*sizeof(char *));
	toexec[0] = curl_strings[0]; // curl
	toexec[1] = fullname; // site.com/file.html
	toexec[2] = curl_strings[3]; // -o
	toexec[3] = filename_tosave; // file.html
	toexec[4] = curl_strings[6];
	toexec[5] = curl_strings[4];
	toexec[6] = curl_strings[7];
	toexec[7] = curl_strings[5];
	toexec[8] = curl_strings[7];
	toexec[9] = curl_strings[8];
	toexec[10] = NULL;
	if (form_data != NULL) {
		int i, j;
		j = 11;
		if (method) {
			toexec = realloc(toexec,(j+1)*sizeof(char *));
			toexec[j-1] = curl_strings[2];
		}
		for (i = 0; form_data[i]; i++) {
			toexec = realloc(toexec,(j+2)*sizeof(char *));
			toexec[j-1] = curl_strings[1];
			toexec[j] = form_data[i];
			j += 2;
		}
		toexec[j+1-2] = NULL;
	}
  
	int i;
	for (i = 0; toexec[i]; i++) {
		printf("[%d]: '%s'\n",i,toexec[i]);
	}
	// Seg fault in for loop 
  
	if (fork()) {
		int childexitstatus;
		wait(&childexitstatus);
	} else {
		mkdir("files/", 0755);
		execvp(toexec[0],toexec);
	}
  
	free(fullname);
	//free(filename_tosave);
	free(toexec);
  
	return filename_tosave;
}

struct form_args_holder *post_check(struct html_element * current, struct html_element * form ,struct form_args_holder *gen) {
	if(current == form) {
		if (current->num_children != 0 && current->tag == ELEMENT_FORM) {
			int i;		
			for (i = 0; i < current->num_children; i++) {
 			    post_check(current->children[i], form, gen);
			}
		}
	} else {
		if (current->num_children != 0) {
			int i;		
			for (i = 0; i < current->num_children; i++) {
 			   post_check(current->children[i], form, gen);
			}
		}
		if(IMPORTANT(current->tag)) {
			gen->args=realloc(gen->args,sizeof(char*) * (gen->length + 2));
			//copy name and value to gen->args[gen->length] 
			//for name
			char* tempstring = malloc(512);
			strncpy(tempstring, strstr(gen->properties,"name=")+5, 512);
			char thingy = tempstring[0];
			char * secondquote = strchr(tempstring+1, thingy);
			int le = secondquote-tempstring+1;
			char* inputstring;
			memcpy(inputstring, tempstring, le);
			inputstring[le]='=';
			inputstring[le+1]='\0';
			//for value
			char* temp2string = malloc(512);
			strncpy(temp2string, strstr(gen->innertext, "value=")+6,512);
			char thing2y = temp2string[0];
			char second2quote = strchr(temp2string+1, thing2y);
			int le2 = second2quote - temp2string+1;
			char* input2string;
			memcpy(input2string, temp2string, le2);
			intput2string[le2] = '\0';
			strcat(intputstring, intput2string);
			// end 
			gen->length++;
			gen->args[gen->length] = NULL;
		}
	}
	return NULL;
}
