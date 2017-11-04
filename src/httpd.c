#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <netdb.h>

#include <errno.h>

#include "httpd.h"
#include "connection.h"
#define MAX_EVENT 64


#define SERVER_STRING "jhttpd/0.1.0"

static const char *mimeTypes[]={
			"text/html",
			"text/javascript",
			"text/css",
			"application/json",
			"image/jpeg",
			"image/gif",
			"image/png",
			NULL};

void error_die(const char *sc)
{
	perror(sc);
	exit(1);
}


struct httpd *create_httpd(struct httpd_conf *hconf)
{
	struct sockaddr_in name;
	struct httpd *phttpd = calloc(1, sizeof(struct httpd));
	
	phttpd->socket = socket(PF_INET, SOCK_STREAM, 0);
	if (phttpd->socket == -1)
		error_die("socket");
	 memset(&name, 0, sizeof(name));
	 name.sin_family = AF_INET;
	 name.sin_port = hconf->port;
	 name.sin_addr.s_addr = hconf->ipv4;
	if (bind(phttpd->socket, (struct sockaddr *)&name, sizeof(name)) < 0)
		error_die("bind");
	if (listen(phttpd->socket, hconf->maxconn) < 0)
		error_die("listen");
	
	phttpd->conf = 	hconf;
	if (hconf->maxconn){
		if (hconf->maxconn > 65536 || hconf->maxconn < 8)
			hconf->maxconn = 128;
		phttpd->conns = malloc(sizeof(struct connection*)*hconf->maxconn);
	}
	else {
		hconf->maxconn = 128;
		phttpd->conns = malloc(sizeof(struct connection*) * 128);
	}
	if (phttpd->conns){
		memset(phttpd->conns, 0, sizeof(struct connection*) * 128);
	}
 	return phttpd;
}

void bad_request(int client)
{
	char buf[256];

	sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
	send(client, buf, sizeof(buf), 0);
	sprintf(buf, "Server: %s\r\n", SERVER_STRING);
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-type: text/html\r\n\r\n");
	send(client, buf, sizeof(buf), 0);
	sprintf(buf, "<P>Your browser sent a bad request, such as a POST without a Content-Length.</P>\r\n");
	send(client, buf, sizeof(buf), 0);
}

void unimplemented(int client)
{
	 char buf[1024];

	sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Server: %s\r\n", SERVER_STRING);
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "</TITLE></HEAD>\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "</BODY></HTML>\r\n");
	send(client, buf, strlen(buf), 0);
}

void not_found(int client)
{
	char buf[256];

	sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Server: %s\r\n", SERVER_STRING);
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<HTML><TITLE>Not Found</TITLE><BODY><P>The server could not fulfill your request because the resource specified is unavailable or nonexistent.</P></BODY></HTML>\r\n");
	send(client, buf, strlen(buf), 0);
}

void headers(int client, const char *filename)
{
	char buf[512];
	struct stat sb;
	//(void)filename;  /* could use filename to determine file type */
	stat(filename, &sb);
	strcpy(buf, "HTTP/1.1 200 OK\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Server: %s\r\n", SERVER_STRING);
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-Lenght: %d\r\n", sb.st_size);
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n\r\n");//case file type
	send(client, buf, strlen(buf), 0);
}

//read & discard headers before send file
//buf[0] = 'A'; buf[1] = '\0';
//while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
//numchars = get_line(client, buf, sizeof(buf));
void send_file(int client, const char *filename)
{
	FILE *fp = NULL;
	int numchars = 1;
	char buf[1024];

printf("%s %d: %s() send %s\n", __FILE__, __LINE__, __func__, filename);
	fp = fopen(filename, "r");
	if (fp) {
		headers(client, filename);
		fgets(buf, sizeof(buf), fp);
		while (!feof(fp)){
			send(client, buf, strlen(buf), 0);
			fgets(buf, sizeof(buf), fp);
		}
		fclose(fp);
	}
	else {
		not_found(client);
	}
}


