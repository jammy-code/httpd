


struct connection * add_connection(struct httpd *phttpd, int socked_fd)
{
	struct connection *conn = (struct connection *)malloc(sizeof(struct connection));

	if (conn){
		int i;
		memset(conn, 0, sizeof(struct connection));
		conn->socket_fd = socket_fd;
		conn->server = phttpd;
		if (phttpd->conn_sum == 0){
			phttpd->conns[0] = conn;
			phttpd->conn_sum = 1;
			return conn;
		}
		for (i=phttpd->conn_sum-1; i>=0; i--){
			if (conn->socket_fd < phttpd->conns[i]->socket_fd){
				phttpd->conns[i+1]=phttpd->conns[i];
				if (i==0){
					phttpd->conns[i+1]= conn;
				}
			}
			else {
				phttpd->conns[i+1]= conn;
			}
		}
		phttpd->conn_sum += 1;
	}
	return conn;
}

void del_connfd(struct httpd *phttpd, int socked_fd)
{
	int i, j;
	for (i=phttpd->conn_sum-1; i>=0; i--){
		if (phttpd->conns[i]->socket_fd == socked_fd){
			free_connection(phttpd->conns[i]);
			for (j=i+1; j<=phttpd->conn_sum-1; j++,i++){
				phttpd->conns[i]= phttpd->conns[j];
				 phttpd->conns[j] = NULL;
			}
			break;
		}
	}
}

int parse_header(struct httpd *phttpd, int socked_fd, const char *buff, int len)
{
	int state = 0;
	
	
	return state;	
}


const char *getStatusDesc(int code)
{
	const char* res;

      switch(code) {
      case 100: res = "Continue";
          break;
      case 101: res = "Switching Protocols";
          break;
      //--------------------------------------
      case 200: res = "OK";
          break;
      case 201: res = "Created";
          break;
      case 202: res = "Accepted";
          break;
      case 203: res = "Non-Authoritative Information";
          break;
      case 204: res = "No Content";
          break;
      case 205: res = "Reset Content";
          break;
      case 206: res = "Partial Content";
          break;
      //--------------------------------------
      case 300: res = "Multiple Choices";
          break;
      case 301: res = "Moved Permanently";
          break;
      case 302: res = "Found";
          break;
      case 303: res = "See Other";
          break;
      case 304: res = "Not Modified";
          break;
      case 305: res = "Use Proxy";
          break;
      case 306: res = "Switch Proxy";
          break;
      case 307: res = "Temporary Redirect";
          break;
      //--------------------------------------
      case 400: res = "Bad Request";
          break;
      case 401: res = "Unauthorized";
          break;
      case 402: res = "Payment Required";
          break;
      case 403: res = "Forbidden";
          break;
      case 404: res = "Not Found";
          break;
      case 405: res = "Method Not Allowed";
          break;
      case 406: res = "Not Acceptable";
          break;
      case 407: res = "Proxy Authentication Required";
          break;
      case 408: res = "Request Timeout";
          break;
      case 409: res = "Conflict";
          break;
      case 410: res = "Gone";
          break;
      case 411: res = "Length Required";
          break;
      case 412: res = "Precondition Failed";
          break;
      case 413: res = "Request Entity Too Large";
          break;
      case 414: res = "Request-URI Too Long";
          break;
      case 415: res = "Unsupported Media Type";
          break;
      case 416: res = "Requested Range Not Satisfiable";
          break;
      case 417: res = "Expectation Failed";
          break;
      //--------------------------------------
      case 500: res = "Internal Server Error";
          break;
      case 501: res = "Not Implemented";
          break;
      case 502: res = "Bad Gateway";
          break;
      case 503: res = "Service Unavailable";
          break;
      case 504: res = "Gateway Timeout";
          break;
      case 505: res = "HTTP Version Not Supported";
          break;
      default:
          res = "";
      } // switch

      return res;

}
