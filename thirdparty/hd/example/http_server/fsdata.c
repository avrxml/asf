/*! \page License
 * Copyright (C) 2009, H&D Wireless AB All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of H&D Wireless AB may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY H&D WIRELESS AB ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <compiler.h>
#include "board.h"
#include "gpio.h"
#include "adc.h"

#include "lwip/stats.h"
#include "lwip/init.h"
#include "lwip/netif.h"

#include <net/fs.h>
#include <util/util.h>
#include <net/hdwireless_gif.h>

/**
 *
 */
static char*
get_adc(void)
{
        volatile avr32_adc_t *adc = &AVR32_ADC;
        static char buf[8];
        signed short val;
        adc_start(adc);
        val = adc_get_value(adc, 0);

        sniprintf(buf, sizeof(buf), "%d", val);
        return buf;
}


/**
 *
 */
static char*
get_tx(void)
{
        static char buf[8];
        sniprintf(buf, sizeof(buf), "%d", lwip_stats.link.xmit);
        return buf;
}


/**
 *
 */
static char*
get_rx(void)
{
        static char buf[8];
        sniprintf(buf, sizeof(buf), "%d", lwip_stats.link.recv);
        return buf;
}


/**
 *
 */
static int
html_add_row(char* buf, int len, char* name, const char* data)
{
        return sniprintf(buf, len, "<tr><td>%s: </td><td><b>%s</b></td></tr>",
                        name, data);
}


/**
 *
 */
static int
html_add_button(char* buf, int len, char* id, char* value)
{
        return sniprintf(buf, len, "<input type=\"submit\" "
                         "style=\"height: 50px\" "
                         "name=\"%s\" "
			 "value=\"%s\"/>&nbsp&nbsp",
			 id, value);
}


/**
 *
 */
void
document_image_gif_cb(const char** buf, int* len, char *arg)
{
	*buf = hdwireless_gif;
        *len = sizeof(hdwireless_gif);
}


/**
 *
 */
void
document_404_html_cb(const char** buf, int* len, char *arg)
{
	static const char *rsp =
		"HTTP/1.0 404 File not found\n"
		"Server: lwIP\n"
		"Content-type: text/html\n\n"
		"<html>"
		"<head><title>802.11 WiFi on Atmel UC3A3256</title></head>"
		"<body bgcolor=\"white\" text=\"black\">"
		"<table width=\"100%\">"
		"<tr valign=\"top\"><td width=\"80\">"
		"<a href=\"http://www.atmel.com/\"><img src=\"image.gif\""
		"border=\"0\" alt=\"logo\" title=\"logo\"></a>"
		"</td><td width=\"500\">"
		"<h2>802.11 WiFi on Atmel UC3A3256</h2>"
		"<h3>404 - Page not found</h3>"
		"<p>"
		"Sorry, the page you are requesting was not found on this"
		"server."
		"</p></td><td>&nbsp;</td></tr></table></body></html>";

	*buf = rsp;
        *len = strlen(rsp);
}


/**
 *
 */
void
document_index_html_cb(const char** buf, int* len, char *arg)
{
        static char rsp[1620];
        int pos = 0;

        pos += sniprintf(rsp + pos, sizeof(rsp) - pos,
                         "HTTP/1.0 200 OK\n"
                         "Server: lwIP\n"
//                         "Refresh: 5;URL=http://%s/index.html\n"
                         "Content-type: text/html\n\n"
                         "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Frameset//EN\" \"http://www.w3.org/TR/html4/frameset.dtd\">"
                         "<html>"
                         "<head><title>802.11bg WiFi on Atmel UC3%s</title>"
                         "</head>"
                         "</html>"
                         "<body bgcolor=\"white\" text=\"black\">"
                         "<table width=\"100%%\">"
                         "<tr valign=\"top\"><td width=\"80\">"
                         "<a href=\"index.html\">"
                         "<img src=\"image.gif\" border=\"0\" alt=\"logo\""
                         "title=\"logo\"></a>"
                         "</td>"
                         "<td width=\"500\">"
                         "<h2>802.11bg WiFi on Atmel UC3%s</h2>"
                         "<p>"

                         "The web page is served by an Atmel UC3%s MCU, using the H&D Wireless&rsquo; "
                         "unique Wi-Fi solution, consisting of the HDG104 WLAN SiP and the "
                         "Software SDK, oWL&copy;. The web server is running on top of the "
                         "lwIP TCP/IP stack. "

                         "</p><hr><table>",
//                         ip2str(netif_default->ip_addr),
#if BOARD == EVK1104
			 "A3256", "A3256", "A3256");
#elif BOARD == EVK1105 || BOARD == EVK1100
	                 "A0512", "A0512", "A0512");
