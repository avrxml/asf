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

#include <string.h>
#include "lwip/opt.h"
#include "lwip/tcpip.h"
#include "fs.h"
#include "httpd.h"
#include "cgi.h"

static struct { const char *key; const char *content; }
http_content_type[] = {
	{"", "Content-type: application/json\r\n\r\n"},
	{"html", "Content-type: text/html\r\n\r\n"},
	{"css", "Content-type: text/css\r\n\r\n"},
	{"js", "Content-type: text/javascript\r\n\r\n"},
	{"png", "Content-type: image/png\r\n\r\n"},
	{"jpg", "Content-type: image/jpeg\r\n\r\n"},
	{"gif", "Content-type: image/gif\r\n\r\n"},
	{"txt", "Content-type: text/plain\r\n\r\n"},
};

#define HTTP_DEFAULT_PAGE "index.html"

static const char http_html_hdr_200[] = "HTTP/1.0 200 OK\r\n";
static const char http_html_hdr_404[] = "HTTP/1.0 404 Not Found\r\n";
static const char http_html_hdr_500[] = "HTTP/1.0 500 Internal Server Error\r\n";

const char http_server_error[] = " \
<!DOCTYPE html PUBLIC \"-//IETF//DTD HTML 2.0//EN\"> \
<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\"> \
<title>500 Internal Server error</title></head><body><h1>500 Internal Server error</h1> \
<p>The internal server error was occur while processing the requested page.</p> \
</body></html>";

BoardStatus status;

/**
 * \brief Send the HTTP header with the appropriated content type.
 *
 * \param client Socket to send data to.
 * \param content_type Index in the http_content_type table.
 */
void http_sendOk(struct netconn *client, int content_type)
{
	/* ASSERT(content_type < HTTP_CONTENT_CNT); */

 	netconn_write(client, http_html_hdr_200, sizeof(http_html_hdr_200) - 1, NETCONN_COPY);
	netconn_write(client, http_content_type[content_type].content,
			strlen(http_content_type[content_type].content), NETCONN_COPY);
}

/**
 * \brief Tokenize a buffer.
 *
 * \param raw_buf Buffer to parse.
 * \param raw_len Buffer length.
 *
 * \return The number of found tokens.
 */
int http_tokenizeGetRequest(char *raw_buf, size_t raw_len)
{
	size_t token = 0;

	for (size_t i = 0; (i < raw_len) && raw_buf; i++) {
		if (raw_buf[i] == '&') {
			token++;
			raw_buf[i] = '\0';
		}

		if (raw_buf[i] == '=') {
			raw_buf[i] = '\0';
		}
	}

	return token + 1;
}

/**
 * \brief ASCII to HEX converter.
 *
 * \param first First digit.
 * \param second Second digit.
 *
 * \return String result.
 */
static char http_hexToAscii(char first, char second)
{
	char hex[5];
	char *stop;

	hex[0] = '0';
	hex[1] = 'x';
	hex[2] = first;
	hex[3] = second;
	hex[4] = 0;

	return strtol(hex, &stop, 16);
}

/**
 * \brief Decode URL.
 *
 * \param raw_buf Input buffer.
 * \param raw_len Input buffer length.
 * \param decodec_buf Output decoded buffer.
 * \param len Output buffer length.
 */
static void http_decodeUrl(const char *raw_buf, size_t raw_len,
		char *decodec_buf, size_t len)
{
	char value;

	/* ASSERT(decodec_buf); */
	memset(decodec_buf, 0, len);

	for (size_t i = 0; i < raw_len; i++) {
		if (!len) {
			return;
		}

		if (raw_buf[i] == '%') {
			if (i + 2 < raw_len) {
				/* Convert HEX value after %. */
				value
					= http_hexToAscii(raw_buf[i + 1],
						raw_buf[i + 2]);
				if (value) {
					*decodec_buf++ = value;
					len--;
					/* Decoded two digit of hex value, go to next value. */
					i += 2;
					continue;
				}
			}
		}

		/* Manage special case of '+', that should be converted to spaces. */
		*decodec_buf++ = (raw_buf[i] == '+' ? ' ' : raw_buf[i]);
		len--;
	}
}

/**
 * \brief Get key value from tokenized buffer.
 */
int http_getValue(char *tolenized_buf, size_t tolenized_buf_len,
		const char *key, char *value, size_t len)
{
	char decoded_str[80];

	if (!tolenized_buf || !key || !value) {
		return -1;
	}

	char *p = tolenized_buf;
	size_t value_len = 0;

	memset(value, 0, len);

	for (size_t i = 0; i < tolenized_buf_len; i++) {
		size_t token_len = strlen(p);
		http_decodeUrl(p, token_len, decoded_str, sizeof(decoded_str));

		if (!strcmp(key, decoded_str)) {
			/* skip key */
			p += token_len + 1;

			http_decodeUrl(p, strlen(p), decoded_str,
					sizeof(decoded_str));
			value_len = strlen(decoded_str);

			if (value_len >= len) {
				return -1;
			}

			strcpy(value, decoded_str);
			return value_len;
		}

		/* jump to next pair */
		p += token_len + 1;
	}

	return -1;
}

