#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>

#include <errno.h>

#include "httpd.h"

#define MAX_EVENT 64

#define SERVER_STRING "Server: jhttpd/0.1.0\r\n"

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
	if (httpd->conns){
		memset(phttpd->conns, 0, sizeof(struct connection*) * 128);
	}
 	return phttpd;
}

void bad_request(int client)
{
	char buf[256];

	sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
	send(client, buf, sizeof(buf), 0);
	sprintf(buf, SERVER_STRING);
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-type: text/html\r\n\r\n");
	send(client, buf, sizeof(buf), 0);
	sprintf(buf, "<P>Your browser sent a bad request, such as a POST without a Content-Length.</P>\r\n");
	send(client, buf, sizeof(buf), 0);
}

void not_found(int client)
{
	char buf[256];

	sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, SERVER_STRING);
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
	char buf[1024];
	//(void)filename;  /* could use filename to determine file type */

	strcpy(buf, "HTTP/1.0 200 OK\r\n");
	send(client, buf, strlen(buf), 0);
	strcpy(buf, SERVER_STRING);
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



static void epoll_addfd(epollfd, fd)
{
	struct epoll_event event;
	int flags;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);

	flags = fcntl(fd, F_GETFL);
	if (-1 != flags)
		fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

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

int http_response(struct connection* conn)
{
	int state = 0;
	switch(conn->state){
		case STATE_NOTFOUND:
			not_found(conn->socket_fd);
			break;
		
		case STATE_OK:
		case 0:
			if (conn->iscgi==0){
				void headers(conn->socket_fd, conn->filepath)
			}
			else
				conn->cgihandle(conn);
			break;
		case STATE_REQERR:
		default:
			bad_request(conn->socket_fd);
			
	}	
	return state;
}

void accept_request(struct httpd *phttpd, int sockfd)
{
	#define BUFFER_SIZE 4096 //MAX HTTP Request Header limited
	char buf[BUFFER_SIZE];
	int hd_size = 0;

            /*因为ET模式只触发一次，所以使用循环确保数据全部接受*/  
            while(1){
                memset( buf, '\0', BUFFER_SIZE );  
                //int ret = recv(sockfd, buf, BUFFER_SIZE-1, 0);  
                int ret = recv(sockfd, buf, BUFFER_SIZE-1, MSG_PEEK);
                if( ret < 0 ) {  
                     /*下面if条件成立，则读缓冲区数据已经读取完成*/  
                    if( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ) ) {  
                        printf( "read later\n" );  
                        break;  
                    }  
                    close( sockfd );  
			del_connfd(phttpd, sockfd);
                    break;  
                }  
                else if( ret == 0 ){
			printf("close socket: %d\n", sockfd);
			close( sockfd );  
			del_connfd(phttpd, sockfd);
                }
                else {
			char *p = strstr(buf, "\r\n\r\n");
			if (p){
				struct connection* conn = get_connection(phttpd, socked);
				hd_size = 4 +  (p - buf);
				printf("header size: %d\n", hd_size);	
				buf[hd_size] = 0;
				recv(sockfd, buf, hd_size, 0);
				parse_header(conn, buf, hd_size);
				check_responder(conn);
				http_response(conn);

			}
			printf("get %d bytes of content: %s\n", ret, buf);  
		}
	}
}


static void epool_triger(struct epoll_event* events, int num, int epollfd, struct httpd *phttpd)  
{
#define BUFFER_SIZE 1024
	int i;
    char buf[BUFFER_SIZE];  
	int listenfd = phttpd->socket;
    for (i = 0; i < num; i++ )  
    {  
        int sockfd;

	if (NULL == events[i].data.ptr)
		continue;	//shutdown signal

	sockfd = events[i].data.fd;
        if ( sockfd == listenfd )  //new connetion
        {  
            struct sockaddr_in client_address;  
            socklen_t client_addrlength = sizeof(client_address);  
            int connfd = accept(listenfd, (struct sockaddr*)&client_address, &client_addrlength);
            epoll_addfd(epollfd, connfd);
		add_connection(phttpd, sockfd);
        }  
        else if ( events[i].events & EPOLLIN )  
        {  
            printf( "event trigger once\n" );  //recv data
		accept_request(phttpd, sockfd);

        }
        else {
            printf( "something else happened \n" );  
        }  
    }  
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
/*
	flags = fcntl (epfd, F_GETFL);
	if (-1 != flags)
		fcntl(epfd, F_SETFL, flags | O_NONBLOCK);
				
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = phttpd->socket;
	epoll_ctl (epfd, EPOLL_CTL_ADD, phttpd->socket, &event);*/
	epoll_addfd(epfd, phttpd->socket);

	while(1){
		int ret = epoll_wait( epfd, events, MAX_EVENT, -1 );
		if ( ret < 0 ){
			printf( "epoll failure\n" );
			break;
		}
		epool_triger( events, ret, epfd, phttpd);
	}
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

