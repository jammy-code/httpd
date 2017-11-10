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

/* Main HTTP headers. */
/* Standard.      RFC7231, Section 5.3.2 */
#define HTTP_HEADER_ACCEPT "Accept"
/* Standard.      RFC7231, Section 5.3.3 */
#define HTTP_HEADER_ACCEPT_CHARSET "Accept-Charset"
/* Standard.      RFC7231, Section 5.3.4; RFC7694, Section 3 */
#define HTTP_HEADER_ACCEPT_ENCODING "Accept-Encoding"
/* Standard.      RFC7231, Section 5.3.5 */
#define HTTP_HEADER_ACCEPT_LANGUAGE "Accept-Language"
/* Standard.      RFC7233, Section 2.3 */
#define HTTP_HEADER_ACCEPT_RANGES "Accept-Ranges"
/* Standard.      RFC7234, Section 5.1 */
#define HTTP_HEADER_AGE "Age"
/* Standard.      RFC7231, Section 7.4.1 */
#define HTTP_HEADER_ALLOW "Allow"
/* Standard.      RFC7235, Section 4.2 */
#define HTTP_HEADER_AUTHORIZATION "Authorization"
/* Standard.      RFC7234, Section 5.2 */
#define HTTP_HEADER_CACHE_CONTROL "Cache-Control"
/* Reserved.      RFC7230, Section 8.1 */
#define HTTP_HEADER_CLOSE "Close"
/* Standard.      RFC7230, Section 6.1 */
#define HTTP_HEADER_CONNECTION "Connection"
/* Standard.      RFC7231, Section 3.1.2.2 */
#define HTTP_HEADER_CONTENT_ENCODING "Content-Encoding"
/* Standard.      RFC7231, Section 3.1.3.2 */
#define HTTP_HEADER_CONTENT_LANGUAGE "Content-Language"
/* Standard.      RFC7230, Section 3.3.2 */
#define HTTP_HEADER_CONTENT_LENGTH "Content-Length"
/* Standard.      RFC7231, Section 3.1.4.2 */
#define HTTP_HEADER_CONTENT_LOCATION "Content-Location"
/* Standard.      RFC7233, Section 4.2 */
#define HTTP_HEADER_CONTENT_RANGE "Content-Range"
/* Standard.      RFC7231, Section 3.1.1.5 */
#define HTTP_HEADER_CONTENT_TYPE "Content-Type"
/* Standard.      RFC7231, Section 7.1.1.2 */
#define HTTP_HEADER_DATE "Date"
/* Standard.      RFC7232, Section 2.3 */
#define HTTP_HEADER_ETAG "ETag"
/* Standard.      RFC7231, Section 5.1.1 */
#define HTTP_HEADER_EXPECT "Expect"
/* Standard.      RFC7234, Section 5.3 */
#define HTTP_HEADER_EXPIRES "Expires"
/* Standard.      RFC7231, Section 5.5.1 */
#define HTTP_HEADER_FROM "From"
/* Standard.      RFC7230, Section 5.4 */
#define HTTP_HEADER_HOST "Host"
/* Standard.      RFC7232, Section 3.1 */
#define HTTP_HEADER_IF_MATCH "If-Match"
/* Standard.      RFC7232, Section 3.3 */
#define HTTP_HEADER_IF_MODIFIED_SINCE "If-Modified-Since"
/* Standard.      RFC7232, Section 3.2 */
#define HTTP_HEADER_IF_NONE_MATCH "If-None-Match"
/* Standard.      RFC7233, Section 3.2 */
#define HTTP_HEADER_IF_RANGE "If-Range"
/* Standard.      RFC7232, Section 3.4 */
#define HTTP_HEADER_IF_UNMODIFIED_SINCE "If-Unmodified-Since"
/* Standard.      RFC7232, Section 2.2 */
#define HTTP_HEADER_LAST_MODIFIED "Last-Modified"
/* Standard.      RFC7231, Section 7.1.2 */
#define HTTP_HEADER_LOCATION "Location"
/* Standard.      RFC7231, Section 5.1.2 */
#define HTTP_HEADER_MAX_FORWARDS "Max-Forwards"
/* Standard.      RFC7231, Appendix A.1 */
#define HTTP_HEADER_MIME_VERSION "MIME-Version"
/* Standard.      RFC7234, Section 5.4 */
#define HTTP_HEADER_PRAGMA "Pragma"
/* Standard.      RFC7235, Section 4.3 */
#define HTTP_HEADER_PROXY_AUTHENTICATE "Proxy-Authenticate"
/* Standard.      RFC7235, Section 4.4 */
#define HTTP_HEADER_PROXY_AUTHORIZATION "Proxy-Authorization"
/* Standard.      RFC7233, Section 3.1 */
#define HTTP_HEADER_RANGE "Range"
/* Standard.      RFC7231, Section 5.5.2 */
#define HTTP_HEADER_REFERER "Referer"
/* Standard.      RFC7231, Section 7.1.3 */
#define HTTP_HEADER_RETRY_AFTER "Retry-After"
/* Standard.      RFC7231, Section 7.4.2 */
#define HTTP_HEADER_SERVER "Server"
/* Standard.      RFC7230, Section 4.3 */
#define HTTP_HEADER_TE "TE"
/* Standard.      RFC7230, Section 4.4 */
#define HTTP_HEADER_TRAILER "Trailer"
/* Standard.      RFC7230, Section 3.3.1 */
#define HTTP_HEADER_TRANSFER_ENCODING "Transfer-Encoding"
/* Standard.      RFC7230, Section 6.7 */
#define HTTP_HEADER_UPGRADE "Upgrade"
/* Standard.      RFC7231, Section 5.5.3 */
#define HTTP_HEADER_USER_AGENT "User-Agent"
/* Standard.      RFC7231, Section 7.1.4 */
#define HTTP_HEADER_VARY "Vary"
/* Standard.      RFC7230, Section 5.7.1 */
#define HTTP_HEADER_VIA "Via"
/* Standard.      RFC7235, Section 4.1 */
#define HTTP_HEADER_WWW_AUTHENTICATE "WWW-Authenticate"
/* Standard.      RFC7234, Section 5.5 */
#define HTTP_HEADER_WARNING "Warning"

