#ifndef __CONNECTION_H__
#define __CONNECTION_H__

include “httpd.h”

struct connection {
	int status;
	struct httpd *server;
	
	int socket_fd;	

	char *hdr_accept;//text/plain
	char *hdr_acpt_charset;	//utf-8
	char *hdr_encoding;	//gzip, deflate
	char *hdr_language;	//en-US
	char *hdr_datetime;
	char *hdr_authorization;
	char *hdr_cache_ctrl;	//no-cache
	char *hdr_connection;	//keep-alive, upgrade
	char *hdr_cookie;
	char *hdr_content_type;	//application/x-www-form-urlencoded
	int hdr_content_len;
	unsigned char *postdata;
};


const char *getStatusDesc(int code);
int parseHeader(struct connection *conn);
int getContentLenght(struct connection *conn);


#endif

