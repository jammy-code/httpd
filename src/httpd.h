#ifndef __JNOS_HTTPD_H__
#define __JNOS_HTTPD_H__


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