/* Additional HTTP headers. */
/* No category.   RFC4229 */
#define HTTP_HEADER_A_IM "A-IM"
/* No category.   RFC4229 */
#define HTTP_HEADER_ACCEPT_ADDITIONS "Accept-Additions"
/* Informational. RFC7089 */
#define HTTP_HEADER_ACCEPT_DATETIME "Accept-Datetime"
/* No category.   RFC4229 */
#define HTTP_HEADER_ACCEPT_FEATURES "Accept-Features"
/* No category.   RFC5789 */
#define HTTP_HEADER_ACCEPT_PATCH "Accept-Patch"
/* Standard.      RFC7639, Section 2 */
#define HTTP_HEADER_ALPN "ALPN"
/* Standard.      RFC7838 */
#define HTTP_HEADER_ALT_SVC "Alt-Svc"
/* Standard.      RFC7838 */
#define HTTP_HEADER_ALT_USED "Alt-Used"
/* No category.   RFC4229 */
#define HTTP_HEADER_ALTERNATES "Alternates"
/* No category.   RFC4437 */
#define HTTP_HEADER_APPLY_TO_REDIRECT_REF "Apply-To-Redirect-Ref"
/* Experimental.  RFC8053, Section 4 */
#define HTTP_HEADER_AUTHENTICATION_CONTROL "Authentication-Control"
/* Standard.      RFC7615, Section 3 */
#define HTTP_HEADER_AUTHENTICATION_INFO "Authentication-Info"
/* No category.   RFC4229 */
#define HTTP_HEADER_C_EXT "C-Ext"
/* No category.   RFC4229 */
#define HTTP_HEADER_C_MAN "C-Man"
/* No category.   RFC4229 */
#define HTTP_HEADER_C_OPT "C-Opt"
/* No category.   RFC4229 */
#define HTTP_HEADER_C_PEP "C-PEP"
/* No category.   RFC4229 */
#define HTTP_HEADER_C_PEP_INFO "C-PEP-Info"
/* Standard.      RFC7809, Section 7.1 */
#define HTTP_HEADER_CALDAV_TIMEZONES "CalDAV-Timezones"
/* Obsoleted.     RFC2068; RFC2616 */
#define HTTP_HEADER_CONTENT_BASE "Content-Base"
/* Standard.      RFC6266 */
#define HTTP_HEADER_CONTENT_DISPOSITION "Content-Disposition"
/* No category.   RFC4229 */
#define HTTP_HEADER_CONTENT_ID "Content-ID"
/* No category.   RFC4229 */
#define HTTP_HEADER_CONTENT_MD5 "Content-MD5"
/* No category.   RFC4229 */
#define HTTP_HEADER_CONTENT_SCRIPT_TYPE "Content-Script-Type"
/* No category.   RFC4229 */
#define HTTP_HEADER_CONTENT_STYLE_TYPE "Content-Style-Type"
/* No category.   RFC4229 */
#define HTTP_HEADER_CONTENT_VERSION "Content-Version"
/* Standard.      RFC6265 */
#define HTTP_HEADER_COOKIE "Cookie"
/* Obsoleted.     RFC2965; RFC6265 */
#define HTTP_HEADER_COOKIE2 "Cookie2"
/* Standard.      RFC5323 */
#define HTTP_HEADER_DASL "DASL"
/* Standard.      RFC4918 */
#define HTTP_HEADER_DAV "DAV"
/* No category.   RFC4229 */
#define HTTP_HEADER_DEFAULT_STYLE "Default-Style"
/* No category.   RFC4229 */
#define HTTP_HEADER_DELTA_BASE "Delta-Base"
/* Standard.      RFC4918 */
#define HTTP_HEADER_DEPTH "Depth"
/* No category.   RFC4229 */
#define HTTP_HEADER_DERIVED_FROM "Derived-From"
/* Standard.      RFC4918 */
#define HTTP_HEADER_DESTINATION "Destination"
/* No category.   RFC4229 */
#define HTTP_HEADER_DIFFERENTIAL_ID "Differential-ID"
/* No category.   RFC4229 */
#define HTTP_HEADER_DIGEST "Digest"
/* No category.   RFC4229 */
#define HTTP_HEADER_EXT "Ext"
/* Standard.      RFC7239 */
#define HTTP_HEADER_FORWARDED "Forwarded"
/* No category.   RFC4229 */
#define HTTP_HEADER_GETPROFILE "GetProfile"
/* Experimental.  RFC7486, Section 6.1.1 */
#define HTTP_HEADER_HOBAREG "Hobareg"
/* Standard.      RFC7540, Section 3.2.1 */
#define HTTP_HEADER_HTTP2_SETTINGS "HTTP2-Settings"
/* No category.   RFC4229 */
#define HTTP_HEADER_IM "IM"
/* Standard.      RFC4918 */
#define HTTP_HEADER_IF "If"
/* Standard.      RFC6638 */
#define HTTP_HEADER_IF_SCHEDULE_TAG_MATCH "If-Schedule-Tag-Match"
/* No category.   RFC4229 */
#define HTTP_HEADER_KEEP_ALIVE "Keep-Alive"
/* No category.   RFC4229 */
#define HTTP_HEADER_LABEL "Label"
/* No category.   RFC5988 */
#define HTTP_HEADER_LINK "Link"
/* Standard.      RFC4918 */
#define HTTP_HEADER_LOCK_TOKEN "Lock-Token"
/* No category.   RFC4229 */
#define HTTP_HEADER_MAN "Man"
/* Informational. RFC7089 */
#define HTTP_HEADER_MEMENTO_DATETIME "Memento-Datetime"
/* No category.   RFC4229 */
#define HTTP_HEADER_METER "Meter"
/* No category.   RFC4229 */
#define HTTP_HEADER_NEGOTIATE "Negotiate"
/* No category.   RFC4229 */
#define HTTP_HEADER_OPT "Opt"
/* Experimental.  RFC8053, Section 3 */
#define HTTP_HEADER_OPTIONAL_WWW_AUTHENTICATE "Optional-WWW-Authenticate"
/* Standard.      RFC4229 */
#define HTTP_HEADER_ORDERING_TYPE "Ordering-Type"
/* Standard.      RFC6454 */
#define HTTP_HEADER_ORIGIN "Origin"
/* Standard.      RFC4918 */
#define HTTP_HEADER_OVERWRITE "Overwrite"
/* No category.   RFC4229 */
#define HTTP_HEADER_P3P "P3P"
/* No category.   RFC4229 */
#define HTTP_HEADER_PEP "PEP"
/* No category.   RFC4229 */
#define HTTP_HEADER_PICS_LABEL "PICS-Label"
/* No category.   RFC4229 */
#define HTTP_HEADER_PEP_INFO "Pep-Info"
/* Standard.      RFC4229 */
#define HTTP_HEADER_POSITION "Position"
/* Standard.      RFC7240 */
#define HTTP_HEADER_PREFER "Prefer"
/* Standard.      RFC7240 */
#define HTTP_HEADER_PREFERENCE_APPLIED "Preference-Applied"
/* No category.   RFC4229 */
#define HTTP_HEADER_PROFILEOBJECT "ProfileObject"
/* No category.   RFC4229 */
#define HTTP_HEADER_PROTOCOL "Protocol"
/* No category.   RFC4229 */
#define HTTP_HEADER_PROTOCOL_INFO "Protocol-Info"
/* No category.   RFC4229 */
#define HTTP_HEADER_PROTOCOL_QUERY "Protocol-Query"
/* No category.   RFC4229 */
#define HTTP_HEADER_PROTOCOL_REQUEST "Protocol-Request"
/* Standard.      RFC7615, Section 4 */
#define HTTP_HEADER_PROXY_AUTHENTICATION_INFO "Proxy-Authentication-Info"
/* No category.   RFC4229 */
#define HTTP_HEADER_PROXY_FEATURES "Proxy-Features"
/* No category.   RFC4229 */
#define HTTP_HEADER_PROXY_INSTRUCTION "Proxy-Instruction"
/* No category.   RFC4229 */
#define HTTP_HEADER_PUBLIC "Public"
/* Standard.      RFC7469 */
#define HTTP_HEADER_PUBLIC_KEY_PINS "Public-Key-Pins"
/* Standard.      RFC7469 */
#define HTTP_HEADER_PUBLIC_KEY_PINS_REPORT_ONLY "Public-Key-Pins-Report-Only"
/* No category.   RFC4437 */
#define HTTP_HEADER_REDIRECT_REF "Redirect-Ref"
/* No category.   RFC4229 */
#define HTTP_HEADER_SAFE "Safe"
/* Standard.      RFC6638 */
#define HTTP_HEADER_SCHEDULE_REPLY "Schedule-Reply"
/* Standard.      RFC6638 */
#define HTTP_HEADER_SCHEDULE_TAG "Schedule-Tag"
/* Standard.      RFC6455 */
#define HTTP_HEADER_SEC_WEBSOCKET_ACCEPT "Sec-WebSocket-Accept"
/* Standard.      RFC6455 */
#define HTTP_HEADER_SEC_WEBSOCKET_EXTENSIONS "Sec-WebSocket-Extensions"
/* Standard.      RFC6455 */
#define HTTP_HEADER_SEC_WEBSOCKET_KEY "Sec-WebSocket-Key"
/* Standard.      RFC6455 */
#define HTTP_HEADER_SEC_WEBSOCKET_PROTOCOL "Sec-WebSocket-Protocol"
/* Standard.      RFC6455 */
#define HTTP_HEADER_SEC_WEBSOCKET_VERSION "Sec-WebSocket-Version"
/* No category.   RFC4229 */
#define HTTP_HEADER_SECURITY_SCHEME "Security-Scheme"
/* Standard.      RFC6265 */
#define HTTP_HEADER_SET_COOKIE "Set-Cookie"
/* Obsoleted.     RFC2965; RFC6265 */
#define HTTP_HEADER_SET_COOKIE2 "Set-Cookie2"
/* No category.   RFC4229 */
#define HTTP_HEADER_SETPROFILE "SetProfile"
/* Standard.      RFC5023 */
#define HTTP_HEADER_SLUG "SLUG"
/* No category.   RFC4229 */
#define HTTP_HEADER_SOAPACTION "SoapAction"
/* No category.   RFC4229 */
#define HTTP_HEADER_STATUS_URI "Status-URI"
/* Standard.      RFC6797 */
#define HTTP_HEADER_STRICT_TRANSPORT_SECURITY "Strict-Transport-Security"
/* No category.   RFC4229 */
#define HTTP_HEADER_SURROGATE_CAPABILITY "Surrogate-Capability"
/* No category.   RFC4229 */
#define HTTP_HEADER_SURROGATE_CONTROL "Surrogate-Control"
/* No category.   RFC4229 */
#define HTTP_HEADER_TCN "TCN"
/* Standard.      RFC4918 */
#define HTTP_HEADER_TIMEOUT "Timeout"
/* Standard.      RFC8030, Section 5.4 */
#define HTTP_HEADER_TOPIC "Topic"
/* Standard.      RFC8030, Section 5.2 */
#define HTTP_HEADER_TTL "TTL"
/* Standard.      RFC8030, Section 5.3 */
#define HTTP_HEADER_URGENCY "Urgency"
/* No category.   RFC4229 */
#define HTTP_HEADER_URI "URI"
/* No category.   RFC4229 */
#define HTTP_HEADER_VARIANT_VARY "Variant-Vary"
/* No category.   RFC4229 */
#define HTTP_HEADER_WANT_DIGEST "Want-Digest"
/* Informational. RFC7034 */
#define HTTP_HEADER_X_FRAME_OPTIONS "X-Frame-Options"

