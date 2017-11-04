#ifndef __JNOS_HTTPD_H__
#define __JNOS_HTTPD_H__


enum HTTP_METHOD {
	HTTP_METHOD_GET,	//http 0.9
	HTTP_METHOD_HEAD,
	HTTP_METHOD_POST,	//http 1.0
	HTTP_METHOD_OPTIONS,
	HTTP_METHOD_PUT,
	HTTP_METHOD_DELETE,
	HTTP_METHOD_TRACE,
	HTTP_METHOD_CONNECT	//http 1.1
};

enum MIME_TYPE {
	MIME_TYPE_HTML,
	MIME_TYPE_JAVASCRIPT,
	MIME_TYPE_CSS,
	MIME_TYPE_JSON,
	MIME_TYPE_JPEG,
	MIME_TYPE_GIF,
	MIME_TYPE_PNG
};


enum HTTP_KEY_VALUE_KIND{
	HTTP_RESPONSE_HEADER_KIND	=0,
	HTTP_HEADER_KIND		=1,
	HTTP_COOKIE_KIND		=2,
	HTTP_POSTDATA_KIND		=4,
	HTTP_QUERYSTRING_KIND		=8,
	HTTP_FOOTER_KIND		=16
};

struct httpd_conf{
	int port;
	unsigned long ipv4;
	char *homedir;
	int maxconn;
};

struct key_value_node{
	struct key_value_node *next;
	char *keyname;
	char *value;
};


struct http_header{
	struct http_header *next;
	char *header;
	char *value;
	enum HTTP_KEY_VALUE_KIND kind;
};

struct httpd_conn{
	int socket;
	unsigned long ipv4;
	


};

typedef int (*HttpdAccessHandle)(struct connection *conn);

struct http_handler{
	char * url;
	HttpdAccessHandle handle;
};




struct httpd {
	int socket;
	int shutdown;
	int inited;
	int handlers_count;
	int handlers_size;
	struct http_handler *handlers;

	struct httpd_conf *conf;
	struct connection **conns;
	int conn_sum;
};

#if 0
struct connection {
	int state;
	int fd;
	time_t last_activity;
	unsigned int timeout;

	char *method;
	char *version;
	char *url;	



};
#endif



//hconf can't free before destory_httpd
struct httpd *create_httpd(struct httpd_conf *hconf);
int destory_httpd(struct httpd* phttpd);
//const char *lookup_connection(struct connection *conn, enum HTTPD_VALUE_KIND, const char *key); 
int httpd_add_handle(struct connection *conn, const char *url, HttpdAccessHandle handle);
int httpd_find_handler(struct connection *conn);
int httpd_process_handler(struct connection *conn);
#endif
