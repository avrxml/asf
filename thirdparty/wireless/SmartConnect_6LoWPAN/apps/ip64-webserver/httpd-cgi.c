/*
 * Copyright (c) 2001, Adam Dunkels.
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
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: httpd-cgi.c,v 1.16 2010/10/19 18:29:03 adamdunkels Exp $
 *
 */

/*
 * This file includes functions that are called by the web server
 * scripts. The functions takes no argument, and the return value is
 * interpreted as follows. A zero means that the function did not
 * complete and should be invoked for the next packet as well. A
 * non-zero value indicates that the function has completed and that
 * the web server should move along to the next script line.
 *
 */

#include <stdio.h>
#include <string.h>

#include "contiki-net.h"
#include "httpd.h"
#include "httpd-cgi.h"
#include "httpd-fs.h"
#include "simple-rpl.h"

#include "net/rpl/rpl.h"
#include "net/rpl/rpl-private.h"

#include "lib/petsciiconv.h"

#include "simple-udp-ping.h"
#include "ip64-addrmap.h"

#include "network-reboot.h"

static struct httpd_cgi_call *calls = NULL;

static const char closed[] =    /*  "CLOSED", */
{ 0x43, 0x4c, 0x4f, 0x53, 0x45, 0x44, 0 };
static const char syn_rcvd[] =  /*  "SYN-RCVD", */
{ 0x53, 0x59, 0x4e, 0x2d, 0x52, 0x43, 0x56,
  0x44, 0
};
static const char syn_sent[] =  /*  "SYN-SENT", */
{ 0x53, 0x59, 0x4e, 0x2d, 0x53, 0x45, 0x4e,
  0x54, 0
};
static const char established[] =       /*  "ESTABLISHED", */
{ 0x45, 0x53, 0x54, 0x41, 0x42, 0x4c, 0x49,
  0x53, 0x48, 0x45, 0x44, 0
};
static const char fin_wait_1[] =        /*  "FIN-WAIT-1", */
{ 0x46, 0x49, 0x4e, 0x2d, 0x57, 0x41, 0x49,
  0x54, 0x2d, 0x31, 0
};
static const char fin_wait_2[] =        /*  "FIN-WAIT-2", */
{ 0x46, 0x49, 0x4e, 0x2d, 0x57, 0x41, 0x49,
  0x54, 0x2d, 0x32, 0
};
static const char closing[] =   /*  "CLOSING", */
{ 0x43, 0x4c, 0x4f, 0x53, 0x49,
  0x4e, 0x47, 0
};
static const char time_wait[] = /*  "TIME-WAIT," */
{ 0x54, 0x49, 0x4d, 0x45, 0x2d, 0x57, 0x41,
  0x49, 0x54, 0
};
static const char last_ack[] =  /*  "LAST-ACK" */
{ 0x4c, 0x41, 0x53, 0x54, 0x2d, 0x41, 0x43,
  0x4b, 0
};
static const char none[] =      /*  "NONE" */
{ 0x4e, 0x4f, 0x4e, 0x45, 0 };
static const char running[] =   /*  "RUNNING" */
{ 0x52, 0x55, 0x4e, 0x4e, 0x49, 0x4e, 0x47,
  0
};
static const char called[] =    /*  "CALLED" */
{ 0x43, 0x41, 0x4c, 0x4c, 0x45, 0x44, 0 };
static const char file_name[] = /*  "file-stats" */
{ 0x66, 0x69, 0x6c, 0x65, 0x2d, 0x73, 0x74,
  0x61, 0x74, 0x73, 0
};
static const char tcp_name[] =  /*  "tcp-connections" */
{ 0x74, 0x63, 0x70, 0x2d, 0x63, 0x6f, 0x6e,
  0x6e, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e,
  0x73, 0
};
static const char proc_name[] = /*  "processes" */
{ 0x70, 0x72, 0x6f, 0x63, 0x65, 0x73, 0x73,
  0x65, 0x73, 0
};

static const char *states[] = {
  closed,
  syn_rcvd,
  syn_sent,
  established,
  fin_wait_1,
  fin_wait_2,
  closing,
  time_wait,
  last_ack,
  none,
  running,
  called
};

