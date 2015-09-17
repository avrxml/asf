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
 * Border Router Web-server example application uses IP64 module to communicate outside 
 * 6LoWPAN network, DHCP module to get an IPv4 address and simple RPL module to create RPL DAG root.
 * This example also uses IP64 Web serve module, to create HTML pages views of configured 
 * information which can be viewed from a browser outside the network. 
 * This example demonstrates the ability of stack to create a DAG root and act as 
 * Border gateway to the nodes which wants to communicate outside the network.
 * This application uses IP64 module which converts IPv6 address of a node to IPv4 address 
 * and creates an entry in IP64 table and maintains it.
 * This Application uses DHCPv4 module to get an IPv4 address from outside the network.
 */


#include "thsq.h"
#include "ip64.h"
#include "ip64-webserver.h"
extern struct process network_reboot_process;
/*---------------------------------------------------------------------------*/
PROCESS(blinker_process, "blinker_process");
PROCESS(router_node_process, "Router node");
AUTOSTART_PROCESSES(&router_node_process
,&blinker_process
,&network_reboot_process
);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(blinker_process, ev, data)
{
	static struct etimer et;
	static uint8_t red, green;
	PROCESS_BEGIN();

	etimer_set(&et, CLOCK_SECOND / 2);
	while(1) {
		PROCESS_WAIT_UNTIL(etimer_expired(&et));
		etimer_reset(&et);
		if(0) {
			leds_on(LEDS_RED);
			red = 1;
			} else {
			red = 0;
		}
		if(!ip64_hostaddr_is_configured()) {
			leds_on(LEDS_GREEN);
			green = 1;
			} else {
			green = 0;
		}
		PROCESS_WAIT_UNTIL(etimer_expired(&et));
		etimer_reset(&et);
		if(red) {
			leds_off(LEDS_RED);
		}
		if(green) {
			leds_off(LEDS_GREEN);
		}
	}
	PROCESS_END();
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(router_node_process, ev, data)
{
	PROCESS_BEGIN();

	/* Set us up as a RPL root node. */
	simple_rpl_init_dag();

	/* Initialize the IP64 module so we'll start translating packets */
	ip64_init();
//#if (ENABLE_WEBSERVER == 1)
	/* Initialize the IP64 webserver */
	ip64_webserver_init();

	NETSTACK_RDC.off(1);
//#endif
	/* ... and do nothing more. */
	while(1) {
		PROCESS_WAIT_EVENT();
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
