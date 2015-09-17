/*
 * Copyright (c) 2012, Thingsquare
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
 */

#include "contiki.h"
#include "lib/random.h"
#include "sys/ctimer.h"
#include "sys/etimer.h"
#include "net/ip/uip.h"
#include "net/ip/uip-debug.h"
#include "net/ipv6/uip-icmp6.h"
#include "net/ipv6/uip-ds6.h"
#include "dev/leds.h"

#include "simple-udp.h"
#include "simple-udp-ping.h"

#include <stdio.h>
#include <string.h>

#define UDP_PORT 3117

#define MAX_DESTINATIONS NBR_TABLE_CONF_MAX_NEIGHBORS

#define _DEBUG_ 1

#define DATALEN 4

#ifndef _DEBUG_
#define UDP_PING_DEBUG 1
#endif

struct pingconn_t {
  uint8_t in_use;
  uint8_t waiting;
  uint8_t sent;
  uint8_t replied;
  rtimer_clock_t echo_time;
  clock_time_t echo_time2;
  uint32_t delay;
  uip_ipaddr_t host;
};
static struct pingconn_t pingconns[MAX_DESTINATIONS];

/*---------------------------------------------------------------------------*/
PROCESS(simple_udp_ping_process, "Simple ping over UDP");
/*---------------------------------------------------------------------------*/
static struct pingconn_t*
get_pingconn(const uip_ipaddr_t *addr)
{
  int i;
  for(i = 0; i < MAX_DESTINATIONS; i++) {
    if(!pingconns[i].in_use) {
      continue;
    }
    if(uip_ipaddr_cmp(&pingconns[i].host, addr)) {
      return &pingconns[i];
    }
  }
  return NULL;
}
/*---------------------------------------------------------------------------*/
static struct pingconn_t*
allocate_pingconn(const uip_ipaddr_t *addr)
{
  static int last = MAX_DESTINATIONS;
  int i;

  for(i = 0; i < MAX_DESTINATIONS; i++) {
    if(!pingconns[i].in_use) {
      pingconns[i].in_use = 1;
      uip_ipaddr_copy(&pingconns[i].host, addr);
      return &pingconns[i];
    }
  }

  last++;
  if(last > MAX_DESTINATIONS) {
    last = 0;
  }

  pingconns[last].in_use = 1;
  uip_ipaddr_copy(&pingconns[last].host, addr);
  return &pingconns[last];
}
/*---------------------------------------------------------------------------*/
int
simple_udp_ping_send_ping(const uip_ipaddr_t *addr)
{
  /* Get ping connection */
  struct pingconn_t* pingconn = get_pingconn(addr);
  if(pingconn == NULL) {
    pingconn = allocate_pingconn(addr);
  }

  printf("simple_udp_ping_send_ping: ");
  uip_debug_ipaddr_print(addr);
  printf("\n");
  pingconn->replied = 0;
  pingconn->waiting = 1;
  return -1;
}
/*---------------------------------------------------------------------------*/
void
simple_udp_ping_clear_conn(uip_ipaddr_t *addr)
{
  /* Get ping connection */
  struct pingconn_t* pingconn = get_pingconn(addr);
  if(pingconn == NULL) {
    return;
  }

  pingconn->replied = 0;
  pingconn->sent = 0;
}
/*---------------------------------------------------------------------------*/
int
simple_udp_ping_has_reply(uip_ipaddr_t *addr)
{
  struct pingconn_t *pingconn = get_pingconn(addr);
  if(pingconn == NULL) {
    return 0;
  }
  if(!pingconn->replied) {
    return 0;
  }
  return 1;
}
/*---------------------------------------------------------------------------*/
int
simple_udp_ping_has_sent(uip_ipaddr_t *addr)
{
  struct pingconn_t *pingconn = get_pingconn(addr);
  if(pingconn == NULL) {
    return 0;
  }
  if(pingconn->waiting) {
    return 1;
  }
  if(pingconn->sent) {
    return 1;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
int
simple_udp_ping_get_delay(uip_ipaddr_t *addr)
{
  uint32_t ms;

  struct pingconn_t *pingconn = get_pingconn(addr);
  if(pingconn == NULL) {
    return -1;
  }
  if(!pingconn->replied) {
    return -1;
  }
  ms = (uint32_t)1000 * (uint32_t)pingconn->delay;
  ms /= RTIMER_SECOND;
  return (int)ms;
}
/*---------------------------------------------------------------------------*/
void
simple_udp_ping_init(void)
{
  process_start(&simple_udp_ping_process, NULL);
}
/*---------------------------------------------------------------------------*/
static void
echo_reply_callback(uip_ipaddr_t *sender,
                    uint8_t ttl, uint8_t *data, uint16_t datalen)
{
  struct pingconn_t* pingconn = get_pingconn(sender);

  if(pingconn != NULL) {
    pingconn->replied = 1;
    pingconn->sent = 0;
    if(clock_time() - pingconn->echo_time2 > CLOCK_SECOND) {
      pingconn->delay = clock_time() - pingconn->echo_time2;
      pingconn->delay *= RTIMER_SECOND;
      pingconn->delay /= CLOCK_SECOND;
    } else {
      pingconn->delay = RTIMER_NOW() - pingconn->echo_time;
    }
#if UDP_PING_DEBUG
    printf("Received echo from ");
    uip_debug_ipaddr_print(sender);
    printf(", delay ticks %lu\n", pingconn->delay);
#endif
  } else {
    printf("warning: received ping reply from unknown host\n");
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(simple_udp_ping_process, ev, data)
{
  static struct etimer et;
  static struct uip_icmp6_echo_reply_notification n;
  int i;

  PROCESS_BEGIN();


  uip_icmp6_echo_reply_callback_add(&n, echo_reply_callback);
  for(i = 0; i < MAX_DESTINATIONS; i++) {
    pingconns[i].in_use = 0;
  }

  while(1) {
#define PERIOD (2 * CLOCK_SECOND)
    etimer_set(&et, PERIOD);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    for(i = 0; i < MAX_DESTINATIONS; i++) {
      if (pingconns[i].in_use && pingconns[i].waiting) {
        struct pingconn_t* pingconn = &pingconns[i];

        pingconn->waiting = 0;

        /* Send ping */
#if UDP_PING_DEBUG
        printf("Sending ping to ");
        uip_debug_ipaddr_print(&pingconn->host);
        printf("\n");
#endif
        uip_icmp6_send(&pingconn->host,
                       ICMP6_ECHO_REQUEST, 0, 0);
        pingconn->echo_time = RTIMER_NOW();
        pingconn->echo_time2 = clock_time();
        pingconn->sent = 1;
        pingconn->replied = 0;
        break;
      }
    }

  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
