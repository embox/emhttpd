/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.04.2015
 */

#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "emhttp_lib.h"

static void httpd_parse_uri(char *str, struct http_req_uri *huri) {
	char *pb;
	pb = str;

	huri->target = pb;

	pb = strchr(pb, '?');
	if (pb) {
		*(pb++) = '\0';
		huri->query = pb;
	} else {
		pb = huri->target;
		huri->query = NULL;
	}
}

static char *httpd_parse_request_line(char *str, struct http_req *hreq) {
	char *pb;
	char *next_line;
	char *uri;

	next_line = strstr(str, "\r\n");
	if (!next_line) {
		printf("can't find sentinel");
		return NULL;
	}

	*next_line = '\0';

	hreq->method = str;
	pb = strchr(str, ' ');
	if (!pb) {
		return NULL;
	}
	*(pb++) = '\0';

	uri = pb;
	pb = strchr(uri, ' ');
	if (!pb) {
		return NULL;
	}
	*pb = '\0';

	httpd_parse_uri(uri, &hreq->uri);

	return next_line + 2; /* "\r\n" */
}


char *http_parse_request(char *str, struct http_req *hreq) {
	char *pb;

	pb = httpd_parse_request_line(str, hreq);
	if (!pb) {
		printf("can't parse request line");
		return NULL;
	}

	return pb;
}

char *http_get_text_response_header(void) {
	return "HTTP/1.1 200\r\n"
			"Content-Type: text/plain\r\n"
			"Connection: close\r\n"
			"\r\n";
}
