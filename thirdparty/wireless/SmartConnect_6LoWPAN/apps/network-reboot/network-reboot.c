/*
 * Copyright (c) 2012, Thingsquare, http://www.thingsquare.com/.
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
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "contiki-net.h"

#include "net/ip/simple-udp.h"

#include "network-reboot.h"

#include "dev/leds.h"

#include "dev/watchdog.h"


#include <string.h>
#include <stdio.h>

static struct simple_udp_connection broadcast_connection;

#define UDP_PORT 31337

#define SEND_INTERVAL		(20 * CLOCK_SECOND)
#define SEND_TIME		(random_rand() % (SEND_INTERVAL))

#define STARTUP_GRACE_PERIOD    (120 * CLOCK_SECOND)

struct network_reboot_msg {
  uint8_t magic_bytes[6];
  uint16_t seconds;
};

static int seconds_until_reboot;

static const char magic_bytes[6] = "Reboot";

/*---------------------------------------------------------------------------*/
PROCESS(network_reboot_process, "network_reboot process");
//AUTOSTART_PROCESSES(&network_reboot_process);
/*---------------------------------------------------------------------------*/
static void
receiver(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  struct network_reboot_msg msg;

  if(datalen == sizeof(struct network_reboot_msg)) {
    memcpy(&msg, data, sizeof(msg));
    if(memcmp(msg.magic_bytes, magic_bytes, sizeof(magic_bytes)) == 0) {
      seconds_until_reboot = uip_htons(msg.seconds);
    }
  }
}
/*---------------------------------------------------------------------------*/
static void
send_msg(struct simple_udp_connection *conn, int seconds)
{
  uip_ipaddr_t addr;
  struct network_reboot_msg msg;
  memcpy(msg.magic_bytes, magic_bytes, sizeof(msg.magic_bytes));
  msg.seconds = uip_htons(seconds_until_reboot);
  uip_create_linklocal_allnodes_mcast(&addr);
  simple_udp_sendto(conn, &msg, sizeof(msg), &addr);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(network_reboot_process, ev, data)
{
  static struct etimer periodic_timer;
  static struct etimer send_timer;
  static struct etimer countdown_timer;

  PROCESS_BEGIN();

  /* Wait for a while before starting to listen to the reboot code. */

  etimer_set(&periodic_timer, STARTUP_GRACE_PERIOD);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

  simple_udp_register(&broadcast_connection, UDP_PORT,
                      NULL, UDP_PORT, receiver);

  etimer_set(&periodic_timer, SEND_INTERVAL);
  etimer_set(&send_timer, SEND_TIME);
  etimer_set(&countdown_timer, CLOCK_SECOND);
  while(1) {
    PROCESS_WAIT_EVENT();

    if(data == &countdown_timer) {
      etimer_reset(&countdown_timer);
      if(seconds_until_reboot > 0) {
	printf("Seconds until reboot %d\n", seconds_until_reboot);
	leds_toggle(LEDS_ALL);
	seconds_until_reboot--;
	if(seconds_until_reboot == 0) {
	  printf("Rebooting\n");
	  //watchdog_init();
	}
      }
    }

    if(data == &periodic_timer) {
      etimer_reset(&periodic_timer);
      etimer_set(&send_timer, SEND_TIME);
    }

    if(data == &send_timer) {
      if(seconds_until_reboot > 0) {
	send_msg(&broadcast_connection, seconds_until_reboot);
      }
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void
network_reboot_init(void)
{
  process_start(&network_reboot_process, NULL);
}
/*---------------------------------------------------------------------------*/
void
network_reboot_reboot(int seconds)
{
  send_msg(&broadcast_connection, seconds);
  seconds_until_reboot = seconds;
}
/*---------------------------------------------------------------------------*/
