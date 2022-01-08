#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

int connect_site(char *site, char *port);
void get(int sd, char *path, char *filepath, char **extraheaders);
int main(int argc, char *argv[]);

int connect_site(char *site, char *port) {
	struct addrinfo *hints, *results;
	int sd;
	
	hints = calloc(1,sizeof(struct addrinfo));
	hints->ai_family = AF_INET;
	hints->ai_socktype = SOCK_STREAM;
	
	getaddrinfo(site,port,hints,&results);
	if (!results) {
		printf("Couldn't resolve host.\n");
		free(hints);
		return -1;
	}
	sd = socket(results->ai_family,results->ai_socktype,results->ai_protocol);
	if (sd == -1) {
		printf("Couldn't create socket.\n");
		free(hints);
		//freeaddrinfo(results);
		return -1;
	}
	
	if (connect(sd,results->ai_addr,results->ai_addrlen) == -1) {
		printf("Couldn't connect to server\n");
		free(hints);
		freeaddrinfo(results);
		return -1;
	}
	
	free(hints);
	freeaddrinfo(results);
	return sd;	
}

char response[2000000];
char request[1024];
char **headers;
int headers_len;
char *html;

void get(int sd, char *path, char *filepath, char **request_headers) { 
	strcpy(request,"GET ");
	strncat(request,path,sizeof(request) - strlen(request) - 1);
	strncat(request," HTTP/1.1\r\n",sizeof(request) - strlen(request) - 1);
	
	while(*request_headers) {
		strncat(request,*request_headers,sizeof(request) - strlen(request) - 1);
		strncat(request,"\r\n",sizeof(request) - strlen(request) - 1);
		request_headers++;
	}
	
	strncat(request,"\r\n",sizeof(request) - strlen(request) - 1);
	
	printf("Request:\n'%s'\n",request);
	
	write(sd,request,strlen(request));
	read(sd,response,sizeof(response));
	while (!strstr(response,"\r\n\r\n")) {
		read(sd,response+strlen(response),sizeof(response)-strlen(response));
	}

	html = response;
	headers_len = 0;
	while (*html != '\r') {
		html = strchr(html,'\n')+1;
		headers_len++;
	}
	html = response;
	headers = malloc(sizeof(char *) * headers_len);
	int i;
	char *currline;
	for (i = 0; i < headers_len; i++) {
		currline = strsep(&html,"\n");
		*(html - 2) = '\0';
		headers[i] = malloc(strlen(currline)+1);
		strcpy(headers[i],currline);
	}
	html = strchr(currline,'\0') + 4;

	printf("Response Headers:\n");
	for (i = 0; i < headers_len; i++) {
		printf("\t%s\n",headers[i]);
	}
	
	int chunk_length = -1;
	int content_length = -1;
	int chunked = 0;
	int content_length_undefined = 1;
	for (i = 0; i < headers_len; i++) {
		if (!chunked && strstr(headers[i],"Transfer-Encoding") && strstr(headers[i],"chunked")) {
			chunked = 1;
		}
		if (content_length_undefined && strstr(headers[i],"Content-Length")) {
			sscanf(strchr(headers[i],' ')+1,"%d",&content_length);
			content_length_undefined = 0;
		}
	}
	if (chunked || content_length_undefined) {
		chunked = 1; // If content_length_undefined
		char *chtml = html;
		while (1) {
			*strchr(chtml,'\r') = '\0';
			printf("c: '%s'\n",chtml);
			sscanf(chtml,"%x",&chunk_length);
			strcpy(chtml,strchr(chtml,'\0')+2);
		
			printf("________________\n");
			printf("Chunk Length: %d bytes\n",chunk_length);
			if (chunk_length == 0) {
				break;
			}		
		
			int bytes_read = strlen(chtml);
			while (bytes_read < chunk_length) {
				bytes_read += read(sd,html+strlen(html),chunk_length - bytes_read);
			}
			
			printf("________________\n");
			printf("%s\n",chtml);
			
			chtml += chunk_length;
			read(sd,chtml,sizeof(response) - strlen(html));
			strcpy(chtml,chtml+2);
			//break;
		}
	} else if (content_length != -1) {
		printf("________________\n");
		printf("Content Length: %d bytes\n",content_length);
		
		int bytes_read = strlen(html);
		while (bytes_read < content_length) {
			bytes_read += read(sd,html+bytes_read,content_length - bytes_read);
		}
		
		//printf("________________\n");
		//printf("%s\n",html);
	}
	
	int tfd = open(filepath,O_WRONLY | O_CREAT | O_TRUNC, 0644);
	write(tfd,html,strlen(html));
	close(tfd);
		
	for (i = 0; i < headers_len; i++) {
		free(headers[i]);
	}
	free(headers);
}

int main(int argc, char *argv[]) {
	
	char *site, *path, *port;
	
	if (argc < 2) {
			printf("Usage: ./a site/path:port savefile");
			return 1;
	}
	if (strchr(argv[1],'/')) {
		site = malloc(strlen(argv[1])+1);
		strcpy(site,argv[1]);
		*strchr(site,'/') = '\0';
		site = realloc(site,strlen(site)+1);
		if (strchr(argv[1],':')) {
			*strchr(site,':') = '\0';
			port = malloc(strlen(strchr(site,':'))+1);
			strcpy(port,strchr(site,':'));
			site = realloc(site,strlen(path)+1);
		} else {
			port = malloc(sizeof("http"));
			strcpy(port,"http");
		}
		path = malloc(strlen(strchr(argv[1],'/'))+1);
		strcpy(path,strchr(argv[1],'/'));
	} else {
		site = malloc(strlen(argv[1])+1);
		strcpy(site,argv[1]);
		if (strchr(argv[1],':')) {
			*strchr(site,':') = '\0';
			site = realloc(site,strlen(site)+1);
			port = malloc(strlen(strchr(argv[1],':'))+1);
			strcpy(port,strchr(argv[1],':')+1);
		} else { 			
			port = malloc(sizeof("http"));
			strcpy(port,"http");
		}
		path = malloc(2);
		strcpy(path,"/");
	}
	
	printf("site: '%s'\npath: '%s'\nport: '%s'\n",site,path,port);
	
	int sd = connect_site(site,port);
	if (sd == -1) {
		return 1;
	}
	
	char **request_headers = malloc(2*sizeof(char *));
	request_headers[0] = malloc(100);
	strncat(request_headers[0],"Host: ",100 - strlen(request) - 1);
	strncat(request_headers[0],site,100 - strlen(request) - 1);
	strncat(request_headers[0],"\r\n",100 - strlen(request) - 1);
	request_headers[1] = NULL;
	
	get(sd, path, argc >= 3 ? argv[2] : "/tmp/page.html",request_headers);
	
	int i;
	for (i = 0; request_headers[i]; i++) {
		free(request_headers[i]);
	}
	free(request_headers);
	close(sd);
	free(site);
	free(path);
}