//level trigger
/*void epool_triger(struct epoll_event* events, int number, int epollfd, int listenfd )  
{  
    char buf[ BUFFER_SIZE ];  
    for ( int i = 0; i < number; i++ )  
    {  
        int sockfd = events[i].data.fd;  
        if ( sockfd == listenfd )  
        {  
            struct sockaddr_in client_address;  
            socklen_t client_addrlength = sizeof( client_address );  
            int connfd = accept( listenfd, ( struct sockaddr* )&client_address, &client_addrlength );  
            addfd( epollfd, connfd, false );  
        }  
        else if ( events[i].events & EPOLLIN )  
        {  
            printf( "event trigger once\n" );  
            memset( buf, '\0', BUFFER_SIZE );  
            int ret = recv( sockfd, buf, BUFFER_SIZE-1, 0 );  
            if( ret <= 0 )  
            {  
                close( sockfd );  
                continue;  
            }  
            printf( "get %d bytes of content: %s\n", ret, buf );  
        }  
        else  
        {  
            printf( "something else happened \n" );  
        }  
    }  
}*/




int get_line(int sock, char *buf, int size)
{
	int i = 0;
	char c = '\0';
	int n;

	while ((i < size - 1) && (c != '\n')){
		n = recv(sock, &c, 1, 0);
		if (n == 1){
			if (c == '\r'){
				n = recv(sock, &c, 1, MSG_PEEK);
				if (n==1 && c=='\n')
					recv(sock, &c, 1, 0);
				else
					c = '\n';
			}
			buf[i] = c;
			i++;
		}
		else if (n == 0) {
			close(sock);
		}
		else {
			c = '\n';
		}
	}
	buf[i] = '\0';

	return i;
}

int httpd_add_handler(struct httpd *phttpd, const char *url, HttpdAccessHandle handle)
{
	int state = -1;
	int i;

	if (phttpd->handlers_count >= phttpd->handlers_size){
		phttpd->handlers = realloc(phttpd->handlers, sizeof(struct http_handler)*
					(phttpd->handlers_size+8));
		phttpd->handlers_size+=8;
	}
	if (phttpd->handlers){
		phttpd->handlers[phttpd->handlers_count].url = strdup(url);
		phttpd->handlers[phttpd->handlers_count].handle = handle;
		phttpd->handlers_count++;
		state = 0;
	}
printf("%s %d: %s() count:%d size:%d\n", __FILE__, __LINE__, __func__, phttpd->handlers_count, phttpd->handlers_size);
	return state;
}

int httpd_find_handler(struct connection *conn)
{
	int state = -1;
	int i;
	struct httpd *phttpd;

	if (NULL == conn) return state;
	conn->handler_index = -1;
	phttpd = conn->server;

printf("%s %d: %s()\n", __FILE__, __LINE__, __func__);
	for (i=0; i<phttpd->handlers_count; i++){
		if (0==strcmp(conn->url+1, phttpd->handlers[i].url)){
			conn->handler_index = i;
printf("%s %d: %s() idx:%d url:%s\n", __FILE__, __LINE__, __func__, i, conn->url);
			state = 0;
			break;
		}
	}
	return state;
}

int httpd_process_handler(struct connection *conn)
{
	int state = -1;
	
	if (NULL == conn) return state;

	if (NULL == conn->filepath && conn->handler_index>=0){
		return conn->server->handlers[conn->handler_index].handle(conn);
	}	
 
	return state;

}
int response_buffer(struct connection *conn)
{
	int state = 0;
	char buff[512];
	snprintf(buff, 512, "HTTP/1.1 200 OK\r\nServer: %s\r\nContent-Type: %s\r\n", SERVER_STRING, mimeTypes[conn->content_type]);
	if (conn->content_lenght){
		int ll = strlen(buff);
		snprintf(buff+ll, 512-ll, "Content-Lenght: %d\r\n", conn->content_lenght);
	}
	strcat(buff, "\r\n");
	send(conn->socket_fd, buff, strlen(buff), 0);
	send(conn->socket_fd, conn->content, strlen(conn->content),0);	
	return state;
}

int httpd_response(struct connection* conn)
{
	int state = 0;
	
printf("%s %d: %s()\n", __FILE__, __LINE__, __func__);
	switch(conn->state){
		case STATE_NOTFOUND:
			not_found(conn->socket_fd);
			break;
		case 501:
			unimplemented(conn->socket_fd);
			break;
		case STATE_OK:
		case 0:
			if (conn->filepath){
				send_file(conn->socket_fd, conn->filepath);
			}
			else
				response_buffer(conn);
			break;
		case STATE_REQERR:
		default:
			bad_request(conn->socket_fd);
			
	}
	clear_request(conn);
	return state;
}

