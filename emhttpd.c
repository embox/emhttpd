/**
 * @file
 * @brief Simple HTTP server
 * @date 16.04.12
 * @author Ilia Vaprol
 * @author Anton Kozlov
 * 	- CGI related changes
 * @author Andrey Golikov
 * 	- Linux adaptation
 */

#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include "emhttpd.h"
#include "config.h"

static char httpd_g_inbuf[BUFF_SZ];


static int httpd_read_http_header(int sk, char *buf, size_t buf_sz) {
	const char pattern[] = "\r\n\r\n";
	char *pb;
	int offset;
	int header_len;

	if (buf_sz <  sizeof(pattern)) {
		return -EINVAL;
	}

	if (0 > read(sk, buf, 4)) {
		return -errno;
	}

	pb = buf + 4;

	for (header_len = 4; header_len < buf_sz; header_len++) {
		if (0 == memcmp(pb - 4, pattern, 4)) {
			break;
		}
		if (0 > read(sk, pb, 1)) {
			return -errno;
		}
		pb++;
	}

	*pb = '\0';

	return header_len;
}

int httpd_build_request(struct client_info *cinfo, struct http_req *hreq, char *buf, size_t buf_sz) {
	int nbyte;

	nbyte = httpd_read_http_header(cinfo->ci_sock, buf, buf_sz - 1);
	if (nbyte < 0) {
		httpd_error("can't read from client socket: %s", strerror(errno));
		return -errno;
	}

	memset(hreq, 0, sizeof(*hreq));
	if (NULL == httpd_parse_request(buf, hreq)) {
		httpd_error("can't parse request");
		return -EINVAL;
	}

	return nbyte;
}

static int httpd_header(const struct client_info *cinfo, const char *msg, int msg_size) {
	int cbyte;
	char buf[1024];
	int buf_sz = sizeof(buf);

	cbyte = snprintf(buf, buf_sz,
			"HTTP/1.1 %d %s\r\n"
			"Content-Type: %s\r\n"
			"Connection: close\r\n"
			"\r\n",
			200, "", "text/plain");

	if (0 > write(cinfo->ci_sock, buf, cbyte)) {
		return -errno;
	}

	if (0 > write(cinfo->ci_sock, msg, msg_size)) {
		return -errno;
	}

	return 0;
}

static void httpd_client_process(struct client_info *cinfo) {
	struct http_req hreq;
	pid_t cgi_child;
	int err;

	if (0 > (err = httpd_build_request(cinfo, &hreq, httpd_g_inbuf, sizeof(httpd_g_inbuf)))) {
		httpd_error("can't build request: %s", strerror(-err));
	}

	httpd_debug("method=%s uri_target=%s uri_query=%s",
			   hreq.method, hreq.uri.target, hreq.uri.query);


	httpd_header(cinfo, "!!!!!!", sizeof("!!!!!!"));
}

int main(int argc, char **argv) {
	int host;
	const char *basedir;

	struct sockaddr_in inaddr;
	const int family = AF_INET;

	inaddr.sin_family = AF_INET;
	inaddr.sin_port= htons(80);
	inaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	basedir = argc > 1 ? argv[1] : "/";

	host = socket(family, SOCK_STREAM, IPPROTO_TCP);
	if (host == -1) {
		httpd_error("socket() failure: %s", strerror(errno));
		return -errno;
	}

	if (-1 == bind(host, (struct sockaddr *) &inaddr, sizeof(inaddr))) {
		httpd_error("bind() failure: %s", strerror(errno));
		close(host);
		return -errno;
	}

	if (-1 == listen(host, 1)) {
		httpd_error("listen() failure: %s", strerror(errno));
		close(host);
		return -errno;
	}

	while (1) {
		struct client_info ci;

		ci.ci_basedir = basedir;
		ci.ci_addrlen = sizeof(inaddr);
		ci.ci_sock = accept(host, &ci.ci_addr, &ci.ci_addrlen);
		if (ci.ci_sock == -1) {
			if (errno != EINTR) {
				httpd_error("accept() failure: %s", strerror(errno));
				usleep(100000);
			}
			continue;
		}

		httpd_client_process(&ci);

		close(ci.ci_sock);
	}

	close(host);

	return 0;
}
