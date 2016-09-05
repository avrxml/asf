/**
 *
 * \file
 *
 * \brief AP Task.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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

#include "asf.h"
#include "FreeRTOS.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "osprintf.h"
#include "ap.h"
#include "sta.h"
#include <string.h>
#include <stdio.h>

/** Using broadcast address for simplicity. */
#define HTTP_PORT						(80)

/** Delay on close error. */
#define webSHORT_DELAY					(10)

/*! Standard GET response. */
#define webHTTP_OK	"HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n"

/*! Format of the dynamic page that is returned on each connection. */
#define webHTML_START \
"<html>\
<head>\
</head>\
<BODY onLoad=\"window.setTimeout(&quot;location.href='index.html'&quot;,3000)\" bgcolor=\"#FFFFFF\" text=\"#2477E6\">\
\r\n - Atmel WILC3000 STA/AP and BT Weather Demo - \
<p><pre><br>System Information:<br>Page Hits = "

#define webHTML_END \
"\r\n</pre>\
\r\n</font></BODY>\
</html>"

/** Webpage hit counter. */
static uint32_t webpage_hit = 0;

/** Placeholder to convert from int to ASCII. */
static char cwebpage_hit[11];

/** Webpage to send to connected STA. */
static char webpage[1024];

extern char report[];
extern uint32_t sta_connected;

/**
 * \brief Core HTTP server function processing the request.
 *
 * \param pvParameters Netconn socket to use.
 */
static void http_request(struct netconn *conn)
{
	struct netbuf *rx_buf;
	char *str;
	uint16_t len;

	while (netconn_recv(conn, &rx_buf) != ERR_OK) {
		vTaskDelay(webSHORT_DELAY);
	}

	if (rx_buf != NULL) {
		/* Where is the data? */
		netbuf_data(rx_buf, (void *)&str, &len);

		/* Is this a GET?  We don't handle anything else. */
		if ((NULL != str) && (!strncmp(str, "GET", 3 ))) {
			/* Write out the HTTP OK header. */
			netconn_write(conn, webHTTP_OK, (uint32_t)strlen(webHTTP_OK), NETCONN_COPY);

			/* Generate page header. */
			strcpy(webpage, webHTML_START);

			/* Append stats and task info. */
			webpage_hit++;
			sprintf(cwebpage_hit, "%d", (int)webpage_hit);
			strcat(webpage, cwebpage_hit);
			sprintf(cwebpage_hit, "%d", (int)sta_connected);
			strcat(webpage, "<br>Nb STA Connected = ");
			strcat(webpage, cwebpage_hit);	
			strcat(webpage, "<p><pre><br>Task          State  Priority  Stack	#<br>************************************************<br>");
			vTaskList((void *)((char *)webpage + strlen((char *)webpage)));
			strcat(webpage, "<p><pre><p><pre><br><br>Weather Bulletin (via AP ");
			strcat(webpage, STA_WLAN_SSID);
			strcat(webpage, ")<br>************************************************<br>");
			strcat(webpage, report);

			/* ... Finally the page footer. */
			strcat(webpage, webHTML_END);

			/* Write out the dynamically generated page. */
			netconn_write(conn, webpage, strlen(webpage), NETCONN_COPY);
		}
		netbuf_delete(rx_buf);
	}

	netconn_close(conn);
	netconn_delete(conn);
}

/**
 * \brief AP task main function.
 */
void ap_task(void *argument)
{
	struct netconn *conn, *newconn;

	/* Just to avoid compiler warnings. */
	UNUSED(argument);
	
	/* Give time for other thread to initialize lwIP. */
	vTaskDelay(1000);

	/* Create a new TCP connection handle */
	conn = netconn_new(NETCONN_TCP);
	if (conn == NULL) {
		osprintf("ap_task: failed to create socket!\n");

		/* Delete the calling task. */
		vTaskDelete(NULL);
	}

	/* Bind to port 80 (HTTP) with default IP address. */
	netconn_bind(conn, NULL, HTTP_PORT);

	/* Put the connection into LISTEN state. */
	netconn_listen(conn);

	/* Handle HTTP requests. */
	while (1) {
		while (netconn_accept(conn, &newconn) != ERR_OK) {
			vTaskDelay( webSHORT_DELAY );
		}

		if (newconn != NULL) {
			http_request(newconn);
		}
	}
}
