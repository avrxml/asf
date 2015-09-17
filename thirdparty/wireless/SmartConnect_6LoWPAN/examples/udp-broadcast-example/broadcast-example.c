/*
 * Copyright (c) 2011, Swedish Institute of Computer Science.
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

 /**
 * \mainpage
 * \section preface Preface
 * UDP Broadcast example uses Simple UDP module to send broadcast data using UDP socket.
 * This example demonstrates the stack ability to create a UDP socket and successfully send
 * and receive UDP broadcast data over the network.
 */
 
#include "contiki.h"
#include "lib/random.h"
#include "sys/ctimer.h"
#include "sys/etimer.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "sys/energest.h"
#include "simple-udp.h"
#include "sys/compower.h"


#include <stdio.h>
#include <string.h>

#define UDP_PORT 1234

#define SEND_INTERVAL		(5 * CLOCK_SECOND)//(5 * CLOCK_SECOND) //rtc
#define SEND_TIME		(random_rand() % (SEND_INTERVAL))

static struct simple_udp_connection broadcast_connection;
static uint16_t mynodeid;
static uip_ds6_addr_t *lladdr;

#ifdef LOW_POWER_MODE
extern uint8_t ready_to_sleep;
extern uint8_t sent_packets;
extern uint8_t ready_to_send_new_packet; //rtc
#endif
/*---------------------------------------------------------------------------*/
PROCESS(broadcast_example_process, "UDP broadcast example process");
AUTOSTART_PROCESSES(&broadcast_example_process);
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
  // printf("Data received on port %d from port %d with length %d,Payload %lu\n\r",receiver_port, sender_port, datalen, *(uint32_t *)data);
  printf("\rUDP Data received is, %s \n", data);

  //printf("Data received on port %d from port %d with length %d\n\r ",receiver_port, sender_port, datalen);
#ifdef LOW_POWER_MODE  
  sent_packets=0;
  ready_to_sleep=1;
#endif  
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(broadcast_example_process, ev, data)
{
  static struct etimer periodic_timer;
  //static struct etimer send_timer;
  uip_ipaddr_t addr;
  static uint16_t count = 0;
  char strdata[30] = {0};
  
  /* Power measurements : variables Start */
  
  static uint16_t last_cpu, last_lpm, last_transmit, last_listen;
  static uint16_t last_idle_transmit, last_idle_listen;

  uint16_t  cpu, lpm, transmit, listen;
  uint16_t all_cpu, all_lpm, all_transmit, all_listen;
  uint16_t idle_transmit, idle_listen;
  // uint16_t all_idle_transmit, all_idle_listen;
  
  /* Power measurements : variables  End */ 
  
  PROCESS_BEGIN();
  lladdr = uip_ds6_get_link_local(-1);
  mynodeid = lladdr->ipaddr.u8[14];
  mynodeid <<= 8;
  mynodeid |= lladdr->ipaddr.u8[15];
  printf("Using NodeId: %X\n\r", mynodeid);
  
  simple_udp_register(&broadcast_connection, UDP_PORT,
                      NULL, UDP_PORT,
                      receiver);

  etimer_set(&periodic_timer, SEND_INTERVAL);
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    etimer_reset(&periodic_timer);
   // etimer_set(&send_timer, SEND_TIME);

    //PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&send_timer));
    uip_create_linklocal_allnodes_mcast(&addr);
    sprintf(strdata, "%s%x%s%x", "NodeId: 0x", mynodeid, " Count: 0x", count);/* Node id: 0x1234 Count: 2bytes*/
    printf("\rSending data: %s \n", strdata);
    simple_udp_sendto(&broadcast_connection, strdata, sizeof(strdata), &addr);
   
	  count++;
	  /* Every 10 packets sent, print power measurements */
	  if(count % 10 == 0)
	  {
		  /* Power measurements : Code start */
		  energest_flush();

		  all_cpu = energest_type_time(ENERGEST_TYPE_CPU);
		  all_lpm = energest_type_time(ENERGEST_TYPE_LPM);
		  all_transmit = energest_type_time(ENERGEST_TYPE_TRANSMIT);
		  all_listen = energest_type_time(ENERGEST_TYPE_LISTEN);
		  //all_idle_transmit = compower_idle_activity.transmit;
		  //all_idle_listen = compower_idle_activity.listen;

		  cpu = all_cpu - last_cpu;
		  lpm = all_lpm - last_lpm;
		  transmit = all_transmit - last_transmit;
		  listen = all_listen - last_listen;
		  idle_transmit = compower_idle_activity.transmit - last_idle_transmit;
		  idle_listen = compower_idle_activity.listen - last_idle_listen;

		  last_cpu = energest_type_time(ENERGEST_TYPE_CPU);
		  last_lpm = energest_type_time(ENERGEST_TYPE_LPM);
		  last_transmit = energest_type_time(ENERGEST_TYPE_TRANSMIT);
		  last_listen = energest_type_time(ENERGEST_TYPE_LISTEN);
		  last_idle_listen = compower_idle_activity.listen;
		  last_idle_transmit = compower_idle_activity.transmit;
		
		  printf("\n Power measurements : cpu %d lpm %d transmit %d listen %d idle_transmit %d idle_listen %d\n",cpu/(5 * 10),lpm/(5 * 10),transmit/(5 * 10),listen/(5 * 10),idle_transmit/(5 * 10),idle_listen/(5 * 10));
        
		  /* Power measurements : Code End */				
	  }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