/**
 * \brief Send HTTP internal server error code.
 */
void http_sendInternalErr(struct netconn *client, int content_type)
{
	/*	ASSERT(content_type < HTTP_CONTENT_CNT); */

	netconn_write(client, http_html_hdr_500, sizeof(http_html_hdr_500) - 1, NETCONN_COPY);
	netconn_write(client, http_content_type[content_type].content,
			strlen(http_content_type[content_type].content), NETCONN_COPY);
}

/**
 * \brief Get requested page name.
 *
 * \param recv_buf Receive buffer.
 * \param recv_len Receive buffer length.
 * \param page_name Output buffer to store requestd page name.
 * \param len Output buffer length.
 */
static void http_getPageName(const char *recv_buf, size_t recv_len,
		char *page_name, size_t len)
{
	int i = 0;
	int str_ok = 0;
	const char *p = recv_buf;
	if (p && (recv_len > sizeof("GET /"))) {
		if (*p++ == 'G' && *p++ == 'E' && *p++ == 'T') {
			str_ok = 1;
			/* skip the space and "/" */
			p += 2;
		}
	}

	if (str_ok) {
		while ((size_t)i < recv_len) {
			char ch = *(p++);
			if (ch == ' ' || ch == '\t' || ch == '\n') {
				break;
			}

			if ((size_t)i == len - 1) {
				break;
			}

			page_name[i++] = ch;
		}
	}

	page_name[i] = '\0';
}

/**
 * \brief Get filename extension type.
 *
 * \param name String containing a filename.
 *
 * \return Extension type on success, NULL otherwise.
 */
const char *get_ext(const char *name)
{
	const char *ext = strstr(name, ".");
	if (ext && (ext + 1)) {
		return (ext + 1);
	}

	return NULL;
}

/**
 * \brief Match content type based on filename extension.
 *
 * \param name String containing a filename.
 *
 * \return Content type index on success, 0 otherwise.
 */
static int http_searchContentType(const char *name)
{
	if (!name) {
		return 0;
	}

	const char *ext = get_ext(name);

	if (!ext) {
		return 0;
	}

	if (!strcmp(ext, "ico")) {
		return HTTP_CONTENT_JPEG;
	}

	for (int i = 0; i < HTTP_CONTENT_CNT; i++) {
		if (!strcmp(ext, http_content_type[i].key)) {
			return i;
		}
	}

	return 0;
}

/**
 * \brief Core HTTP server function processing the request.
 *
 * \param pvParameters Netconn socket to use.
 */
void http_request(void *pvParameters)
{
	char req_string[80];
	struct netconn *conn;
	struct netbuf *inbuf;
	char *buf;
	u16_t buflen;
	struct fs_file file;
	http_handler_t cgi;
	uint32_t i;

	conn = pvParameters;

	/* Read the data from the port, blocking if nothing yet there. */
	/* We assume the request is in one netbuf. */
	if (ERR_OK == netconn_recv(conn, &inbuf)) {
		/* Read data from netbuf to the provided buffer. */
		netbuf_data(inbuf, (void**)&buf, &buflen);

		memset(req_string, 0, sizeof(req_string));
		http_getPageName(buf, buflen, req_string, sizeof(req_string));

		if (req_string[0] == '\0') {
			strcpy(req_string, HTTP_DEFAULT_PAGE);
		}

		/* Try to get a CGI handler for the request. */
		cgi = cgi_search(req_string, cgi_table);
		if (cgi) {
			/* Answer CGI request. */
			if (cgi(conn, req_string, buf, buflen) < 0) {
				http_sendInternalErr(conn, HTTP_CONTENT_HTML);
				netconn_write(conn, http_server_error, sizeof(http_server_error) - 1, NETCONN_COPY);
			}
		}
		/* Normal HTTP page request. */
		else {
			if (fs_open(req_string, &file) == 0) {
				netconn_write(conn, http_html_hdr_404, sizeof(http_html_hdr_404) - 1, NETCONN_COPY);
			}
			else {
				/* Send the HTML header for file type. */
				int type = http_searchContentType(req_string);
				http_sendOk(conn, type);

				/* Send the HTML content. */
				/* Transfer speed test: wget.exe http://192.168.0.100/test */
				if (req_string[0] == 't' && req_string[1] == 'e' && req_string[2] == 's' && req_string[3] == 't') {
				  	for (i = 0; i < 1000; ++i) {
						netconn_write(conn, file.data, file.len, NETCONN_COPY);
					}
				}
				else {
					netconn_write(conn, file.data, file.len, NETCONN_COPY);
				}
			}
		}
	}

	/* Close the connection (server closes in HTTP). */
	netconn_close(conn);

	/* Delete the buffer (netconn_recv gives us ownership, */
	/* so we have to make sure to deallocate the buffer). */
	netbuf_delete(inbuf);

	/* Free resource. */
	netconn_delete(conn);

	/* Delete the calling task. */
	vTaskDelete(NULL);
}