/* Some provisional headers. */
#define HTTP_HEADER_ACCESS_CONTROL_ALLOW_ORIGIN "Access-Control-Allow-Origin"
/** @} */ /* end of group headers */


/* Main HTTP methods. */
/* Not safe. Not idempotent. RFC7231, Section 4.3.6. */
#define HTTP_METHOD_CONNECT "CONNECT"
/* Not safe. Idempotent.     RFC7231, Section 4.3.5. */
#define HTTP_METHOD_DELETE "DELETE"
/* Safe.     Idempotent.     RFC7231, Section 4.3.1. */
#define HTTP_METHOD_GET "GET"
/* Safe.     Idempotent.     RFC7231, Section 4.3.2. */
#define HTTP_METHOD_HEAD "HEAD"
/* Safe.     Idempotent.     RFC7231, Section 4.3.7. */
#define HTTP_METHOD_OPTIONS "OPTIONS"
/* Not safe. Not idempotent. RFC7231, Section 4.3.3. */
#define HTTP_METHOD_POST "POST"
/* Not safe. Idempotent.     RFC7231, Section 4.3.4. */
#define HTTP_METHOD_PUT "PUT"
/* Safe.     Idempotent.     RFC7231, Section 4.3.8. */
#define HTTP_METHOD_TRACE "TRACE"

