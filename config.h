/*
 * config.h
 *
 *  Created on: Apr 13, 2020
 *      Author: anton
 */

#ifndef CONFIG_H_
#define CONFIG_H_


#ifdef __EMBUILD_MOD__
#	include <framework/mod/options.h>
#	define USE_IP_VER       OPTION_GET(NUMBER,use_ip_ver)
#	define USE_CGI          OPTION_GET(BOOLEAN,use_cgi)
#	define USE_REAL_CMD     OPTION_GET(BOOLEAN,use_real_cmd)
#	define USE_PARALLEL_CGI OPTION_GET(BOOLEAN,use_parallel_cgi)
#endif /* __EMBUILD_MOD__ */

#define BUFF_SZ     1024

#endif /* CONFIG_H_ */