#if 0
void accept_request(struct httpd *phttpd, int sockfd)
{
	#define BUFFER_SIZE 4096 //MAX HTTP Request Header limited
	char buf[BUFFER_SIZE];
	int hd_size = 0;

            /*因为ET模式只触发一次，所以使用循环确保数据全部接受*/  
	while(1){
		memset( buf, '\0', BUFFER_SIZE );  
		//int ret = recv(sockfd, buf, BUFFER_SIZE-1, 0);  
		int ret = recv(sockfd, buf, BUFFER_SIZE-1, MSG_PEEK);// MSG_PEEK 查看数据,并不从系统缓冲区移走数据
		if( ret < 0 ) {  
			/*下面if条件成立，则读缓冲区数据已经读取完成*/ 
			if( ( errno == EAGAIN ) || ( errno == EINTR ) || ( errno == EWOULDBLOCK )) {
				printf( "read later: %d\n", errno);
				break;
			}
			else {
			printf("read error: %d, close socket: %d\n", error, sockfd);
			close( sockfd );
			//epoll_ctl(
			del_connfd(phttpd, sockfd);
			break;
			}  
                }  
                else if( ret == 0 ){
			printf("close socket: %d\n", sockfd);
			close( sockfd );  
			del_connfd(phttpd, sockfd);
			break;  
                }
                else {
			char *p = strstr(buf, "\r\n\r\n");
			if (p){
				struct connection* conn = get_connection(phttpd, sockfd);
				if (conn){
					hd_size = 4 +  (p - buf);
					printf("header size: %d\n", hd_size);	
					buf[hd_size] = 0;
					recv(sockfd, buf, hd_size, 0);
					parse_header(conn, buf, hd_size);
					check_responder(conn);
					httpd_response(conn);
					//keep alive????
					close(sockfd);
					del_connfd(phttpd, sockfd);
				}
			}
			//printf("get %d bytes of content: %s\n", ret, buf);  
		}
	}
}
#endif

int epoll_write(int fd, unsigned char *buff, int len)
{
	int slen = 0;

	do {
		int wcnt = write(fd, buff+ slen, len-slen);
		if (wcnt > 0)
			break;
	}while(0);

	return slen;
}

#if 0
static void epool_triger(struct epoll_event* events, int num, int epollfd, struct httpd *phttpd)  
{
#define BUFFER_SIZE 1024
	int i;
	char buf[BUFFER_SIZE];  
	int listenfd = phttpd->socket;
	for (i = 0; i < num; i++ ) {  
		int sockfd = events[i].data.fd;
		if ((events[i].events & EPOLLERR) ||
			(events[i].events & EPOLLHUP) ||
			(!(events[i].events & EPOLLIN)))
		{
			/* An error has occured on this fd, or the socket is not
			ready for reading (why were we notified then?) */
			fprintf (stderr, "epoll error\n");
			del_connfd(phttpd, sockfd);
			continue;
		}
		if (NULL == events[i].data.ptr){
			fprintf (stderr, "shutdown??????\n");
			continue;	//shutdown signal
		}

		if ( sockfd == listenfd ){  //new connetion
			struct sockaddr_in client_addr;  
			socklen_t addrlength = sizeof(client_addr);  
			int connfd = accept(listenfd, (struct sockaddr*)&client_addr, &addrlength);
			epoll_addfd(epollfd, connfd);
			add_connection(phttpd, connfd);
		}  
		else if ( events[i].events & EPOLLIN )  
		{  
			printf( "event trigger once %d\n", events[i].data.fd );  //recv data
			accept_request(phttpd, events[i].data.fd);
		}
		else {
			printf( "something else happened \n" );
		}
	}
}
#endif

static void epoll_addfd(int epfd, int fd)
{
	struct epoll_event event;
	int flags;

	flags = fcntl(fd, F_GETFL);
	if (-1 != flags)
		fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);

}

static void epoll_delfd(int epfd, int fd)
{
	struct epoll_event event;
	
	memset(&event, 0, sizeof(event));
	event.data.fd = fd;
	epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &event);

}

