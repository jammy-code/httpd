#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "httpd.h"
#include "connection.h"
#include "httpheader.h"

#define DEFAULT_FILE "index.html"


struct connection * add_connection(struct httpd *phttpd, int socket_fd)
{
	struct connection *conn;

	conn = (struct connection *)malloc(sizeof(struct connection));
	if (conn){
		int i;
		printf("%s %d: %s() %08x fd=%d\n", __FILE__, __LINE__, __func__, conn, socket_fd);
		memset(conn, 0, sizeof(struct connection));
		conn->socket_fd = socket_fd;
		conn->server = phttpd;
		if (phttpd->conn_sum == 0){
			phttpd->conns[0] = conn;
			phttpd->conn_sum = 1;
		printf("%s %d: %s() index=0\n", __FILE__, __LINE__, __func__);
			return conn;
		}
		for (i=phttpd->conn_sum-1; i>=0; i--){
			if (conn->socket_fd < phttpd->conns[i]->socket_fd){
				phttpd->conns[i+1]=phttpd->conns[i];
				if (i==0){
					phttpd->conns[i]= conn;
		printf("%s %d: %s() index=%d\n", __FILE__, __LINE__, __func__, i);
				}
			}
			else {
				phttpd->conns[i+1]= conn;
		printf("%s %d: %s() index=%d\n", __FILE__, __LINE__, __func__, i+1);
				break;
			}
		}
		phttpd->conn_sum += 1;
	}
	return conn;
}
void free_connection(struct connection *conn)
{
	close(conn->socket_fd);
	if (conn->url) free(conn->url);
	if (conn->filepath) free(conn->filepath);

	if (conn->content) free(conn->content);
	/*char *hdr_accept;//text/plain
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
	*/
	free(conn);
}

void del_connfd(struct httpd *phttpd, int socket_fd)
{
	int i, j;
	for (i=phttpd->conn_sum-1; i>=0; i--){
		if (phttpd->conns[i]->socket_fd == socket_fd){
printf("%s %d: %s() del connect idx=%d fd=%d\n", __FILE__, __LINE__, __func__, i, socket_fd);
			close(socket_fd);
			free_connection(phttpd->conns[i]);
			for (j=i+1; j<=phttpd->conn_sum-1; j++,i++){
				phttpd->conns[i]= phttpd->conns[j];
				 phttpd->conns[j] = NULL;
			}
			phttpd->conn_sum--;
			break;
		}
	}
}

struct connection* get_connection(struct httpd *phttpd, int socket)
{
	//使用二分法查找
	int low=0;
	int high=phttpd->conn_sum-1;
	while(low<=high){
		int mid=low+((high-low)>>1);
//printf("%s %d: %s()low=%d mid=%d high=%d fd=%d\n", __FILE__, __LINE__, __func__, low, mid, high, phttpd->conns[mid]->socket_fd);
		if (socket == phttpd->conns[mid]->socket_fd){
//printf("%s %d: %s() found %d at d!\n", __FILE__, __LINE__, __func__, socket, mid);
			return phttpd->conns[mid];
		}
		else if (socket < phttpd->conns[mid]->socket_fd){
			high = mid - 1;
		}
		else {
			low = mid + 1;
		}
	}
	return NULL;
}
void clear_request(struct connection* conn)
{
	if (!conn) return;
	
	if (conn->filepath){
		free(conn->filepath);
		conn->filepath = NULL;
	}
	if (conn->url){
		free(conn->url);
		conn->url = NULL;
	}
	if (conn->url){
		free(conn->url);
		conn->url = NULL;
	}
}

void del_key_value(struct key_value_node *node)
{
	struct key_value_node *next;

	while (node){
		if (node->keyname) free(node->keyname);
		if (node->value) free(node->value);
		next = node->next;
		free(node);
		node = next;
	}
}

int add_key_value(struct key_value_node **pheader, const char *name, const char *value)
{
	int state = -1;

	if (name && *name){
		struct key_value_node *node = (struct key_value_node *)malloc(sizeof(struct key_value_node));
		if (node){
			memset(node, 0, sizeof(struct key_value_node));
			node->keyname = strdup(name);
			node->value = strdup(value);
			if (NULL != *pheader){
				struct key_value_node *prev = *pheader;
				while (prev->next) prev=prev->next;
				prev->next = node;
				state = 0;
			}
			else {
				*pheader = node;
				state = 0;
			}
		} 
	}
	return state;
}

