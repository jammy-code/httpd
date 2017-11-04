#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include "httpd.h"

enum HTTP_STATE {
	STATE_OK	= 200,
	STATE_REQERR 	= 400,
	STATE_NOTFOUND	= 404

};
enum HTTP_CONNECTION_STATE {
	CONNECTION_INIT = 0,
	CONNECTION_URL_RECEIVED,
	CONNECTION_HEADER_RECEIVED,
	CONNECTION_HEADER_PROCEIVED,




};

struct connection {
	int state;
	struct httpd *server;
	
	int socket_fd;
	struct key_value_node *query_arguments;
	int query_count;	
	struct http_header *headers_received;
	enum HTTP_METHOD method;
	char *url;
	char *filepath;
	int handler_index;
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
	struct key_value_node *post_arguments;
	int content_length;
	int content_type;
	char *content;

	int (*cgihandle)(struct connection *conn);
};


const char *getStatusDesc(int code);
int parseHeader(struct connection *conn);
int getContentLenght(struct connection *conn);

char *lookup_querystring(struct connection *conn, const char *name);

#endif

