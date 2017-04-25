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

struct httpd_conf{
	int port;
	unsigned long ipv4;
	char *homedir;
	int maxconn;
};

struct httpd_conn{
	int socket;
	unsigned long ipv4;
	


};


struct httpd {
	int socket;
	int shutdown;
	int inited;

	struct httpd_conf *conf;
	struct connection *conns;
	int conn_sum;
};

struct connection {
	int state;
	int fd;
	time_t last_activity;
	unsigned int timeout;

	char *method;
	char *version;
	char *url;	



};

//hconf can't free before destory_httpd
struct httpd *create_httpd(struct httpd_conf *hconf);
int destory_httpd(struct httpd* phttpd);

#endif