#elif BOARD == EVK1101
                        "B0256", "B0256", "B0256");
#else
                        "N/A", "N/A", "N/A");
#endif


        pos += html_add_row(rsp + pos, sizeof(rsp) - pos,
			    "IP Address", ip2str(netif_default->ip_addr));
        pos += html_add_row(rsp + pos, sizeof(rsp) - pos,
			    "MAC Address", mac2str(netif_default->hwaddr));
        pos += html_add_row(rsp + pos, sizeof(rsp) - pos,
			    "TX packets", get_tx());
        pos += html_add_row(rsp + pos, sizeof(rsp) - pos,
			    "RX packets", get_rx());
        pos += html_add_row(rsp + pos, sizeof(rsp) - pos,
#if BOARD == EVK1104 || BOARD == EVK1101
			    "Light sensor ADC value",
#elif BOARD == EVK1105 || BOARD == EVK1100
			    "ADC value",
#else
                            "N/A",
#endif
                            get_adc());

        pos += sniprintf(rsp + pos, sizeof(rsp) - pos,
                         "<tr><td><form method=\"get\">"
                         "<input type=\"submit\" style=\"height: 50px\" value=\"Refresh\"/>"
                        "</form></td></tr>");


        pos += sniprintf(rsp + pos, sizeof(rsp) - pos,
                        "</table><br>");
        pos += sniprintf(rsp + pos, sizeof(rsp) - pos, "<form id=\"form\""
                        "name=\"form\" method=\"get\" action=\"index.html\">"
                        "<fieldset style=\"width: 300;\">"
                        "<legend>LED control</legend>");
        pos += html_add_button(rsp + pos, sizeof(rsp) - pos, "toggle", "LED0");
        pos += html_add_button(rsp + pos, sizeof(rsp) - pos, "toggle", "LED1");
        pos += html_add_button(rsp + pos, sizeof(rsp) - pos, "toggle", "LED2");
        pos += html_add_button(rsp + pos, sizeof(rsp) - pos, "toggle", "LED3");
        pos += sniprintf(rsp + pos, sizeof(rsp) - pos, "</fieldset></form>");

        pos += sniprintf(rsp + pos, sizeof(rsp) - pos, "</td>"
                        "<td>&nbsp;</td></tr></table></body></html>");

        if (pos >= sizeof rsp) {
                printk("Overflow %d\n", pos);
        }
        buf[sizeof buf] = '\0';
        *buf = rsp;
        *len = pos;
}


/**
 *
 */
void
query_led0_cb(const char** buf, int* len, char *arg)
{
        document_index_html_cb(buf, len, arg);
        LED_Toggle(LED0);
}


/**
 *
 */
void
query_led1_cb(const char** buf, int* len, char *arg)
{
        document_index_html_cb(buf, len, arg);
        LED_Toggle(LED1);
}


/**
 *
 */
void
query_led2_cb(const char** buf, int* len, char *arg)
{
        document_index_html_cb(buf, len, arg);
        LED_Toggle(LED2);
}


/**
 *
 */
void
query_led3_cb(const char** buf, int* len, char *arg)
{
        document_index_html_cb(buf, len, arg);
        LED_Toggle(LED3);
}

static const struct fsdata_file file_led3 = {
	"/index.html?toggle=LED3",
	query_led3_cb,
	NULL
};

static const struct fsdata_file file_led2 = {
	"/index.html?toggle=LED2",
	query_led2_cb,
	&file_led3,
};

static const struct fsdata_file file_led1 = {
	"/index.html?toggle=LED1",
	query_led1_cb,
	&file_led2,
};

static const struct fsdata_file file_led0 = {
	"/index.html?toggle=LED0",
	query_led0_cb,
	&file_led1,
};


static const struct fsdata_file file_image_gif = {
	"/image.gif",
	document_image_gif_cb,
	&file_led0,
};

static const struct fsdata_file file_404_html = {
	"/404.html",
	document_404_html_cb,
	&file_image_gif,
};

static const struct fsdata_file file_index_html = {
	"/index.html",
	document_index_html_cb,
	&file_404_html,
};

void fsdata_app_enable(void) {
        fs_register_root(&file_index_html,
                         NULL);
}