/* Additional HTTP methods. */
/* Not safe. Idempotent.     RFC3744, Section 8.1. */
#define HTTP_METHOD_ACL "ACL"
/* Not safe. Idempotent.     RFC3253, Section 12.6. */
#define HTTP_METHOD_BASELINE_CONTROL "BASELINE-CONTROL"
/* Not safe. Idempotent.     RFC5842, Section 4. */
#define HTTP_METHOD_BIND "BIND"
/* Not safe. Idempotent.     RFC3253, Section 4.4, Section 9.4. */
#define HTTP_METHOD_CHECKIN "CHECKIN"
/* Not safe. Idempotent.     RFC3253, Section 4.3, Section 8.8. */
#define HTTP_METHOD_CHECKOUT "CHECKOUT"
/* Not safe. Idempotent.     RFC4918, Section 9.8. */
#define HTTP_METHOD_COPY "COPY"
/* Not safe. Idempotent.     RFC3253, Section 8.2. */
#define HTTP_METHOD_LABEL "LABEL"
/* Not safe. Idempotent.     RFC2068, Section 19.6.1.2. */
#define HTTP_METHOD_LINK "LINK"
/* Not safe. Not idempotent. RFC4918, Section 9.10. */
#define HTTP_METHOD_LOCK "LOCK"
/* Not safe. Idempotent.     RFC3253, Section 11.2. */
#define HTTP_METHOD_MERGE "MERGE"
/* Not safe. Idempotent.     RFC3253, Section 13.5. */
#define HTTP_METHOD_MKACTIVITY "MKACTIVITY"
/* Not safe. Idempotent.     RFC4791, Section 5.3.1. */
#define HTTP_METHOD_MKCALENDAR "MKCALENDAR"
/* Not safe. Idempotent.     RFC4918, Section 9.3. */
#define HTTP_METHOD_MKCOL "MKCOL"
/* Not safe. Idempotent.     RFC4437, Section 6. */
#define HTTP_METHOD_MKREDIRECTREF "MKREDIRECTREF"
/* Not safe. Idempotent.     RFC3253, Section 6.3. */
#define HTTP_METHOD_MKWORKSPACE "MKWORKSPACE"
/* Not safe. Idempotent.     RFC4918, Section 9.9. */
#define HTTP_METHOD_MOVE "MOVE"
/* Not safe. Idempotent.     RFC3648, Section 7. */
#define HTTP_METHOD_ORDERPATCH "ORDERPATCH"
/* Not safe. Not idempotent. RFC5789, Section 2. */
#define HTTP_METHOD_PATCH "PATCH"
/* Safe.     Idempotent.     RFC7540, Section 3.5. */
#define HTTP_METHOD_PRI "PRI"
/* Safe.     Idempotent.     RFC4918, Section 9.1. */
#define HTTP_METHOD_PROPFIND "PROPFIND"
/* Not safe. Idempotent.     RFC4918, Section 9.2. */
#define HTTP_METHOD_PROPPATCH "PROPPATCH"
/* Not safe. Idempotent.     RFC5842, Section 6. */
#define HTTP_METHOD_REBIND "REBIND"
/* Safe.     Idempotent.     RFC3253, Section 3.6. */
#define HTTP_METHOD_REPORT "REPORT"
/* Safe.     Idempotent.     RFC5323, Section 2. */
#define HTTP_METHOD_SEARCH "SEARCH"
/* Not safe. Idempotent.     RFC5842, Section 5. */
#define HTTP_METHOD_UNBIND "UNBIND"
/* Not safe. Idempotent.     RFC3253, Section 4.5. */
#define HTTP_METHOD_UNCHECKOUT "UNCHECKOUT"
/* Not safe. Idempotent.     RFC2068, Section 19.6.1.3. */
#define HTTP_METHOD_UNLINK "UNLINK"
/* Not safe. Idempotent.     RFC4918, Section 9.11. */
#define HTTP_METHOD_UNLOCK "UNLOCK"
/* Not safe. Idempotent.     RFC3253, Section 7.1. */
#define HTTP_METHOD_UPDATE "UPDATE"
/* Not safe. Idempotent.     RFC4437, Section 7. */
#define HTTP_METHOD_UPDATEREDIRECTREF "UPDATEREDIRECTREF"
/* Not safe. Idempotent.     RFC3253, Section 3.5. */
#define HTTP_METHOD_VERSION_CONTROL "VERSION-CONTROL"

