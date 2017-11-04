#include <stdlib.h>



#include "httpd.h"
#include "list.h"

#define COOKIE_NAME "session"


struct session {
	struct list node;
	char id[33];
	time_t start;
	char value[64];
	unsigned int rc;	//reference counter

};





static struct session *get_session(struct connection *conn)
{

	struct session *ret;
	const char *cookie;

//	cookie = lookup_connection(conn, HTTP_KEY_VALUE_COOKIE, COOKIE_NAME);

//	if (NULL == cookie){
//		ret = sessions;
//		while (NULL 
//	}
	ret = NULL;
	return ret;
} 
