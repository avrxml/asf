/*
 * Copyright (c) 2002, Adam Dunkels.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution. 
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.  
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef __WEBSOCKET_HTTP_CLIENT_H__
#define __WEBSOCKET_HTTP_CLIENT_H__

#include "contiki-net.h"
#include "http-strings.h"

#ifndef WEBSOCKET_CONF_MAX_URLLEN
#define WEBSOCKET_CONF_MAX_URLLEN 80
#endif /* WEBSOCKET_CONF_MAX_URLLEN */

#ifndef WEBSOCKET_CONF_MAX_SUBPROTOLEN
#define WEBSOCKET_CONF_MAX_SUBPROTOLEN 30
#endif /* WEBSOCKET_CONF_MAX_SUBPROTOLEN */

#ifdef WEBSOCKET_HTTP_CLIENT_CONF_BUFSIZE
#define WEBSOCKET_HTTP_CLIENT_BUFSIZE WEBSOCKET_HTTP_CLIENT_CONF_BUFSIZE
#else /* WEBSOCKET_HTTP_CLIENT_CONF_BUFSIZE */
#define WEBSOCKET_HTTP_CLIENT_BUFSIZE 400
#endif /* WEBSOCKET_HTTP_CLIENT_CONF_BUFSIZE */

struct websocket_http_client_state {
  struct uip_conn *conn;
  uint8_t timer;
  uint8_t state;
  uint8_t httpflag;

  char host[WEBSOCKET_CONF_MAX_URLLEN];
  uint16_t port;
  char file[WEBSOCKET_CONF_MAX_URLLEN];
  char subprotocol[WEBSOCKET_CONF_MAX_SUBPROTOLEN];

  uint16_t getrequestptr;
  uint16_t getrequestleft;

  uint8_t inputbuf[WEBSOCKET_HTTP_CLIENT_BUFSIZE];
  uint16_t inputbufptr;

  uint8_t outputbuf[WEBSOCKET_HTTP_CLIENT_BUFSIZE];
  uint16_t outputbufptr, outputbuf_sendnext;

  char mimetype[32];
};

/* Functions. */
void websocket_http_client_init(void);
unsigned char websocket_http_client_get(struct websocket_http_client_state *s,
					const char *host,
					uint16_t port,
					const char *file,
                                        const char *subprotocol);
uint16_t websocket_http_client_send(struct websocket_http_client_state *s,
				    const uint8_t *data,
				    uint16_t datalen);
uint16_t websocket_http_client_sendbuflen(struct websocket_http_client_state *s);

void websocket_http_client_close(struct websocket_http_client_state *s);

void websocket_http_client_appcall(void *state);

char *websocket_http_client_mimetype(struct websocket_http_client_state *s);
char *websocket_http_client_filename(struct websocket_http_client_state *s);
char *websocket_http_client_hostname(struct websocket_http_client_state *s);
unsigned short websocket_http_client_port(struct websocket_http_client_state *s);


/* Callback functions that have to be implemented by the application
   program. */
void websocket_http_client_datahandler(struct websocket_http_client_state *s,
				       uint8_t *data, uint16_t len);
void websocket_http_client_connected(struct websocket_http_client_state *s);
void websocket_http_client_timedout(struct websocket_http_client_state *s);
void websocket_http_client_aborted(struct websocket_http_client_state *s);
void websocket_http_client_closed(struct websocket_http_client_state *s);


#endif /* __WEBSOCKET_HTTP_CLIENT_H__ */
