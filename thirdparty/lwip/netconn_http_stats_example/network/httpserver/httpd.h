/**
 * \file
 *
 * \brief Httpd server.
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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

typedef int (*http_handler_t)(struct netconn *client,
		const char *name,
		char *revc_buf,
		size_t revc_len);

typedef struct BoardStatus {
	char local_ip[sizeof("123.123.123.123")];
	char last_connected_ip[sizeof("123.123.123.123")];
	uint32_t up_time;
	uint32_t tot_req;
} BoardStatus;

extern BoardStatus status;

void http_sendOk(struct netconn *client, int content_type);
void http_sendInternalErr(struct netconn *client, int content_type);
int http_tokenizeGetRequest(char *raw_buf, size_t raw_len);
int http_getValue(char *tolenized_buf, size_t tolenized_buf_len,
		const char *key, char *value, size_t len);
const char *get_ext(const char *name);
void http_request(void *pvParameters);

#endif /* __HTTPD_H__ */
