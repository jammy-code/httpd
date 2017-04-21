#ifndef __CONNECTION_H__
#define __CONNECTION_H__

struct connection {
	int status;
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
};


const char *getStatusDesc(int code);


#endif

