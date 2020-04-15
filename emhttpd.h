/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.04.2015
 */

#ifndef EMHTTPD_H_
#define EMHTTPD_H_

#include <sys/socket.h>

#include "config.h"

#include "emhttpd_log.h"

struct client_info {
	struct sockaddr ci_addr;
	socklen_t ci_addrlen;
	int ci_sock;
	int ci_index;

	const char *ci_basedir;
};


#endif /* EMHTTPD_H_ */


