/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.04.2015
 */

#ifndef EMHTTPD_H_
#define EMHTTPD_H_

#include <stdio.h>
#include <sys/socket.h>

#include "config.h"

#include "emhttpd_log.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) \
	(sizeof(array) / sizeof(*(array)))
#endif

#define HTTPD_MAX_PATH 128
#define CGI_PREFIX  "/cgi-bin/"

struct client_info {
	struct sockaddr ci_addr;
	socklen_t ci_addrlen;
	int ci_sock;
	int ci_index;

	const char *ci_basedir;
};

struct http_req_uri {
	char *target;
	char *query;
};

struct http_req {
	struct http_req_uri uri;
	char *method;
	char *content_len;
	char *content_type;
};

extern char *httpd_parse_request(char *str, struct http_req *hreq);

#endif /* EMHTTPD_H_ */


