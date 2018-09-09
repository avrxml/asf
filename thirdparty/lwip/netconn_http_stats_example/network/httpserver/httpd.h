/**
 * \file
 *
 * \brief Httpd server.
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
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
