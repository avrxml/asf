/*
 * Copyright (c) 2004, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: httpd.c,v 1.20 2010/12/14 22:45:22 dak664 Exp $
 */
 
#include <stdio.h>
#include <string.h>

#include "contiki-net.h"

#include "httpd-fs.h"
#include "httpd-cgi.h"
#include "lib/petsciiconv.h"
#include "ip64-webserver-http-strings.h"

#include "httpd.h"
#include "ledctrl.h"



uint8_t nbr_id,nbr_action;

#ifndef WEBSERVER_CONF_CGI_CONNS
#define CONNS UIP_CONNS
#else /* WEBSERVER_CONF_CGI_CONNS */
#define CONNS WEBSERVER_CONF_CGI_CONNS
#endif /* WEBSERVER_CONF_CGI_CONNS */

#define STATE_WAITING 0
#define STATE_OUTPUT  1

#define SEND_STRING(s, str) PSOCK_SEND(s, (uint8_t *)str, (unsigned int)strlen(str))
MEMB(conns, struct httpd_state, CONNS);

#define ISO_nl      0x0a
#define ISO_space   0x20
#define ISO_bang    0x21
#define ISO_percent 0x25
#define ISO_ampersand 0x26
#define ISO_question 0x3F
#define ISO_period  0x2e
#define ISO_slash   0x2f
#define ISO_colon   0x3a

struct udp_socket led_socket;

void led_socket_callback(struct udp_socket *c,
                          void *ptr,
                          const uip_ipaddr_t *source_addr,
                          uint16_t source_port,
                          const uip_ipaddr_t *dest_addr,
                          uint16_t dest_port,
                          const uint8_t *data,
                          uint16_t datalen);



void led_socket_callback(struct udp_socket *c,
                          void *ptr,
                          const uip_ipaddr_t *source_addr,
                          uint16_t source_port,
                          const uip_ipaddr_t *dest_addr,
                          uint16_t dest_port,
                          const uint8_t *data,
                          uint16_t datalen)
{
  //
  // place holder
  //
}


int simple_udp_ping_send_ping(const uip_ipaddr_t *addr);