/*---------------------------------------------------------------------------*/
static unsigned short
write_mss_error(int code)
{
  /* Error: uip_mss() is too small */
  /*printf("ERR#%d: increase UIP_CONF_TCP_MSS from %d\n", code, uip_mss());*/
  return snprintf((char *)uip_appdata, uip_mss(),
                  "ERR#%d: increase UIP_CONF_TCP_MSS from %d", code, uip_mss());
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(nullfunction(struct httpd_state *s, char *ptr))
{
  PSOCK_BEGIN(&s->sout);
  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
httpd_cgifunction
httpd_cgi(char *name)
{
  struct httpd_cgi_call *f;

  /* Find the matching name in the table, return the function. */
  for(f = calls; f != NULL; f = f->next) {
    if(strncmp(f->name, name, strlen(f->name)) == 0) {
      return f->function;
    }
  }
  return nullfunction;
}
/*---------------------------------------------------------------------------*/
static unsigned short
generate_file_stats(void *arg)
{
  char *f = (char *)arg;

  return snprintf((char *)uip_appdata, uip_mss(), "%5u", httpd_fs_count(f));
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(file_stats(struct httpd_state *s, char *ptr))
{
  PSOCK_BEGIN(&s->sout);

  PSOCK_GENERATOR_SEND(&s->sout, generate_file_stats,
                       (void *)(strchr(ptr, ' ') + 1));

  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
static unsigned short
make_tcp_stats(void *arg)
{
  struct uip_conn *conn;
  struct httpd_state *s = (struct httpd_state *)arg;

  conn = &uip_conns[s->u.count];

#if UIP_CONF_IPV6
  char buf[48];

  httpd_sprint_ip6(&conn->ripaddr, buf);
  return snprintf((char *)uip_appdata, uip_mss(),
                  "<tr><td>%d</td><td>%s:%u</td><td>%s</td><td>%u</td><td>%u</td><td>%c %c</td></tr>\r\n",
                  uip_htons(conn->lport),
                  buf,
                  uip_htons(conn->rport),
                  states[conn->tcpstateflags & UIP_TS_MASK],
                  conn->nrtx,
                  conn->timer,
                  (uip_outstanding(conn)) ? '*' : ' ',
                  (uip_stopped(conn)) ? '!' : ' ');
#else
  return snprintf((char *)uip_appdata, uip_mss(),
                  "<tr><td>%d</td><td>%u.%u.%u.%u:%u</td><td>%s</td><td>%u</td><td>%u</td><td>%c %c</td></tr>\r\n",
                  uip_htons(conn->lport),
                  conn->ripaddr.u8[0],
                  conn->ripaddr.u8[1],
                  conn->ripaddr.u8[2],
                  conn->ripaddr.u8[3],
                  uip_htons(conn->rport),
                  states[conn->tcpstateflags & UIP_TS_MASK],
                  conn->nrtx,
                  conn->timer,
                  (uip_outstanding(conn)) ? '*' : ' ',
                  (uip_stopped(conn)) ? '!' : ' ');
#endif /* UIP_CONF_IPV6 */
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(tcp_stats(struct httpd_state *s, char *ptr))
{

  PSOCK_BEGIN(&s->sout);

  for(s->u.count = 0; s->u.count < UIP_CONNS; ++s->u.count) {
    if((uip_conns[s->u.count].tcpstateflags & UIP_TS_MASK) != UIP_CLOSED) {
      PSOCK_GENERATOR_SEND(&s->sout, make_tcp_stats, s);
    }
  }

  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
static unsigned short
make_processes(void *p)
{
  char name[40];

  strncpy(name, PROCESS_NAME_STRING((struct process *)p), 40);
  petsciiconv_toascii(name, 40);

  return snprintf((char *)uip_appdata, uip_mss(),
                  "<tr><td>%p</td><td>%s</td><td>%p</td><td>%s</td></tr>\r\n",
                  p, name,
                  *((char **)&(((struct process *)p)->thread)),
                  states[9 + ((struct process *)p)->state]);
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(processes(struct httpd_state *s, char *ptr))
{
  PSOCK_BEGIN(&s->sout);
  for(s->u.ptr = PROCESS_LIST(); s->u.ptr != NULL;
      s->u.ptr = ((struct process *)s->u.ptr)->next) {
    PSOCK_GENERATOR_SEND(&s->sout, make_processes, s->u.ptr);
  }
  PSOCK_END(&s->sout);
}
#if UIP_CONF_IPV6
/* These cgi's are invoked by the status.shtml page in /apps/webserver/httpd-fs.
 * To keep the webserver build small that 160 byte page is not present in the
 * default httpd-fsdata.c file. Run the PERL script /../../tools/makefsdata from the
 * /apps/webserver/ directory to include it. Delete status.shtml before running
 * the script if you want to exclude it again.
 * NB: Webserver builds on all platforms will use the current httpd-fsdata.c file. The added 160 bytes
 * could overflow memory on the smaller platforms.
 */
/*---------------------------------------------------------------------------*/
#define HTTPD_STRING_ATTR
#define httpd_snprintf snprintf
#define httpd_cgi_sprint_ip6 httpd_sprint_ip6

static const char httpd_cgi_addrh[] HTTPD_STRING_ATTR = "<tr><td>";
static const char httpd_cgi_addrf[] HTTPD_STRING_ATTR =
  "<tr><td>Room for %u more</td></tr>";
static const char httpd_cgi_addrb[] HTTPD_STRING_ATTR = "</td></tr>";
static const char httpd_cgi_addrn[] HTTPD_STRING_ATTR = "(none)</td></tr>";
extern uip_ds6_nbr_t uip_ds6_nbr_cache[];
//extern uip_ds6_route_t uip_ds6_routing_table[];
extern uip_ds6_netif_t uip_ds6_if;

static unsigned short
make_addresses(void *p)
{
  int i, j = 0;
  uint16_t numprinted;

  numprinted =
    httpd_snprintf((char *)uip_appdata, uip_mss(), httpd_cgi_addrh);
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    if(uip_ds6_if.addr_list[i].isused) {
      j++;
      numprinted += httpd_cgi_sprint_ip6(&uip_ds6_if.addr_list[i].ipaddr,
                                         (char *)uip_appdata + numprinted);
      if(numprinted >= uip_mss()) {
        return write_mss_error(1);
      }
      numprinted +=
        httpd_snprintf((char *)uip_appdata + numprinted,
                       uip_mss() - numprinted, httpd_cgi_addrb);
      if(numprinted >= uip_mss()) {
        return write_mss_error(2);
      }
    }
  }
//if (j==0) numprinted += httpd_snprintf((char *)uip_appdata+numprinted, uip_mss()-numprinted, httpd_cgi_addrn);
  numprinted +=
    httpd_snprintf((char *)uip_appdata + numprinted, uip_mss() - numprinted,
                   httpd_cgi_addrf, UIP_DS6_ADDR_NB - j);
  if(numprinted >= uip_mss()) {
    return write_mss_error(3);
  }
  return numprinted;
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(addresses(struct httpd_state *s, char *ptr))
{
  PSOCK_BEGIN(&s->sout);

  PSOCK_GENERATOR_SEND(&s->sout, make_addresses, s->u.ptr);

  PSOCK_END(&s->sout);
}
static uint8_t nbr_no = 1;
/*---------------------------------------------------------------------------*/
static unsigned short
make_neighbor(void *p)
{
  uint16_t numprinted = 0;
  uip_ds6_nbr_t *nbr;
  
  struct httpd_state *s = (struct httpd_state *)p;

  nbr = s->u.ptr;
  /*  for(i = 0; i < UIP_DS6_NBR_NB; i++)*/ {
    /*    if(uip_ds6_nbr_cache[i].isused)*/ {
    
      numprinted +=
          httpd_snprintf((char *)uip_appdata, uip_mss(), httpd_cgi_addrh);
      if(numprinted >= uip_mss()) {
        return write_mss_error(4);
      }

      numprinted +=
          httpd_cgi_sprint_ip6(&nbr->ipaddr,
                               (char *)uip_appdata + numprinted);
      if(numprinted >= uip_mss()) {
        return write_mss_error(5);
      }

      const uip_lladdr_t *lladdr = uip_ds6_nbr_get_ll(nbr);
      if(lladdr != NULL) {
        uint16_t linkmetric = rpl_get_parent_link_metric(lladdr);
        uint32_t metricint, metricfrac;

        metricint = linkmetric / RPL_DAG_MC_ETX_DIVISOR;
        metricfrac = (linkmetric * 1000UL) / (uint32_t)RPL_DAG_MC_ETX_DIVISOR % 1000UL;
        numprinted += httpd_snprintf((char *)uip_appdata + numprinted,
                                     uip_mss() - numprinted,
                                     "</td><td>%lu.%03lu", metricint,
                                     metricfrac);
      } else {
        numprinted += httpd_snprintf((char *)uip_appdata + numprinted,
                                     uip_mss() - numprinted,
                                     "</td><td>-");

      }
      if(numprinted >= uip_mss()) {
        return write_mss_error(6);
      }

      
      if(simple_udp_ping_has_reply(&nbr->ipaddr)) {
        int delay = simple_udp_ping_get_delay(&nbr->ipaddr);
        numprinted +=
            httpd_snprintf((char *)uip_appdata + numprinted,
                           uip_mss() - numprinted, "</td><td>%u ms", delay);
        if(numprinted >= uip_mss()) {
          return write_mss_error(6);
        }
      } else if(simple_udp_ping_has_sent(&nbr->ipaddr)) {
        numprinted +=
            httpd_snprintf((char *)uip_appdata + numprinted,
                           uip_mss() - numprinted, "</td><td>Ping scheduled...");
        if(numprinted >= uip_mss()) {
          return write_mss_error(7);
        }
      } else {
        numprinted +=
            httpd_snprintf((char *)uip_appdata + numprinted,
                           uip_mss() - numprinted, "</td><td>-");
        if(numprinted >= uip_mss()) {
          return write_mss_error(8);
        }
      }
		numprinted += httpd_snprintf((char *)uip_appdata + numprinted,
                                     uip_mss() - numprinted,
                                     "</td><td><input type=\"radio\"  name=\"nbrid\" value=\"%d\">",nbr_no++);
      numprinted +=
          httpd_snprintf((char *)uip_appdata + numprinted,
                         uip_mss() - numprinted, httpd_cgi_addrb);
      if(numprinted >= uip_mss()) {
        return write_mss_error(9);
      }
    }
  }

  return numprinted;
}
/*---------------------------------------------------------------------------*/
static unsigned short
make_neighbor_roomfor(void *p)
{
  static const char httpd_cgi_room2[] HTTPD_STRING_ATTR =
    "<tr><td colspan=4>Room for %u more</td></tr>";
 static int j = 0;
  uint16_t numprinted = 0;
  uip_ds6_nbr_t *nbr;

  for(j = 0,
        nbr = nbr_table_head(ds6_neighbors);
      nbr != NULL;
      j++,
        nbr = nbr_table_next(ds6_neighbors, nbr));
  /*  for(i = 0; i < UIP_DS6_NBR_NB; i++) {
    if(uip_ds6_nbr_cache[i].isused) {
      j++;
    }
    }*/

  numprinted += httpd_snprintf((char *)uip_appdata + numprinted,
       uip_mss() - numprinted,
       httpd_cgi_room2, NBR_TABLE_CONF_MAX_NEIGHBORS - j);
  if(numprinted >= uip_mss()) {
    return write_mss_error(10);
  }

  return numprinted;
}

/*---------------------------------------------------------------------------*/
static unsigned short
make_led_toggle(void *p)
{
  static const char httpd_cgi_room2[] HTTPD_STRING_ATTR =
    "<tr><td colspan=4>LED Blink Command Sent to All Neighbors!!!</td></tr>";
  uint16_t numprinted = 0;

  numprinted += httpd_snprintf((char *)uip_appdata + numprinted,
       uip_mss() - numprinted,
       httpd_cgi_room2);
  if(numprinted >= uip_mss()) {
    return write_mss_error(10);
  }

  return numprinted;
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(neighbors(struct httpd_state *s, char *ptr))
{
  PSOCK_BEGIN(&s->sout);
  printf("\n\r PT_THREAD, neighbors");
  for(s->u.ptr = nbr_table_head(ds6_neighbors),nbr_no=1;
      s->u.ptr != NULL;
      s->u.ptr = nbr_table_next(ds6_neighbors, s->u.ptr)) {
    PSOCK_GENERATOR_SEND(&s->sout, make_neighbor, s);
  }

  PSOCK_GENERATOR_SEND(&s->sout, make_neighbor_roomfor, s);
  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(neighborsping(struct httpd_state *s, char *ptr))
{
  PSOCK_BEGIN(&s->sout);
  printf("\n\r PT_THREAD, neighborsping");
  /* Ping nodes */
  for(s->u.ptr = nbr_table_head(ds6_neighbors),nbr_no=1;
      s->u.ptr != NULL;
      s->u.ptr = nbr_table_next(ds6_neighbors, s->u.ptr)) {
    const uip_ipaddr_t *addr;
    uip_ds6_nbr_t *nbr= s->u.ptr;
    addr = uip_ds6_nbr_get_ipaddr(nbr);
    if(addr != NULL) {
      simple_udp_ping_send_ping(addr);
    }
    PSOCK_GENERATOR_SEND(&s->sout, make_neighbor, s);
  }

  PSOCK_END(&s->sout);
}

/*---------------------------------------------------------------------------*/
static
PT_THREAD(neighborsled(struct httpd_state *s, char *ptr))
{
    uint8_t databuf[1] = {0};
	  PSOCK_BEGIN(&s->sout);
    printf("\n\r PT_THREAD, neighborsled");
	  /* Ping nodes */
	  for(s->u.ptr = nbr_table_head(ds6_neighbors),nbr_no=1;
	    s->u.ptr != NULL;
	    s->u.ptr = nbr_table_next(ds6_neighbors, s->u.ptr)) {
		  const uip_ipaddr_t *addr;
		  uip_ds6_nbr_t *nbr= s->u.ptr;
		  addr = uip_ds6_nbr_get_ipaddr(nbr);
		  if(addr != NULL) { // to be modified
        databuf[0] = 03;
        printf("LED Blink cmd to all neighbors. Data = %d", databuf[0]);
			  udp_socket_sendto(&led_socket, (void*) databuf, 1,addr, LED_UDP_PORT);			
		  }
		  PSOCK_GENERATOR_SEND(&s->sout, make_neighbor, s);
		  
	  }
		PSOCK_GENERATOR_SEND(&s->sout, make_led_toggle, s);

		PSOCK_END(&s->sout);
	    
}
/*---------------------------------------------------------------------------*/
static unsigned short
make_route(void *p)
{
  static const char httpd_cgi_rtesh[] HTTPD_STRING_ATTR = " ";
  static const char httpd_cgi_rtes0[] HTTPD_STRING_ATTR = "<tr><td>";
  static const char httpd_cgi_rtes2[] HTTPD_STRING_ATTR = ", %lu s";
  uint16_t numprinted;
  struct httpd_state *s = (struct httpd_state *)p;
  uip_ds6_route_t *r;

  //  conn = &uip_conns[s->u.count];

  numprinted =
    httpd_snprintf((char *)uip_appdata, uip_mss(), httpd_cgi_rtesh);
  if(numprinted >= uip_mss()) {
    return write_mss_error(11);
  }
  {
    r = s->u.ptr;

    /* <tr><td> */
    numprinted += httpd_snprintf((char *)uip_appdata + numprinted,
				 uip_mss() - numprinted,
				 httpd_cgi_rtes0);
    if(numprinted >= uip_mss()) {
      return write_mss_error(12);
    }
    numprinted += httpd_cgi_sprint_ip6(&r->ipaddr,
				       (char *)uip_appdata + numprinted);
    numprinted += httpd_snprintf((char *)uip_appdata + numprinted,
                                 uip_mss() - numprinted,
                                 "</td><td>");
    if(numprinted >= uip_mss()) {
      return write_mss_error(13);
    }


    uip_ipaddr_t *nexthop;

    nexthop = uip_ds6_route_nexthop(r);
    if(nexthop != NULL) {
      numprinted += httpd_cgi_sprint_ip6(nexthop,
                                         (char *)uip_appdata + numprinted);
    }
    if(numprinted >= uip_mss()) {
      return write_mss_error(15);
    }
    if(r->state.lifetime < 3600L * 24) {
      numprinted += httpd_snprintf((char *)uip_appdata + numprinted,
                                   uip_mss() - numprinted,
                                   httpd_cgi_rtes2,
                                   r->state.lifetime);
      if(numprinted >= uip_mss()) {
        return write_mss_error(16);
      }
    }

    const uip_lladdr_t *lladdr = uip_ds6_nbr_lladdr_from_ipaddr(nexthop);
    if(lladdr != NULL) {
      uint16_t linkmetric = rpl_get_parent_link_metric(lladdr);
      uint32_t metricint, metricfrac;

      metricint = linkmetric / RPL_DAG_MC_ETX_DIVISOR;
      metricfrac = (linkmetric * 1000UL) / (uint32_t)RPL_DAG_MC_ETX_DIVISOR % 1000UL;
      numprinted += httpd_snprintf((char *)uip_appdata + numprinted,
                                   uip_mss() - numprinted,
                                   "</td><td>%lu.%03lu", metricint,
                                   metricfrac);
      } else {
      numprinted += httpd_snprintf((char *)uip_appdata + numprinted,
                                   uip_mss() - numprinted,
                                   "</td><td>-");

    }
    if(numprinted >= uip_mss()) {
      return write_mss_error(14);
    }

    /* Ping */
    if(simple_udp_ping_has_reply(&r->ipaddr)) {
      int delay = simple_udp_ping_get_delay(&r->ipaddr);
      numprinted +=
          httpd_snprintf((char *)uip_appdata + numprinted,
                         uip_mss() - numprinted, "</td><td>%u ms", delay);
      if(numprinted >= uip_mss()) {
        return write_mss_error(6);
      }
    } else if(simple_udp_ping_has_sent(&r->ipaddr)) {
      numprinted +=
          httpd_snprintf((char *)uip_appdata + numprinted,
                         uip_mss() - numprinted, "</td><td>Ping scheduled...");
      if(numprinted >= uip_mss()) {
        return write_mss_error(7);
      }
    } else {
      numprinted +=
          httpd_snprintf((char *)uip_appdata + numprinted,
                         uip_mss() - numprinted, "</td><td>-");
      if(numprinted >= uip_mss()) {
        return write_mss_error(8);
      }
    }

    numprinted +=
        httpd_snprintf((char *)uip_appdata + numprinted,
                       uip_mss() - numprinted, httpd_cgi_addrb);
    if(numprinted >= uip_mss()) {
      return write_mss_error(9);
    }


  }
  return numprinted;
}
/*---------------------------------------------------------------------------*/
static unsigned short
make_routes_roomfor(void *p)
{
  static const char httpd_cgi_rtesh[] HTTPD_STRING_ATTR = " ";
  static const char httpd_cgi_rtesf[] HTTPD_STRING_ATTR =
    "<tr><td colspan=1>Room for %u more</td>";
  int j = 0;
  uint16_t numprinted = 0;

  numprinted =
    httpd_snprintf((char *)uip_appdata, uip_mss(), httpd_cgi_rtesh);

  j = uip_ds6_route_num_routes();
  numprinted += httpd_snprintf((char *)uip_appdata + numprinted,
                               uip_mss() - numprinted,
                               httpd_cgi_rtesf, UIP_DS6_ROUTE_NB - j);
  if(numprinted >= uip_mss()) {
    return write_mss_error(19);
  }

  numprinted += httpd_snprintf((char *)uip_appdata + numprinted,
                               uip_mss() - numprinted,
                               "<td colspan=3>IP ");
  const uip_ipaddr_t *addr = simple_rpl_global_address();
  if(addr != NULL) {
    numprinted += httpd_cgi_sprint_ip6(addr,
                                       (char *)uip_appdata + numprinted);
  } else {
    numprinted += httpd_snprintf((char *)uip_appdata + numprinted,
                                 uip_mss() - numprinted,
                                 "none");
  }

  numprinted += httpd_snprintf((char *)uip_appdata + numprinted,
                               uip_mss() - numprinted,
                               ", DAG ");
  const uip_ipaddr_t *root = simple_rpl_root();
  if(root != NULL) {
    numprinted += httpd_cgi_sprint_ip6(root,
                                       (char *)uip_appdata + numprinted);
  } else {
    numprinted += httpd_snprintf((char *)uip_appdata + numprinted,
                                 uip_mss() - numprinted,
                                 "none");
  }
  rpl_dag_t *dag;
  int version = -1;
  dag = rpl_get_any_dag();
  if(dag != NULL) {
    version = dag->version;
  }
  numprinted += httpd_snprintf((char *)uip_appdata + numprinted,
                               uip_mss() - numprinted,
                               ", version %d</tr>", version);

  if(numprinted >= uip_mss()) {
    return write_mss_error(19);
  }

  return numprinted;
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(routes(struct httpd_state *s, char *ptr))
{
  PSOCK_BEGIN(&s->sout);
  printf("\n\r PT_THREAD, routes");
  for(s->u.ptr = uip_ds6_route_head();
      s->u.ptr != NULL;
      s->u.ptr = uip_ds6_route_next(s->u.ptr)) {
    PSOCK_GENERATOR_SEND(&s->sout, make_route, s);
  }
  PSOCK_GENERATOR_SEND(&s->sout, make_routes_roomfor, s);

  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(routesping(struct httpd_state *s, char *ptr))
{
  PSOCK_BEGIN(&s->sout);
  printf("\n\r PT_THREAD, routesping");
  for(s->u.ptr = uip_ds6_route_head();
      s->u.ptr != NULL;
      s->u.ptr = uip_ds6_route_next(s->u.ptr)) {
    uip_ds6_route_t *r = (uip_ds6_route_t*) s->u.ptr;
    simple_udp_ping_send_ping(&r->ipaddr);
    PSOCK_GENERATOR_SEND(&s->sout, make_route, s);
  }
  PSOCK_GENERATOR_SEND(&s->sout, make_routes_roomfor, s);

  PSOCK_END(&s->sout);
}
#endif /* WEBSERVER_CONF_STATUSPAGE */
/*---------------------------------------------------------------------------*/
static unsigned short
make_addrmap(void *p)
{
  uint16_t numprinted;
  struct ip64_addrmap_entry *m;
  struct httpd_state *s = (struct httpd_state *)p;

  numprinted = 0;
  m = s->u.ptr;

  if(m != NULL && !timer_expired(&m->timer)) {
    numprinted +=
      httpd_snprintf((char *)uip_appdata, uip_mss(), "<tr><td>");
    if(numprinted >= uip_mss()) {
      return write_mss_error(20);
    }
    numprinted +=
      httpd_cgi_sprint_ip6((uip_ipaddr_t *)&m->ip6addr,
			   (char *)uip_appdata + numprinted);
    if(numprinted >= uip_mss()) {
      return write_mss_error(21);
    }
    numprinted +=
      httpd_snprintf((char *)uip_appdata + numprinted,
		     uip_mss() - numprinted, "</td><td>%d</td><td>",
		     m->ip6port);
    if(numprinted >= uip_mss()) {
      return write_mss_error(22);
    }
    numprinted +=
      httpd_sprint_ip4(&m->ip4addr,
		       (char *)uip_appdata + numprinted);
    if(numprinted >= uip_mss()) {
      return write_mss_error(23);
    }
    numprinted +=
      httpd_snprintf((char *)uip_appdata + numprinted,
		     uip_mss() - numprinted, "</td><td>%d</td><td>%d</td>",
		     m->ip4port,
		     m->mapped_port);
    if(numprinted >= uip_mss()) {
      return write_mss_error(24);
    }
    numprinted +=
      httpd_snprintf((char *)uip_appdata + numprinted,
		     uip_mss() - numprinted, "<td>%d</td>",
		     m->protocol);
    if(numprinted >= uip_mss()) {
      return write_mss_error(25);
    }

    numprinted +=
      httpd_snprintf((char *)uip_appdata + numprinted,
		     uip_mss() - numprinted, "<td>%lu</td>",
		     m->timer.interval - (clock_time() - m->timer.start));
    if(numprinted >= uip_mss()) {
      return write_mss_error(26);
    }

    numprinted +=
      httpd_snprintf((char *)uip_appdata + numprinted,
		     uip_mss() - numprinted, "<td>0x%02x</td>",
		     m->flags);
    if(numprinted >= uip_mss()) {
      return write_mss_error(27);
    }
}
  return numprinted;
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(addrmap(struct httpd_state *s, char *ptr))
{
  PSOCK_BEGIN(&s->sout);
  printf("\n\r PT_THREAD, addrmap");
  for(s->u.ptr = ip64_addrmap_list();
      s->u.ptr != NULL;
      s->u.ptr = list_item_next(s->u.ptr)) {
    PSOCK_GENERATOR_SEND(&s->sout, make_addrmap, s);
  }

  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(rplreset(struct httpd_state *s, char *ptr))
{
  PSOCK_BEGIN(&s->sout);
  printf("\n\r PT_THREAD, rplreset");
  if(rpl_repair_root(RPL_DEFAULT_INSTANCE)) {
    PSOCK_SEND_STR(&s->sout, "Initiating global repair of RPL network...");
  } else {
    PSOCK_SEND_STR(&s->sout, "Could not start global network repair");
  }
  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(rplroot(struct httpd_state *s, char *ptr))
{
  int ret;
  PSOCK_BEGIN(&s->sout);
  printf("\n\r PT_THREAD, rplroot");
  ret = simple_rpl_init_dag_immediately();
  if(ret == 0) {
    PSOCK_SEND_STR(&s->sout, "Becoming RPL root of the network...");
  } else if(ret == -1) {
    PSOCK_SEND_STR(&s->sout, "Failed to create a RPL root: no local address found");
  } else if(ret == -2) {
    PSOCK_SEND_STR(&s->sout, "Failed to create a RPL root: no preferred address found");
  }
  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(reboot_function(struct httpd_state *s, char *ptr))
{
  PSOCK_BEGIN(&s->sout);
  printf("\n\r PT_THREAD, reboot_function");
  network_reboot_reboot(120);
  PSOCK_SEND_STR(&s->sout, "Rebooting network in 120 seconds...");
  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
void
httpd_cgi_add(struct httpd_cgi_call *c)
{
  struct httpd_cgi_call *l;

  c->next = NULL;
  if(calls == NULL) {
    calls = c;
  } else {
    for(l = calls; l->next != NULL; l = l->next);
    l->next = c;
  }
}
/*---------------------------------------------------------------------------*/

static const char adrs_name[] HTTPD_STRING_ATTR = "addresses";
static const char nbrs_name[] HTTPD_STRING_ATTR = "neighbors";
static const char nbrsping_name[] HTTPD_STRING_ATTR = "nbrsping";
static const char nbrsled_name[] HTTPD_STRING_ATTR = "nbrsled";
static const char rtes_name[] HTTPD_STRING_ATTR = "routes";
static const char rtesping_name[] HTTPD_STRING_ATTR = "rtsping";
static const char rplreset_name[] HTTPD_STRING_ATTR = "rplreset";
static const char rplroot_name[] HTTPD_STRING_ATTR = "rplroot";
static const char addrmap_name[] HTTPD_STRING_ATTR = "amap";
static const char reboot_name[] HTTPD_STRING_ATTR = "reboot";

HTTPD_CGI_CALL(file, file_name, file_stats);
HTTPD_CGI_CALL(tcp, tcp_name, tcp_stats);
HTTPD_CGI_CALL(proc, proc_name, processes);
HTTPD_CGI_CALL(adrs, adrs_name, addresses);
HTTPD_CGI_CALL(nbrs, nbrs_name, neighbors);
HTTPD_CGI_CALL(nbrsping, nbrsping_name, neighborsping);
HTTPD_CGI_CALL(nbrsled, nbrsled_name, neighborsled);
HTTPD_CGI_CALL(rtes, rtes_name, routes);
HTTPD_CGI_CALL(rtesping, rtesping_name, routesping);
HTTPD_CGI_CALL(rplr, rplreset_name, rplreset);
HTTPD_CGI_CALL(rplrt, rplroot_name, rplroot);
HTTPD_CGI_CALL(amap, addrmap_name, addrmap);
HTTPD_CGI_CALL(reboot, reboot_name, reboot_function);


void
httpd_cgi_init(void)
{
  httpd_cgi_add(&file);
  httpd_cgi_add(&tcp);
  httpd_cgi_add(&proc);

  httpd_cgi_add(&amap);
  httpd_cgi_add(&adrs);
  httpd_cgi_add(&nbrs);
  httpd_cgi_add(&nbrsping);
  httpd_cgi_add(&nbrsled);
  httpd_cgi_add(&rtes);
  httpd_cgi_add(&rtesping);
  httpd_cgi_add(&rplr);
  httpd_cgi_add(&rplrt);

  httpd_cgi_add(&reboot);

  simple_udp_ping_init(); /* ability to send and reply to UDP pings */
}
/*---------------------------------------------------------------------------*/