#define HTTP_VERSION_1_0 "HTTP/1.0"
#define HTTP_VERSION_1_1 "HTTP/1.1"

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

const char *get_datetime_string()
{
	const char *ret=NULL;
	const char str_datetime[32];
	static const char *const days[]={
		"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	static const char *const mons[] ={
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	struct tm now, *pNow;
	time_t t;

	time(&t);
	pNow = gmtime(&t);
	if (NULL == pNow)
		return ret;
	now = *pNow;
	snprintf(str_datetime, sizeof(str_datetime), "%3s, %02u %3s %04u %02u:%02u:%02u GMT",
			days[now.tm_wday % 7], (unsigned int)now.tm_mday,
			mons[now.tm_mon % 12], (unsigned int)((1900+now.tm_year)%10000),
			(unsigned int) now.tm_hour, (unsigned int) now.tm_min,
			(unsigned int) now.tm_sec);
	ret = str_datetime;
	return ret;
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

#define HTML_NON_AUTH_INFO "<p></p>"
#define HTML_BAD_REQUEST "<P>Your browser sent a bad request, such as a POST without a Content-Length.</P>"
#define HTML_UNAUTHORIZED "<p></p>"
#define HTML_NOT_FOUND "<p>The webpage cannot be found!</p>"
#define HTML_REQ_TIMEOUT "<p></p>"
#define HTML_REG_ENT_TOO_LARGE "<p></p>"
#define HTML_REQ_URI_TOO_LONG "<p></p>"
#define HTML_INTE_SERV_ERR "<p></p>"
#define HTML_NOT_IMPL "<HTML><HEAD><TITLE>Method Not Implemented</TITLE></HEAD><BODY><P>HTTP request method not supported.</BODY></HTML>"
#define HTML_BAD_GATEWAY "<p></p>"
#define HTML_SERV_UNAV "<p></p>"
#define HTML_VER_NOT_SUPP "<p></p>"
#define HTML_UNKNOWN_ERROR "<p></p>"

void request_error(struct connection *conn)
{
	char buff[1024];
	int clen = 0;
	const char *cont;
	
	if (NULL == conn) return;
	
	sprintf(buff, "HTTP/1.1 %d ", conn->state);
	switch (conn->state){
		
		case 203:
			strcat(buff, "Non-Authoritative Information\r\n");
			clen = sizeof(HTML_NON_AUTH_INFO)-1;
			cont = HTML_NON_AUTH_INFO;
			break;	
		case 400:
			strcat(buff, "BAD REQUEST\r\n");
			clen = sizeof(HTML_BAD_REQUEST)-1;
			cont = HTML_NOT_FOUND;
			break;
		case 401:
			strcat (buff, "Unauthorized");
			clen = sizeof(HTML_UNAUTHORIZED)-1;
			cont = HTML_UNAUTHORIZED;
			break;
		case 404:
			strcat (buff, "Unauthorized");
			clen = sizeof(HTML_NOT_FOUND)-1;
			cont = HTML_NOT_FOUND;
			break;
			
		case 408:
			strcat (buff, "Request Timeout");
			clen = sizeof(HTML_REQ_TIMEOUT)-1;
			cont = HTML_REQ_TIMEOUT；
			break;
		case 413:
			strcat (buff, "Request Entity Too Large");
			clen = sizeof(HTML_REG_ENT_TOO_LARGE)-1;
			cont = HTML_REG_ENT_TOO_LARGE；
			break;
		case 414:
			strcat (buff, "Request URI Too Long");
			clen = sizeof(HTML_REQ_URI_TOO_LONG)-1;
			cont = HTML_REQ_URI_TOO_LONG；
			break;
		case 500:
			strcat (buff, "Internal Server Error");
			clen = sizeof(HTML_INTE_SERV_ERR)-1;
			cont = HTML_INTE_SERV_ERR；
			break;
		case 501:
			strcat (buff, "Not Implemented");
			clen = sizeof(HTML_NOT_IMPL)-1;
			cont = HTML_NOT_IMPL；
			break;
		case 502:
			strcat (buff, "Bad Gateway");
			clen = sizeof(HTML_BAD_GATEWAY)-1;
			cont = HTML_BAD_GATEWAY；
			break;
		case 503:
			strcat (buff, "Service Unavailable");
			clen = sizeof(HTML_SERV_UNAV)-1;
			cont = HTML_SERV_UNAV；
			break;
		case 505:
			strcat (buff, "HTTP Version Not Supported");
			clen = sizeof(HTML_VER_NOT_SUPP)-1;
			cont = HTML_VER_NOT_SUPP；
			break;
		default:
			strcat (buff, "Unknown Error");
			clen = sizeof(HTML_UNKNOWN_ERROR)-1;
			cont = HTML_UNKNOWN_ERROR；
			break;
	}
	snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), "Content-Type: text/html\r\n"
			"Content-Length: %d\r\nConnection: close\r\nDate: %s\r\nServer: %s\r\n\r\n%s",
		 	clen, cont);
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
	sprintf(buf, "Content-Length: %d\r\n", sb.st_size);
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
printf("%s %d: %s() content-type:%d\n", __FILE__, __LINE__, __func__, conn->content_type);
	snprintf(buff, 512, "HTTP/1.1 200 OK\r\nServer: %s\r\nContent-Type: %s\r\n", SERVER_STRING, mimeTypes[conn->content_type]);
	if (conn->content_length){
		int ll = strlen(buff);
		snprintf(buff+ll, 512-ll, "Content-Length: %d\r\n", conn->content_length);
	}
	strcat(buff, "\r\n");
	send(conn->socket_fd, buff, strlen(buff), 0);
	send(conn->socket_fd, conn->content, strlen(conn->content),0);	
printf("%s %d: %s()\n", __FILE__, __LINE__, __func__);
	return state;
}

