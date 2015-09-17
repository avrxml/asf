/**
 * \file
 *
 * \brief Httpd server.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef __HTTPD_H__
#define __HTTPD_H__

/** HTTP content type enum*/
enum {
	HTTP_CONTENT_JSON = 0,
	HTTP_CONTENT_HTML,
	HTTP_CONTENT_CSS,
	HTTP_CONTENT_JS,
	HTTP_CONTENT_PNG,
	HTTP_CONTENT_JPEG,
	HTTP_CONTENT_GIF,
	HTTP_CONTENT_PLAIN,
	HTTP_CONTENT_CNT
};

typedef int (*http_handler_t)(const char *name, char *revc_buf,
		size_t revc_len);

#define HTTP_DEFAULT_PAGE "index.html"

/** FIFO tx buffer. */
#define CONFIG_AFSK_TX_BUFLEN                   1024
#define CGI_LED_ID_KEY    "n"
#define CGI_LED_CMD_KEY   "set"

#define CGI_MATCH_NONE   0
/** Select item in table only if string match */
#define CGI_MATCH_WORD   1
/** Select item in table if the extention match */
#define CGI_MATCH_EXT    2
/** Select item in table if the string is content */
#define CGI_MATCH_NAME   3

typedef struct HttpCGI {
	/** Strategy to find string in the CGI table. */
	unsigned type; 
	/** Request string from GET request */
	const char *name; 
	/** Callback to process the special request */
	http_handler_t handler; 
} HttpCGI;


void httpd_init(void);

#endif /* __HTTPD_H__ */
