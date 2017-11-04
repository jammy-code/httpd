#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "httpd.h"
#include "connection.h"

static struct httpd_conf hconf;



void show_usage()
{
	printf("\nhttpd -d homedir [ -i interface ] [ -b addr ] [ -p port ] [ -h ]\n\n");
}


void proc_opt(argc, argv)
{
	int ch;  
	opterr = 0;  
	while ((ch = getopt(argc,argv,"i:b:p:d:h"))!=-1)  
	{  
		switch(ch) {
		case 'i':
			break;
		case 'p':
			hconf.port = atoi(optarg);
			if (hconf.port == 0){
				hconf.port = htons(80);
			}
			break;
		case 'b':
			hconf.ipv4 = inet_addr(optarg);
			break;
		case 'd':
			hconf.homedir = strdup(optarg);
			break;
		default:
			show_usage();
			exit(0);
		}
	}
}

int system_handle(struct connection *conn)
{
	int state = 500;
printf("%s %d: %s()\n", __FILE__, __LINE__, __func__);
	return state;
}

int account_handle(struct connection *conn)
{
	int state = 500;
	char *action, *uid, *pwd, *session;
	char *content;
	int size, len;
	if (NULL == conn) return state;

	content = malloc(512);
	if (content){
		len = 0;
		size = 512;
		snprintf(content, 512, "{result:0}");
		len = strlen(content);
		conn->content_lenght = len;
		conn->content = content;
		state = 200;
	}
printf("%s %d: %s()\n", __FILE__, __LINE__, __func__);
	action = lookup_querystring(conn, "action");
printf("%s %d: %s()\n", __FILE__, __LINE__, __func__);
	if (NULL==action) return state;

printf("%s %d: %s()\n", __FILE__, __LINE__, __func__);
	if (0 == strcmp(action, "login")){
	//login
	//	if (conn->method == HTTP_METHOD_POST){
		snprintf(content, 512, "{result:%s}", action);
		len = strlen(content);
		conn->content_lenght = len;
		conn->content = content;
		state = 200;
	//	}
	}

	return state;
}

int network_handle(struct connection *conn)
{
	int state = 500;
printf("%s %d: %s()\n", __FILE__, __LINE__, __func__);
	return state;
}


int main(int argc, char **argv)
{
	struct httpd *phttpd;
	
	memset(&hconf, 0, sizeof(hconf));
	hconf.ipv4 = htonl(INADDR_ANY);
	hconf.port = htons(80);
	hconf.maxconn = 8;
	
	if (argc > 1)
		proc_opt(argc, argv);
	
	if (hconf.homedir == NULL)
		hconf.homedir = strdup("/usr/www-root");

	phttpd = create_httpd(&hconf);
	printf("%s %d\n", __func__, __LINE__);
	httpd_add_handler(phttpd, "system.cgi", &system_handle);
	httpd_add_handler(phttpd, "account.cgi", &account_handle);
	httpd_add_handler(phttpd, "network.cgi", &network_handle);
	httpd_run(phttpd);
	printf("%s %d end\n", __func__, __LINE__);
}