int epoll_run(struct httpd* phttpd)
{

	struct epoll_event event;
	int epfd;
	int flags;
	struct epoll_event events[MAX_EVENT]; 

	epfd = epoll_create(MAX_EVENT);
#if 0
	flags = fcntl (epfd, F_GETFD);
	if (-1 != flags)
		fcntl(epfd, F_SETFD, flags | FD_CLOEXEC);
#endif
	epoll_addfd(epfd, phttpd->socket);

	while(1){
		int i;
		int ret = epoll_wait( epfd, events, MAX_EVENT, -1 );
		if ( ret < 0 ){
			printf( "epoll failure\n" );
			break;
		}
		for (i = 0; i < ret; i++){
			int eventfd = events[i].data.fd;
			if ((events[i].events & EPOLLERR) ||
				(events[i].events & EPOLLHUP) ||
				(!(events[i].events & EPOLLIN))){
				/* An error has occured on this fd, or the socket is not
				 ready for reading (why were we notified then?) */
				perror("epoll error");
				//close (eventfd);
				del_connfd(phttpd, eventfd);
				continue;
			}
			else if (phttpd->socket == eventfd){
				/* We have a notification on the listening socket, which
				means one or more incoming connections. */
				while (1){
					struct sockaddr in_addr;
					socklen_t in_len;
					int infd, s;
					char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

					in_len = sizeof in_addr;
					infd = accept (phttpd->socket, &in_addr, &in_len);
					if (infd == -1){
						if ((errno == EAGAIN) || (errno == EWOULDBLOCK)){
							/* We have processed all incoming
							connections. */
							break;
						}
						else {
							perror ("accept");
							break;
						}
					}

					s = getnameinfo (&in_addr, in_len,
							hbuf, sizeof hbuf,
							sbuf, sizeof sbuf,
							NI_NUMERICHOST | NI_NUMERICSERV);
					if (s == 0) {
					printf("Accepted connection on descriptor %d "
					     "(host=%s, port=%s)\n", infd, hbuf, sbuf);
					}
					/* Make the incoming socket non-blocking and add it to the list of fds to monitor. */
					epoll_addfd(epfd, infd);
					add_connection(phttpd, infd);
				}
				continue;
			}
			else if (events[i].events & EPOLLIN) {
				/* We have data on the fd waiting to be read. Read and
				 display it. We must read whatever data is available
				 completely, as we are running in edge-triggered mode
				 and won't get a notification again for the same
				 data. */
				while (1) {
					ssize_t count;
					char buf[512];
					//int done = 0;

					//count = read (eventfd, buf, sizeof buf);
					count = recv(eventfd, buf, sizeof(buf), MSG_PEEK);// MSG_PEEK 查看数据,并不从系统缓冲区移走数据
					if (count < 0) {
						/* If errno == EAGAIN, that means we have read all
						 data. So go back to the main loop. */
						perror ("recv error");
						if ( EAGAIN==errno || EINTR==errno ) {
							printf("error no: %d\n", errno);
						}
						else {
							//done = 1;
							printf("close %d\n", eventfd);
							close(eventfd);
							epoll_delfd(epfd, eventfd);
							del_connfd(phttpd, eventfd);
						}
						break;
					}
					else if (count == 0) {
						/* End of file. The remote has closed the
						 connection. */
						printf ("Closed connection on descriptor %d\n",	eventfd);
						/* Closing the descriptor will make epoll remove it
						from the set of descriptors which are monitored. */
						//close (eventfd);
						del_connfd(phttpd, eventfd);
						break;
					}
					else {
						char *p = strstr(buf, "\r\n\r\n");
						if (p){
							struct connection* conn = get_connection(phttpd, eventfd);
							if (conn){
								int hd_size = 4 +  (p - buf);
								printf("header size: %d\n", hd_size);	
								buf[hd_size] = 0;
								recv(eventfd, buf, hd_size, 0);
								parse_header(conn, buf, hd_size);
								check_responder(conn);
								httpd_process_handler(conn);
								httpd_response(conn);
								//del_connfd(phttpd, eventfd);
							}
						}
					}
				}
			}
			else {
				printf( "something else happened \n" );
			}

                }
		//epool_triger( events, ret, epfd, phttpd);
	}
	close(epfd);
}



int httpd_run(struct httpd* phttpd)
{
	epoll_run(phttpd);
	destory_httpd(phttpd);
}


int destory_httpd(struct httpd* phttpd)
{
	int ret = -1;

	return ret;

}


static ssize_t socket_send(int sockfd, const char* buffer, size_t buflen)
{
	ssize_t ret;
	size_t total = buflen;
	const char *p = buffer;

	while(1){
		ret =send(sockfd, p, total, 0);
		if(ret <0){
			if(errno == EINTR)
				return -1;
			//buffer full, retry
			if(errno == EAGAIN){
				usleep(1000);
				continue;
			}
			return -1;
		}

		if((size_t)ret == total)
			return buflen;

		total -= ret;
		p += ret;
	}

	return ret;
}

