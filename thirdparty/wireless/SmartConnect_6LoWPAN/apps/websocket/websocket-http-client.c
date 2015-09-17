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

#include <string.h>
#include <stdio.h>

#include "contiki-net.h"

#include "ip64-addr.h"
#include "mdns.h"
#include "websocket-http-client.h"

#define WEBSOCKET_HTTP_CLIENT_TIMEOUT 100

#define WEBSOCKET_HTTP_CLIENT_STATE_STATUSLINE 0
#define WEBSOCKET_HTTP_CLIENT_STATE_HEADERS    1
#define WEBSOCKET_HTTP_CLIENT_STATE_DATA       2
#define WEBSOCKET_HTTP_CLIENT_STATE_CLOSE      3

#define HTTPFLAG_NONE   0
#define HTTPFLAG_OK     1
#define HTTPFLAG_MOVED  2
#define HTTPFLAG_ERROR  3


#define ISO_nl       0x0a
#define ISO_cr       0x0d
#define ISO_space    0x20

#define MIN(a, b) ((a) < (b) ? (a) : (b))

/*-----------------------------------------------------------------------------------*/
char *
websocket_http_client_mimetype(struct websocket_http_client_state *s)
{
  return s->mimetype;
}
/*-----------------------------------------------------------------------------------*/
char *
websocket_http_client_filename(struct websocket_http_client_state *s)
{
  return s->file;
}
/*-----------------------------------------------------------------------------------*/
char *
websocket_http_client_hostname(struct websocket_http_client_state *s)
{
  return s->host;
}
/*-----------------------------------------------------------------------------------*/
unsigned short
websocket_http_client_port(struct websocket_http_client_state *s)
{
  return s->port;
}
/*-----------------------------------------------------------------------------------*/
void
websocket_http_client_init(void)
{

}
/*-----------------------------------------------------------------------------------*/
static void
init_connection(struct websocket_http_client_state *s)
{
  char portnumber[10];
  s->state = WEBSOCKET_HTTP_CLIENT_STATE_STATUSLINE;

  sprintf(portnumber, ":%d", s->port);
  s->getrequestleft = sizeof(http_get) - 1 + 1 +
    sizeof(http_10) - 1 +
    sizeof(http_crnl) - 1 +
    sizeof(http_host) - 1 +
    sizeof(http_crnl) - 1 +
    (uint16_t)strlen(http_request_fields) +
    (uint16_t)strlen(s->subprotocol) +
    (uint16_t)strlen(http_crnl) +
    (uint16_t)strlen(http_crnl) +
    (uint16_t)strlen(s->file) + (uint16_t)strlen(s->host) +
    (uint16_t)strlen(portnumber);
  s->getrequestptr = 0;

  s->inputbufptr = 0;
}
/*-----------------------------------------------------------------------------------*/
void
websocket_http_client_close(struct websocket_http_client_state *s)
{
  s->state = WEBSOCKET_HTTP_CLIENT_STATE_CLOSE;
}
/*-----------------------------------------------------------------------------------*/
unsigned char
websocket_http_client_get(struct websocket_http_client_state *s,
			  const char *host, uint16_t port, const char *file,
                          const char *subprotocol)
{
  struct uip_conn *conn;
  uip_ip6addr_t ip6addr;
  uip_ip4addr_t ip4addr;
  uip_ipaddr_t *ipaddr;

  ipaddr = &ip6addr;
  /* First check if the host is an IP address. */
  if(uiplib_ip6addrconv(host, &ip6addr) == 0) {
    if(uiplib_ip4addrconv(host, &ip4addr) != 0) {
      ip64_addr_4to6(&ip4addr, &ip6addr);
    } else {
#if UIP_UDP
      ipaddr = mdns_lookup(host);
      
      if(ipaddr == NULL) {
	return 0;
      }
#else /* UIP_UDP */
      return 0;
#endif /* UIP_UDP */
    }
  }
  
  conn = tcp_connect(ipaddr, uip_htons(port), NULL);
  
  if(conn == NULL) {
    return 0;
  }
  tcp_markconn(conn, s);
  s->conn = conn;
  s->port = port;
  strncpy(s->file, file, sizeof(s->file));
  strncpy(s->host, host, sizeof(s->host));
  strncpy(s->subprotocol, subprotocol, sizeof(s->subprotocol));
  init_connection(s);
  return 1;
}
/*-----------------------------------------------------------------------------------*/
/* Copy data into a "window", specified by the windowstart and
   windowend variables. Only data that fits within the window is
   copied. This function is used to copy data into the uIP buffer, which
   typically is smaller than the data that is to be copied.
*/
static unsigned char *windowptr;
static int windowstart, windowend;
static int
window_copy(int curptr, const char *data, unsigned char datalen)
{
  int len;

  if(windowstart == windowend) {
    return curptr + datalen;
  }
  
  if(curptr + datalen < windowstart) {
    /* If all the data is before the window, we do not copy the
       data. */
    return curptr + datalen;
  }

  if(curptr > windowend) {
    /* If all the data is after the window, we do not copy the data. */
    return curptr + datalen;
  }

  len = datalen;
  
  /* Trim off data before the window. */
  data += windowstart - curptr;
  len  -= windowstart - curptr;

  /* Trim off data after the window. */
  if(len > windowend - windowstart) {
    len = windowend - windowstart;
  }

  strncpy((char *)windowptr + windowstart, data, len);
  windowstart += len;

  return curptr + datalen;
}
/*-----------------------------------------------------------------------------------*/
static void
senddata(struct websocket_http_client_state *s)
{
  uint16_t len;
  int curptr;
  char portnumber[10];

  if(s->getrequestleft > 0) {
    windowstart = s->getrequestptr;
    curptr = 0;
    windowend = windowstart + uip_mss();
    windowptr = (uint8_t *)uip_appdata - windowstart;

    curptr = window_copy(curptr, http_get, sizeof(http_get) - 1);
    curptr = window_copy(curptr, s->file, (unsigned char)strlen(s->file));
    curptr = window_copy(curptr, " ", 1);
    curptr = window_copy(curptr, http_10, sizeof(http_10) - 1);

    curptr = window_copy(curptr, http_crnl, sizeof(http_crnl) - 1);
    
    curptr = window_copy(curptr, http_host, sizeof(http_host) - 1);
    curptr = window_copy(curptr, s->host, (unsigned char)strlen(s->host));
    sprintf(portnumber, ":%d", s->port);
    curptr = window_copy(curptr, portnumber, (unsigned char)strlen(portnumber));
    curptr = window_copy(curptr, http_crnl, sizeof(http_crnl) - 1);

    curptr = window_copy(curptr, http_request_fields,
			 (unsigned char)strlen(http_request_fields));

    curptr = window_copy(curptr, s->subprotocol,
			 (unsigned char)strlen(s->subprotocol));

    curptr = window_copy(curptr, http_crnl, strlen(http_crnl));
    curptr = window_copy(curptr, http_crnl, strlen(http_crnl));
    len = s->getrequestleft > uip_mss()?
      uip_mss():
      s->getrequestleft;
    uip_send(uip_appdata, len);
  } else if(s->outputbufptr > 0 && s->outputbuf_sendnext != 0) {
    /* Retransmit previously sent data */
    uip_send(s->outputbuf, s->outputbuf_sendnext);
  } else if(s->outputbufptr > 0 && s->outputbuf_sendnext == 0) {
    len = MIN(s->outputbufptr, uip_mss());
    s->outputbuf_sendnext = len;
    uip_send(s->outputbuf, len);
    /*    printf("Data on outputbuf to send, %d bytes, sending %d\n", s->outputbufptr, len);*/
  }
}
/*-----------------------------------------------------------------------------------*/
static void
acked(struct websocket_http_client_state *s)
{
  uint16_t len;

  /* This function gets called when data we previously have sent has
   * been acknowledged by the receiver. We need to compute how much
   * data we previously sent and remove this data from the output
   * buffer. */

  /* We first check if we have pieces of the original GET request left
   * to send. */
  if(s->getrequestleft > 0) {
    len = s->getrequestleft > uip_mss()?
      uip_mss():
      s->getrequestleft;
    s->getrequestleft -= len;
    s->getrequestptr += len;

    /* If the entire GET request is sent, we call the user to let it
     * know that we now are connected. */
    if(s->getrequestleft == 0) {
      websocket_http_client_connected(s);
    }
  } else if(s->outputbuf_sendnext > 0) {

    /* If we have previously sent data that now is acknowledged, we
     * should remove this data from the output buffer, and update the
     * output buffer pointers accordingly. */

    memcpy(&s->outputbuf[0],
           &s->outputbuf[s->outputbuf_sendnext],
           s->outputbufptr - s->outputbuf_sendnext);

    s->outputbufptr -= s->outputbuf_sendnext;
    s->outputbuf_sendnext = 0;
  }
}
/*-----------------------------------------------------------------------------------*/
static uint16_t
parse_statusline(struct websocket_http_client_state *s, uint16_t len)
{
  char *cptr;
  
  while(len > 0 && s->inputbufptr < sizeof(s->inputbuf)) {
    s->inputbuf[s->inputbufptr] = *(char *)uip_appdata;
    uip_appdata = (char *)uip_appdata + 1;
    --len;
    if(s->inputbuf[s->inputbufptr] == ISO_nl) {

      if((strncmp((char *)s->inputbuf, http_10,
		  sizeof(http_10) - 1) == 0) ||
	 (strncmp((char *)s->inputbuf, http_11,
		  sizeof(http_11) - 1) == 0)) {
	cptr = (char *)&(s->inputbuf[9]);
	s->httpflag = HTTPFLAG_NONE;
	if(strncmp(cptr, http_200, sizeof(http_200) - 1) == 0) {
	  /* 200 OK */
	  s->httpflag = HTTPFLAG_OK;
	} else if(strncmp(cptr, http_301, sizeof(http_301) - 1) == 0 ||
		  strncmp(cptr, http_302, sizeof(http_302) - 1) == 0) {
	  /* 301 Moved permanently or 302 Found. Location: header line
	     will contain thw new location. */
	  s->httpflag = HTTPFLAG_MOVED;
	} else {
	  s->inputbuf[s->inputbufptr - 1] = 0;
	}
      } else {
	uip_abort();
	websocket_http_client_aborted(s);
	return 0;
      }
      
      /* We're done parsing the status line, so we reset the pointer
	 and start parsing the HTTP headers.*/
      s->inputbufptr = 0;
      s->state = WEBSOCKET_HTTP_CLIENT_STATE_HEADERS;
      break;
    } else {
      ++s->inputbufptr;
    }
  }
  return len;
}
/*-----------------------------------------------------------------------------------*/
static char
casecmp(char *str1, const char *str2, char len)
{
  static char c;
  
  while(len > 0) {
    c = *str1;
    /* Force lower-case characters. */
    if(c & 0x40) {
      c |= 0x20;
    }
    if(*str2 != c) {
      return 1;
    }
    ++str1;
    ++str2;
    --len;
  }
  return 0;
}
/*-----------------------------------------------------------------------------------*/
static uint16_t
parse_headers(struct websocket_http_client_state *s, uint16_t len)
{
  char *cptr;
  static unsigned char i;
  
  while(len > 0 && s->inputbufptr < sizeof(s->inputbuf)) {
    s->inputbuf[s->inputbufptr] = *(char *)uip_appdata;
    uip_appdata = (char *)uip_appdata + 1;
    --len;
    if(s->inputbuf[s->inputbufptr] == ISO_nl) {
      /* We have an entire HTTP header line in s->inputbuf, so
	 we parse it. */
      if(s->inputbuf[0] == ISO_cr) {
	/* This was the last header line (i.e., and empty "\r\n"), so
	   we are done with the headers and proceed with the actual
	   data. */
	s->state = WEBSOCKET_HTTP_CLIENT_STATE_DATA;
	return len;
      }

      s->inputbuf[s->inputbufptr - 1] = 0;
      /* Check for specific HTTP header fields. */
      if(casecmp((char *)s->inputbuf, http_content_type,
		 sizeof(http_content_type) - 1) == 0) {
	/* Found Content-type field. */
	cptr = strchr((char *)s->inputbuf, ';');
	if(cptr != NULL) {
	  *cptr = 0;
	}
	strncpy(s->mimetype, (char *)s->inputbuf +
		sizeof(http_content_type) - 1, sizeof(s->mimetype));
      } else if(casecmp((char *)s->inputbuf, http_location,
			sizeof(http_location) - 1) == 0) {
	cptr = (char *)s->inputbuf +
	  sizeof(http_location) - 1;
	
	if(strncmp(cptr, http_http, 7) == 0) {
	  cptr += 7;
	  for(i = 0; i < s->inputbufptr - 7; ++i) {
	    if(*cptr == 0 ||
	       *cptr == '/' ||
	       *cptr == ' ' ||
	       *cptr == ':') {
	      s->host[i] = 0;
	      break;
	    }
	    s->host[i] = *cptr;
	    ++cptr;
	  }
	}
	strncpy(s->file, cptr, sizeof(s->file));
	/*	s->file[s->inputbufptr - i] = 0;*/
      }


      /* We're done parsing, so we reset the pointer and start the
	 next line. */
      s->inputbufptr = 0;
    } else {
      ++s->inputbufptr;
    }
  }
  return len;
}
/*-----------------------------------------------------------------------------------*/
static void
newdata(struct websocket_http_client_state *s)
{
  uint16_t len;

  len = uip_datalen();

  if(s->state == WEBSOCKET_HTTP_CLIENT_STATE_STATUSLINE) {
    len = parse_statusline(s, len);
  }

  if(s->state == WEBSOCKET_HTTP_CLIENT_STATE_HEADERS && len > 0) {
    len = parse_headers(s, len);
  }

  if(len > 0 && s->state == WEBSOCKET_HTTP_CLIENT_STATE_DATA &&
     s->httpflag != HTTPFLAG_MOVED) {
    websocket_http_client_datahandler(s, (uint8_t *)uip_appdata, len);
  }
}
/*-----------------------------------------------------------------------------------*/
uint16_t
websocket_http_client_sendbuflen(struct websocket_http_client_state *s)
{
  return sizeof(s->outputbuf) - s->outputbufptr;
}
/*-----------------------------------------------------------------------------------*/
uint16_t
websocket_http_client_send(struct websocket_http_client_state *s,
			   const uint8_t *data, uint16_t datalen)
{
  int len;

  len = MIN(datalen, sizeof(s->outputbuf) - s->outputbufptr);
  if(len != datalen) {
    /* Could not send, return the length of the available data buffer
       instead. */
    printf("websocket_http_client_send: could not send %d bytes, only %d available\n",
	   datalen, len);
    return len;
  }
  /*  printf("Putting %d bytes of data on outputbuf, ptr is %d\n", len, s->outputbufptr);*/
  memcpy(&s->outputbuf[s->outputbufptr], data, len);
  s->outputbufptr += len;
  tcpip_poll_tcp(s->conn);
  return len;
}
/*-----------------------------------------------------------------------------------*/
void
websocket_http_client_appcall(void *state)
{
  char *dataptr;
  struct websocket_http_client_state *s = state;
  
  if(uip_connected()) {
    s->timer = 0;
    s->outputbufptr = 0;
    s->outputbuf_sendnext = 0;
    s->state = WEBSOCKET_HTTP_CLIENT_STATE_STATUSLINE;
    senddata(s);
    return;
  }

  if(uip_timedout()) {
    websocket_http_client_timedout(s);
  }

  if(uip_aborted()) {
    /*    printf("aborted\n"); */
    websocket_http_client_aborted(s);
  }

  if(state == NULL) {
    uip_abort();
    return;
  }

  if(s->state == WEBSOCKET_HTTP_CLIENT_STATE_CLOSE) {
    websocket_http_client_closed(s);
    uip_abort();
    return;
  }


  /* The acked() and newdata() functions may alter the uip_appdata
     ptr, so we need to store it in the "dataptr" variable so that we
     can restore it before the senddata() function is called. */  
  dataptr = uip_appdata;
  
  if(uip_acked()) {
    s->timer = 0;
    acked(s);
  }
  if(uip_newdata()) {
    s->timer = 0;
    newdata(s);
  }

  uip_appdata = dataptr;
  
  if(uip_rexmit() ||
     uip_newdata() ||
     uip_acked()) {
    senddata(s);
  } else if(uip_poll()) {
    senddata(s);
    ++s->timer;
    if(s->timer == WEBSOCKET_HTTP_CLIENT_TIMEOUT) {
      websocket_http_client_timedout(s);
      uip_abort();
      return;
    }
  }

  if(uip_closed() || uip_aborted() || uip_timedout()) {
    tcp_markconn(uip_conn, NULL);
    if(s->httpflag != HTTPFLAG_MOVED) {
      /* Send NULL data to signal EOF. */
      websocket_http_client_datahandler(s, NULL, 0);
    } else {
#if UIP_UDP
      if(mdns_lookup(s->host) == NULL) {
	mdns_query(s->host);
      }
#endif /* UIP_UDP */
      websocket_http_client_get(s, s->host, s->port, s->file,
                                s->subprotocol);
    }
  }
}
/*-----------------------------------------------------------------------------------*/