int httpd_response(struct connection* conn)
{
	int state = 0;
	
printf("%s %d: %s()\n", __FILE__, __LINE__, __func__);
	switch(conn->state){
		case STATE_OK:
		case 0:
			if (conn->filepath){
				send_file(conn->socket_fd, conn->filepath);
			}
			else{
				response_buffer(conn);
			}
			break;
		case STATE_REQERR:
		default:
			request_error(conn);
			
	}
	clear_request(conn);
	return state;
}


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
			unsigned long evt = events[i].events;
			if (phttpd->socket == eventfd){	//new accept;
				printf("epoll events:%08x, error no:%d\r\n", evt, errno);
				if (events[i].events & EPOLLIN) {
					while(1) {
						struct sockaddr in_addr;
						socklen_t in_len;
						int infd, s;
						char hbuf[NI_MAXHOST], sbuf[NI_MAXHOST];

						in_len = sizeof in_addr;
						infd = accept (phttpd->socket, &in_addr, &in_len);
						if (infd > 0){
							s = getnameinfo (&in_addr, in_len,
								hbuf, sizeof hbuf,
								sbuf, sizeof sbuf,
								NI_NUMERICHOST | NI_NUMERICSERV);
							if (s == 0) {
								printf("Accepted connection on descriptor %d "
									"(host=%s, port=%s)\n", infd, hbuf, sbuf);
							}
							printf("add connection %d\r\n", infd);
							epoll_addfd(epfd, infd);
							add_connection(phttpd, infd);
						}
						else {
							printf("accept error:%d\r\n", errno);
							if (errno == EAGAIN) {
								break;
							}
							else if (errno == EINTR){
								;//do nothing
							}
							else {
								printf("socket error, should relisten...\r\n");
							}
						}
					}//while
				}
				else if (evt & EPOLLERR ||  evt & EPOLLHUP ){
					printf("epoll error, events:%d\r\n", evt);
				}
			}
			else if (evt & EPOLLIN) {	//new data
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
							printf("socket %d error no: %d\n", eventfd, errno);
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
						epoll_delfd(epfd, eventfd);
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
				printf( "something else happened fd:%d, event:%d\r\n", eventfd, evt);
			}

                }//for
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