/*---------------------------------------------------------------------------*/
static unsigned short
generate(void *state)
{
  struct httpd_state *s = (struct httpd_state *)state;

  if(s->file.len > uip_mss()) {
    s->len = uip_mss();
  } else {
    s->len = s->file.len;
  }
  memcpy(uip_appdata, s->file.data, s->len);
  
  return s->len;
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(send_file(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);
  
  do {
    PSOCK_GENERATOR_SEND(&s->sout, generate, s);
    s->file.len -= s->len;
    s->file.data += s->len;
  } while(s->file.len > 0);
      
  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(send_part_of_file(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);

  PSOCK_SEND(&s->sout, (uint8_t *)s->file.data, s->len);
  
  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
static void
next_scriptstate(struct httpd_state *s)
{
  char *p;

  if((p = strchr(s->scriptptr, ISO_nl)) != NULL) {
    p += 1;
    s->scriptlen -= (unsigned short)(p - s->scriptptr);
    s->scriptptr = p;
  } else {
    s->scriptlen = 0;
  }
  /*  char *p;
  p = strchr(s->scriptptr, ISO_nl) + 1;
  s->scriptlen -= (unsigned short)(p - s->scriptptr);
  s->scriptptr = p;*/
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(handle_script(struct httpd_state *s))
{
  char *ptr;
  
  PT_BEGIN(&s->scriptpt);

  while(s->file.len > 0) {

    /* Check if we should start executing a script. */
    if(*s->file.data == ISO_percent &&
       *(s->file.data + 1) == ISO_bang) {
      s->scriptptr = s->file.data + 3;
      s->scriptlen = s->file.len - 3;
      if(*(s->scriptptr - 1) == ISO_colon) {
	httpd_fs_open(s->scriptptr + 1, &s->file);
	PT_WAIT_THREAD(&s->scriptpt, send_file(s));
      } else {
	PT_WAIT_THREAD(&s->scriptpt,
		       httpd_cgi(s->scriptptr)(s, s->scriptptr));
      }
      next_scriptstate(s);
      
      /* The script is over, so we reset the pointers and continue
	 sending the rest of the file. */
      s->file.data = s->scriptptr;
      s->file.len = s->scriptlen;
    } else {
      /* See if we find the start of script marker in the block of HTML
	 to be sent. */

      if(s->file.len > uip_mss()) {
        s->len = uip_mss();
      } else {
        s->len = s->file.len;
      }

      if(*s->file.data == ISO_percent) {
        ptr = strchr(s->file.data + 1, ISO_percent);
      } else {
        ptr = strchr(s->file.data, ISO_percent);
      }
      if(ptr != NULL &&
          ptr != s->file.data) {
        s->len = (int)(ptr - s->file.data);
        if(s->len >= uip_mss()) {
          s->len = uip_mss();
        }
        if(s->len >= s->file.len) {
          s->len = s->file.len;
        }
      }
      PT_WAIT_THREAD(&s->scriptpt, send_part_of_file(s));
      s->file.data += s->len;
      s->file.len -= s->len;
    }
  }

  PT_END(&s->scriptpt);
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(send_headers(struct httpd_state *s, const char *statushdr))
{
/* gcc warning if not initialized. 
 * If initialized, minimal-net platform segmentation fault if not static...
 */
  static const char *ptr = NULL;

  PSOCK_BEGIN(&s->sout);

  SEND_STRING(&s->sout, statushdr);

  ptr = strrchr(s->filename, ISO_period);
  if(ptr == NULL) {
    ptr = ip64_webserver_http_content_type_binary;
  } else if(strncmp(ip64_webserver_http_html, ptr, 5) == 0 ||
	    strncmp(ip64_webserver_http_shtml, ptr, 6) == 0) {
    ptr = ip64_webserver_http_content_type_html;
  } else if(strncmp(ip64_webserver_http_css, ptr, 4) == 0) {
    ptr = ip64_webserver_http_content_type_css;
  } else if(strncmp(ip64_webserver_http_png, ptr, 4) == 0) {
    ptr = ip64_webserver_http_content_type_png;
  } else if(strncmp(ip64_webserver_http_gif, ptr, 4) == 0) {
    ptr = ip64_webserver_http_content_type_gif;
  } else if(strncmp(ip64_webserver_http_jpg, ptr, 4) == 0) {
    ptr = ip64_webserver_http_content_type_jpg;
  } else {
    ptr = ip64_webserver_http_content_type_plain;
  }
  SEND_STRING(&s->sout, ptr);
  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(handle_output(struct httpd_state *s))
{
  char *ptr;
  
  PT_BEGIN(&s->outputpt);
 
  if(!httpd_fs_open(s->filename, &s->file)) {
    strcpy(s->filename, ip64_webserver_http_404_html);
    httpd_fs_open(s->filename, &s->file);
    PT_WAIT_THREAD(&s->outputpt,
		   send_headers(s,
		   ip64_webserver_http_header_404));
    PT_WAIT_THREAD(&s->outputpt,
		   send_file(s));
  } else {
    PT_WAIT_THREAD(&s->outputpt,
		   send_headers(s,
		   ip64_webserver_http_header_200));
    ptr = strrchr(s->filename, ISO_period);
    if(ptr != NULL && strncmp(ptr, ip64_webserver_http_shtml, 6) == 0) {
      PT_INIT(&s->scriptpt);
      PT_WAIT_THREAD(&s->outputpt, handle_script(s));
    } else {
      PT_WAIT_THREAD(&s->outputpt,
		     send_file(s));
    }
  }
  PSOCK_CLOSE(&s->sout);
  PT_END(&s->outputpt);
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(handle_input(struct httpd_state *s))
{
  char *ptr ,*ptr2;
   void *uptr;
   const uip_ipaddr_t *addr = NULL;
  
  PSOCK_BEGIN(&s->sin);

  PSOCK_READTO(&s->sin, ISO_space);
  
  if(strncmp(s->inputbuf, ip64_webserver_http_get, 4) != 0) {
    PSOCK_CLOSE_EXIT(&s->sin);
  }
  PSOCK_READTO(&s->sin, ISO_space);

  if(s->inputbuf[0] != ISO_slash) {
    PSOCK_CLOSE_EXIT(&s->sin);
  }

  if(s->inputbuf[1] == ISO_space) {
    strncpy(s->filename, ip64_webserver_http_index_html, sizeof(s->filename));
  } else {
    s->inputbuf[PSOCK_DATALEN(&s->sin) - 1] = 0;
    strncpy(s->filename, s->inputbuf, sizeof(s->filename));
  }
   ptr = strrchr(s->filename, ISO_question);
   if(ptr != NULL)
   {
	   
	   uint8_t k;
	   char nbrid_s[3] = {0};
	   uint8_t	databuf[1] = {0};
	   
	   if(!(strncmp(ptr+1,"nbraction",9)))
	   {
       char* tailptr = NULL;
		   ptr2 = strchr(ptr+1,ISO_ampersand);
		   k=ptr2-ptr-11; //k=4 Ping ,k=6 On,k=7 Off, k=9 Blink

	   	 printf("\n\r Webserver cmd, k = %d", k);
		   strncpy(nbrid_s,ptr2+7,3);           
		   nbr_id = strtoul(nbrid_s, &tailptr, 10);
     
		   if (*tailptr != '\0') {
           nbr_id = 0;
       }         
       printf("\n\r nbr id is : %d\r\n",nbr_id);  
       // todoo for nbr count more than 9
		  
		   uptr = nbr_table_head(ds6_neighbors);
		   for(uint8_t i=1;i<nbr_id;i++)
		   {
			   uptr = nbr_table_next(ds6_neighbors,uptr);
		   }
		   if(uptr!=NULL)
		   {				
			    uip_ds6_nbr_t *nbr= uptr;
			    addr = uip_ds6_nbr_get_ipaddr(nbr);				
		    }		
			
		  if(addr != NULL) {
          printf("\r\n Target node addr = %2X%2X ", addr->u8[14], addr->u8[15]);
          switch (k) {
		      
            case 6:	// Led On
			        nbr_action = 0;		
				      databuf[0] = LED_UDP_ON;	
			        udp_socket_sendto(&led_socket, (void*)databuf, 1,addr, LED_UDP_PORT);
              printf("\n\r Sent LED On");
              break;
		    
		        case 7:	// Led Off
			        databuf[0] = LED_UDP_OFF;	
			        udp_socket_sendto(&led_socket, (void*)databuf, 1,addr, LED_UDP_PORT);
			        nbr_action = 1;
              printf("\n\r Sent LED off");
              break;
            
            case 9:	// Led Blink
              databuf[0] = LED_UDP_BLINK;	
		          udp_socket_sendto(&led_socket, (void*)databuf, 1,addr, LED_UDP_PORT);
              nbr_action = 1;
              printf("\n\r Sent LED blink");
		          break;
        
	        default:
		        nbr_action = 2;				
		        simple_udp_ping_send_ping(addr);
        		printf("\n\r Sent udp ping");
	      }			
          
	    }
		  printf("\n\r nbr action is : %d\r\n",nbr_action);  
    
     }
   }
   
  petsciiconv_topetscii(s->filename, sizeof(s->filename));
  /*  webserver_log_file(&uip_conn->ripaddr, s->filename);*/
  petsciiconv_toascii(s->filename, sizeof(s->filename));
  
  s->state = STATE_OUTPUT;

  while(1) {
    PSOCK_READTO(&s->sin, ISO_nl);

    if(strncmp(s->inputbuf, ip64_webserver_http_referer, 8) == 0) {
      s->inputbuf[PSOCK_DATALEN(&s->sin) - 2] = 0;
      petsciiconv_topetscii(s->inputbuf, PSOCK_DATALEN(&s->sin) - 2);
      /*      webserver_log(s->inputbuf);*/
    }
  }
  
  PSOCK_END(&s->sin);
}

/*---------------------------------------------------------------------------*/
static void
handle_connection(struct httpd_state *s)
{
  handle_input(s);
  if(s->state == STATE_OUTPUT) {
    handle_output(s);
  }
}
/*---------------------------------------------------------------------------*/
void
httpd_appcall(void *state)
{
  struct httpd_state *s = (struct httpd_state *)state;

  if(uip_closed() || uip_aborted() || uip_timedout()) {
    if(s != NULL) {
      memb_free(&conns, s);
    }
  } else if(uip_connected()) {
    s = (struct httpd_state *)memb_alloc(&conns);
    if(s == NULL) {
      uip_abort();
      return;
    }
    tcp_markconn(uip_conn, s);
    PSOCK_INIT(&s->sin, (uint8_t *)s->inputbuf, sizeof(s->inputbuf) - 1);
    PSOCK_INIT(&s->sout, (uint8_t *)s->inputbuf, sizeof(s->inputbuf) - 1);
    PT_INIT(&s->outputpt);
    s->state = STATE_WAITING;
    /*    timer_set(&s->timer, CLOCK_SECOND * 100);*/
    s->timer = 0;
    handle_connection(s);
  } else if(s != NULL) {
    if(uip_poll()) {
      ++s->timer;
      if(s->timer >= 20) {
	uip_abort();
	memb_free(&conns, s);
      }
    } else {
      s->timer = 0;
    }
    handle_connection(s);
  } else {
    uip_abort();
  }
}


/*---------------------------------------------------------------------------*/
void
httpd_init(void)
{
  udp_socket_input_callback_t reply_callback = led_socket_callback;
  tcp_listen(UIP_HTONS(80));
  memb_init(&conns);
  httpd_cgi_init();
  udp_socket_register(&led_socket, 
                        NULL, 
                        reply_callback);
                        
  udp_socket_bind(&led_socket, LED_UDP_PORT);
                        
}
/*---------------------------------------------------------------------------*/
#if UIP_CONF_IPV6
uint8_t
httpd_sprint_ip6(const uip_ip6addr_t *addr, char *result)
{
  unsigned char i = 0;
  unsigned char zerocnt = 0;
  unsigned char numprinted = 0;
  char *starting = result;

  *result++ = '[';
  while(numprinted < 8) {
    if((addr->u16[i] == 0) && (zerocnt == 0)) {
      while(addr->u16[zerocnt + i] == 0) {
	zerocnt++;
      }
      if(zerocnt == 1) {
        *result++ = '0';
         numprinted++;
         break;
      }
      i += zerocnt;
      numprinted += zerocnt;
    } else {
      result += sprintf(result, "%x", (unsigned int)(uip_ntohs(addr->u16[i])));
      i++;
      numprinted++;
    }
    if(numprinted != 8) {
      *result++ = ':';
    }
  }
  *result++ = ']';
  *result = 0;
  return (result - starting);
}
#endif /* UIP_CONF_IPV6 */
/*---------------------------------------------------------------------------*/
uint8_t
httpd_sprint_ip4(const uip_ip4addr_t *addr, char *result)
{
  return sprintf(result, "%d.%d.%d.%d", uip_ipaddr_to_quad(addr));
}
/*---------------------------------------------------------------------------*/
