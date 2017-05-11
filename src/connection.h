#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include "httpd.h"

enum HTTP_STATE {
	STATE_OK	= 200,
	STATE_REQERR 	= 400,
	STATE_NOTFOUND	= 404

};

struct connection {
	int state;
	struct httpd *server;
	
	int socket_fd;	

	enum HTTP_METHOD method;
	char *url;
	char *filepath;
	int iscgi;
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
	int (*cgihandle)(struct connection *conn);
};


const char *getStatusDesc(int code);
int parseHeader(struct connection *conn);
int getContentLenght(struct connection *conn);


#endif

