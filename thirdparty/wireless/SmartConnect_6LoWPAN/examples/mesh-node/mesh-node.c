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
 * This example application has to options. 
 *	Mesh-node – Join an existing DAG network. Wait till DAG network is found.
 *	Mesh-root – Create a DAG network and becomes root of the DAG network.
 * This example uses UIP-DS6 to get notification from core stack, when route is added for the node.
 * This example demonstrates the ability of stack to create DAG network, 
 * join the existing network and get notifications from UIP module.
 */

#include "thsq.h"



#include "net/ip/uip-debug.h"
#if (MESH_NODE == 1)
/*---------------------------------------------------------------------------*/
PROCESS(mesh_node_process, "Mesh node");
AUTOSTART_PROCESSES(&mesh_node_process);
/*---------------------------------------------------------------------------*/
static void
route_callback(int event, uip_ipaddr_t *route, uip_ipaddr_t *ipaddr,
               int numroutes)
{
    leds_off(LEDS_ALL);
   printf("Got a RPL route event: %d", event);
	uip_debug_ipaddr_print(route);
	printf("; Ipaddr:");
	uip_debug_ipaddr_print(ipaddr);
	printf("; Num of routes: %d\r\n", numroutes);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(mesh_node_process, ev, data)
{
  static struct uip_ds6_notification n;
  PROCESS_BEGIN();

  leds_on(LEDS_ALL);
  uip_ds6_notification_add(&n, route_callback);

  while(1) {
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
#else
/*---------------------------------------------------------------------------*/
PROCESS(mesh_root_process, "Mesh root");
AUTOSTART_PROCESSES(&mesh_root_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(mesh_root_process, ev, data)
{
  PROCESS_BEGIN();

  /* Set us up as a RPL root root. */
  simple_rpl_init_dag();

  while(1) {
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}
#endif