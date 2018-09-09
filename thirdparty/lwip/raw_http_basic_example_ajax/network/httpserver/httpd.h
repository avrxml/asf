/**
 * \file
 *
 * \brief Httpd server.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
