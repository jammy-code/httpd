#ifndef __HTTPD_HEADER_H__
#define __HTTPD_HEADER_H__

/* See also: http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html */
#define HTTP_HEADER_ACCEPT "Accept"
#define HTTP_HEADER_ACCEPT_CHARSET "Accept-Charset"
#define HTTP_HEADER_ACCEPT_ENCODING "Accept-Encoding"
#define HTTP_HEADER_ACCEPT_LANGUAGE "Accept-Language"
#define HTTP_HEADER_ACCEPT_RANGES "Accept-Ranges"
#define HTTP_HEADER_AGE "Age"
#define HTTP_HEADER_ALLOW "Allow"
#define HTTP_HEADER_AUTHORIZATION "Authorization"
#define HTTP_HEADER_CACHE_CONTROL "Cache-Control"
#define HTTP_HEADER_CONNECTION "Connection"
#define HTTP_HEADER_CONTENT_ENCODING "Content-Encoding"
#define HTTP_HEADER_CONTENT_LANGUAGE "Content-Language"
#define HTTP_HEADER_CONTENT_LENGTH "Content-Length"
#define HTTP_HEADER_CONTENT_LOCATION "Content-Location"
#define HTTP_HEADER_CONTENT_MD5 "Content-MD5"
#define HTTP_HEADER_CONTENT_RANGE "Content-Range"
#define HTTP_HEADER_CONTENT_TYPE "Content-Type"
#define HTTP_HEADER_COOKIE "Cookie"
#define HTTP_HEADER_DATE "Date"
#define HTTP_HEADER_ETAG "ETag"
#define HTTP_HEADER_EXPECT "Expect"
#define HTTP_HEADER_EXPIRES "Expires"
#define HTTP_HEADER_FROM "From"
#define HTTP_HEADER_HOST "Host"
#define HTTP_HEADER_IF_MATCH "If-Match"
#define HTTP_HEADER_IF_MODIFIED_SINCE "If-Modified-Since"
#define HTTP_HEADER_IF_NONE_MATCH "If-None-Match"
#define HTTP_HEADER_IF_RANGE "If-Range"
#define HTTP_HEADER_IF_UNMODIFIED_SINCE "If-Unmodified-Since"
#define HTTP_HEADER_LAST_MODIFIED "Last-Modified"
#define HTTP_HEADER_LOCATION "Location"
#define HTTP_HEADER_MAX_FORWARDS "Max-Forwards"
#define HTTP_HEADER_PRAGMA "Pragma"
#define HTTP_HEADER_PROXY_AUTHENTICATE "Proxy-Authenticate"
#define HTTP_HEADER_PROXY_AUTHORIZATION "Proxy-Authorization"
#define HTTP_HEADER_RANGE "Range"
/* This is not a typo, see HTTP spec */
#define HTTP_HEADER_REFERER "Referer"
#define HTTP_HEADER_RETRY_AFTER "Retry-After"
#define HTTP_HEADER_SERVER "Server"
#define HTTP_HEADER_SET_COOKIE "Set-Cookie"
#define HTTP_HEADER_SET_COOKIE2 "Set-Cookie2"
#define HTTP_HEADER_TE "TE"
#define HTTP_HEADER_TRAILER "Trailer"
#define HTTP_HEADER_TRANSFER_ENCODING "Transfer-Encoding"
#define HTTP_HEADER_UPGRADE "Upgrade"
#define HTTP_HEADER_USER_AGENT "User-Agent"
#define HTTP_HEADER_VARY "Vary"
#define HTTP_HEADER_VIA "Via"
#define HTTP_HEADER_WARNING "Warning"
#define HTTP_HEADER_WWW_AUTHENTICATE "WWW-Authenticate"
#define HTTP_HEADER_ACCESS_CONTROL_ALLOW_ORIGIN "Access-Control-Allow-Origin"
#define HTTP_HEADER_CONTENT_DISPOSITION "Content-Disposition"

/** @} */ /* end of group headers */

/**
 * @defgroup versions HTTP versions
 * These strings should be used to match against the first line of the
 * HTTP header.
 * @{
 */
#define HTTP_VERSION_1_0 "HTTP/1.0"
#define HTTP_VERSION_1_1 "HTTP/1.1"

/** @} */ /* end of group versions */

/**
 * @defgroup methods HTTP methods
 * Standard HTTP methods (as strings).
 * @{
 */
#define HTTP_METHOD_CONNECT "CONNECT"
#define HTTP_METHOD_DELETE "DELETE"
#define HTTP_METHOD_GET "GET"
#define HTTP_METHOD_HEAD "HEAD"
#define HTTP_METHOD_OPTIONS "OPTIONS"
#define HTTP_METHOD_POST "POST"
#define HTTP_METHOD_PUT "PUT"
#define HTTP_METHOD_PATCH "PATCH"
#define HTTP_METHOD_TRACE "TRACE"

/** @} */ /* end of group methods */

/**
 * @defgroup postenc HTTP POST encodings
 * See also: http://www.w3.org/TR/html4/interact/forms.html#h-17.13.4
 * @{
 */
#define HTTP_POST_ENCODING_FORM_URLENCODED "application/x-www-form-urlencoded"
#define HTTP_POST_ENCODING_MULTIPART_FORMDATA "multipart/form-data"



#endif
