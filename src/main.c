#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "httpd.h"

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


int main(int argc, char **argv)
{
	struct httpd *phttpd;
	
	memset(&hconf, 0, sizeof(hconf));
	hconf.ipv4 = htonl(INADDR_ANY);
	hconf.port = htons(80);
	hconf.maxconn = 8;
	
	if (argc > 1)
		proc_opt(argc, argv);

	phttpd = create_httpd(&hconf);
	printf("%s %d\n", __func__, __LINE__);
	httpd_run(phttpd);
	printf("%s %d end\n", __func__, __LINE__);
}