int parse_querystring(struct connection *conn, const char *buff, int len)
{
	int state = -1;
	char *args = buff;
	if (NULL == conn || NULL == buff) return state;
	
	while (*args != '\0' && args < buff+len){
		char *equ = strchr(args, '=');
		char *amp = strchr(args, '&');
		if (amp != NULL){
			*amp = '\0';
			amp++;
		}
		*equ = '\0';
		equ++;
		if(0 == add_key_value(&conn->query_arguments, args, equ)){
			conn->query_count++;
		}
		if (amp){
			args = amp;
			continue;
		}
		break;	
	}
}

int parse_header(struct connection* conn, const char *buff, int len)
{
	int state = 0;
	char value[512]; 
	char *p = buff;
	int pos = 0;
	
	while(*p > ' ' && pos<sizeof(value)-1 && p<buff+len){
		value[pos] = *p;
		p++;
		pos++;
	}
	value[pos] = '\0';
printf("%s %d: %s() method: %s\n", __FILE__, __LINE__, __func__, value);
	if (strcmp(value, HTTP_METHOD_GET_STR)==0){
		conn->method = HTTP_METHOD_GET;
	}
	else if (strcmp(value, HTTP_METHOD_POST_STR)==0){
		conn->method = HTTP_METHOD_POST;
	}
	else if (strcmp(value, HTTP_METHOD_HEAD_STR)==0){
		conn->method = HTTP_METHOD_HEAD;
	}
	else {
		conn->state = STATE_REQERR;
		return conn->state;
	}
	
	while(*p <=' ' && p <buff+len) p++;
	pos = 0;
	while (*p > ' ' && *p != '?' && pos<sizeof(value)-1 && p<buff+len){
		value[pos] = *p;
		p++;
		pos++;
	}
	value[pos] = '\0';
printf("%s %d: %s() url: %s\n", __FILE__, __LINE__, __func__, value);
	conn->url = strdup(value);
	
	if (*p == '?'){
		p++;
		pos=0;
		while (*p > ' ' && pos<sizeof(value)-1 && p<buff+len){
			value[pos] = *p;
			p++;
			pos++;
		}
		value[pos] = '\0';
		parse_querystring(conn, value, pos);
	}
	
	while(*p <=' ' && p <buff+len) p++;
	if (strncmp(p, HTTP_VERSION_1_0_STR, strlen(HTTP_VERSION_1_0_STR))==0){
		p += strlen(HTTP_VERSION_1_0_STR);
	}
	if (strncmp(p, HTTP_VERSION_1_1_STR, strlen(HTTP_VERSION_1_1_STR))==0){
		p += strlen(HTTP_VERSION_1_1_STR);
	}
	p+=2; //\r\n
	//parse other...
	
	return state;	
}

int check_responder(struct connection* conn)
{
	int state = 0;
	char *url = conn->url;
	char path[512];
	
	if (conn->state)
		return conn->state;
	
	
printf("%s %d: %s()\n", __FILE__, __LINE__, __func__);
	path[sizeof(path)-1] = '\0';
	strncpy(path, conn->server->conf->homedir, sizeof(path)-1);
	strncat(path, url, sizeof(path)-strlen(path)-1);
	if (url[strlen(url)-1] == '/'){
		strncat(path, DEFAULT_FILE, sizeof(path)-strlen(path)-1);
	}
	if (0 == access(path, R_OK)){
		conn->filepath = strdup(path);
	}
	else if (0 != httpd_find_handler(conn)){
		conn->state = STATE_NOTFOUND;
	}
printf("%s %d: %s return", __FILE__, __LINE__, __func__);
	return state;
}

const char *lookup_connection(struct connection *conn, enum HTTP_KEY_VALUE_KIND kind, const char *key)
{
	struct http_header *pos;

	if (NULL == conn)
		return NULL;

	for (pos = conn->headers_received; NULL != pos; pos=pos->next){
		if ((0 != (pos->kind & kind)) && strcasecmp(pos->header, key)==0)
			return pos->value;

	}

}


 
char *lookup_querystring(struct connection *conn, const char *name)
{
	char *value = NULL;
	int i;
	struct key_value_node *node;

	if (conn==NULL || name==NULL || *name == '\0') return value;

	node = conn->query_arguments;
	while (node){
		if (0 == strcmp(node->keyname, name)){
			value = node->value;
			break;
		}
		node = node->next;

	}
	return value;
}
