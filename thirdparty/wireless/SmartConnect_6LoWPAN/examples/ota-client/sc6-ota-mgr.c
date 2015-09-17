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
 * Over the air module manager
 */
#include "contiki.h"
#include "contiki-conf.h"
#include "lib/random.h"
#include "sys/ctimer.h"
#include "sys/etimer.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ip/uip-debug.h"
#include "simple-udp.h"
#include "servreg-hack.h"
#include "compiler.h"
#include "stdbool.h"
#include "ota.h"
#include "ota_mgr.h"
#include "ota_common.h"
#include "ota_upgrade.h"
#include "delay.h"
#ifdef OTA_SERVER
#include "ota_mgr_server.h"
#include "ota_parser.h"
#else
#include "ota_mgr_client.h"
#endif
#ifndef OTA_SERVER
#include "ota_nvm.h"
#endif
#include <stdio.h>
#include <string.h>
#include "leds.h"

#define UDP_PORT 5321
#define SERVICE_ID 190

#define SEND_INTERVAL		(5 * CLOCK_SECOND)


/* Currently UPGRADE and COMMON module is added for SC6LoWPAN */
/* TODO: Inlcude the macro definition in project symbols */
#define OTA_COMMON_SUPPORT   1
#define OTA_UPGRADE_SUPPORT  1

#define EXTND_ADDR_SIZE        8

#define TIMER_NONE             0
#define TIMER_INTERVAL         1
#define TIMER_PERIODIC         2

#define TIMER_STARTED          1
uint8_t server_configured = 0;
uint16_t global_shortaddr;
uint64_t global_extndaddr;
uip_ipaddr_t global_server_addr;
uint16_t pan_id = IEEE802154_CONF_PANID;
uint8_t ota_recvd_data[128];
/*---------------------------------------------------------------------------*/
PROCESS(otau_client_process, "Over-The-Air  client process");
AUTOSTART_PROCESSES(&otau_client_process);

void ota_mgr_init(void);
void configure_server_details(uip_ipaddr_t *addr);
static void ota_data_conf(module_code_t msg_code,uint8_t *addr,uint8_t status);
static struct simple_udp_connection unicast_connection;

#if (OTA_COMMON_SUPPORT == 1)
static struct ctimer ota_common_tmr;
void *ota_common_tmr_hdlr;
uint8_t ota_common_tmr_mode;
uint8_t ota_common_data[128];
#endif
#if (OTA_UPGRADE_SUPPORT == 1)
static struct ctimer ota_upgrade_tmr;
void *ota_upgrade_tmr_hdlr;
uint8_t ota_upgrade_tmr_mode;
uint8_t ota_upgrade_data[128];
#endif


node_info_t node_info = {
	.image_start = 0x00002000,
	.image_size = 0x00012326,
	.new_image = 0,
	.supported_mode = 0x02,
	.firmware = "SC6lowpan ",
	.firmware_version = "1.0.0",
	.board = "SAMR21XPro",
	.board_version = "1.0.0"
};

/*---------------------------------------------------------------------------*/

void get_node_info(node_info_t * client_info)
{
	memcpy(client_info,&node_info,sizeof(node_info_t));
	
}

void ota_mgr_data_req(module_code_t msg_code, addr_mode_t addr_mode, uint8_t *addr, void *payload, uint8_t len)
{
    uip_ipaddr_t srv_addr;
	uint64_t server_global_prefix = 0x000000000000aaaa;
	if (EXTENDED_ADDR_MODE != addr_mode)
	{
		if(addr != NULL)
		{
			memcpy((uint8_t *)&(srv_addr.u16[4]),addr,PREFIX_ADDR_SIZE);
			memcpy((uint8_t *)&(srv_addr.u16[0]),&server_global_prefix,NATIVE_ADDR_SIZE);
		}
		else
		 {
			if(server_configured)
			{
				memcpy((uint8_t *)&(srv_addr.u16[4]),(uint8_t *)&(global_server_addr.u16[4]),PREFIX_ADDR_SIZE);
				memcpy((uint8_t *)&(srv_addr.u16[0]),&server_global_prefix,NATIVE_ADDR_SIZE);			 	
			}
			else
			{
				//uip_create_linklocal_allnodes_mcast(&srv_addr);
				ota_data_conf(msg_code, (uint8_t *) &srv_addr,SUCCESS);
			}
				
		 }
		 
#if (OTA_COMMON_SUPPORT == 1)		
			if(COMMON == msg_code)
			{
				memcpy(ota_common_data, &msg_code, sizeof(msg_code));
		        memcpy(ota_common_data+1, payload, len);
				simple_udp_sendto(&unicast_connection, ota_common_data, len + sizeof(msg_code), &srv_addr);
				ota_data_conf(msg_code, (uint8_t *) &srv_addr,SUCCESS);
			}
			
#endif		
#if (OTA_UPGRADE_SUPPORT == 1)			
			else if(UPGRADE == msg_code)
			{
				memcpy(ota_upgrade_data, &msg_code, sizeof(msg_code));
		        memcpy(ota_upgrade_data+1, payload, len);
				simple_udp_sendto(&unicast_connection, ota_upgrade_data, len + sizeof(msg_code), &srv_addr);
				ota_data_conf(msg_code, (uint8_t *) &srv_addr,SUCCESS);
			}
			
#endif
	}
}

static void ota_data_conf(module_code_t msg_code,uint8_t *addr,uint8_t status)
{
	#if (OTA_COMMON_SUPPORT == 1)
	if(COMMON == msg_code)
	{
		ota_common_sent_frame(NATIVE_ADDR_MODE, addr, status);
	}
	else
	#endif
	#if (OTA_UPGRADE_SUPPORT == 1)
	if(UPGRADE == msg_code)
	{
		ota_upgrade_sent_frame(NATIVE_ADDR_MODE, addr, status);
	}
	else
	#endif
	{
		/* Log error */
	}
}
/*****************************************************************************
*****************************************************************************/


/**/


void ota_mgr_timer_start(module_code_t msg_code, uint32_t interval, ota_timer_mode_t mode, void *handler)
{
	switch (msg_code)
	{
#if (OTA_COMMON_SUPPORT == 1)	
	case COMMON:
	{	
		if(ota_common_tmr_mode != TIMER_NONE)
		{
			ctimer_stop(&ota_common_tmr);
		}
	    ctimer_set(&ota_common_tmr,(interval*CLOCK_SECOND)/1000,handler,NULL);
		ota_common_tmr_hdlr = handler;
		if(TIMER_MODE_SINGLE == mode)
			ota_common_tmr_mode = TIMER_INTERVAL;
		else
		{
			ota_common_tmr_mode = TIMER_PERIODIC;
		}
	}
	break;
#endif		
#if (OTA_UPGRADE_SUPPORT == 1)		
	case UPGRADE:
	{
		if(ota_upgrade_tmr_mode != TIMER_NONE)
		{
			ctimer_stop(&ota_upgrade_tmr);
		}	
		ctimer_set(&ota_upgrade_tmr,(interval*CLOCK_SECOND)/1000,handler,NULL);
		ota_upgrade_tmr_hdlr = handler;
		if(TIMER_MODE_SINGLE == mode)
			ota_upgrade_tmr_mode = TIMER_INTERVAL;
		else
		{
			ota_upgrade_tmr_mode = TIMER_PERIODIC;		
		}
	}
	break;
#endif
	default:
		break;
	}
}

void ota_mgr_timer_stop(module_code_t msg_code)
{
	switch (msg_code)
	{
#if (OTA_COMMON_SUPPORT == 1)	
		case COMMON:
			ctimer_stop(&ota_common_tmr);
			ota_common_tmr_mode = TIMER_NONE;
			break;
#endif		
#if (OTA_UPGRADE_SUPPORT == 1)			
		case UPGRADE:
			ctimer_stop(&ota_upgrade_tmr);
			ota_upgrade_tmr_mode = TIMER_NONE;
			break;
#endif
		default:
			break;
	}
}



/*---------------------------------------------------------------------------*/
void configure_server_details(uip_ipaddr_t *addr)
{
	if(addr != NULL) {
#if (OTA_COMMON_SUPPORT == 1)		
		configure_common_server_details(NATIVE_ADDR_MODE, (uint8_t *) &(addr->u16[4]));
		configure_common_server_details(EXTENDED_ADDR_MODE, (uint8_t *) &(addr->u16[4]));
#endif		
#if (OTA_UPGRADE_SUPPORT == 1)			
		configure_upgrade_server_details(NATIVE_ADDR_MODE, (uint8_t *) &(addr->u16[4]));
		configure_upgrade_server_details(EXTENDED_ADDR_MODE, (uint8_t *) &(addr->u16[4]));
#endif
	} else {
	  printf("ERROR: Address Received is NULL");
	}

}
uint16_t *get_pan_id(void)
{
	return &pan_id;
}
uint8_t *get_node_address(addr_mode_t addr_mode)
{

	if (NATIVE_ADDR_MODE == addr_mode)
	{
		return (uint8_t *)&global_extndaddr;
	}
	else
	{
		return (uint8_t *)&global_extndaddr;
	}
}
void ota_mgr_set_channel(uint8_t channel)
{
	rf_set_channel(channel);
}

void ota_mgr_led(ota_led_t led_state)
{
	if (OTA_LED_ON == led_state)
	{
		leds_on(LED0);
	}
	else if (OTA_LED_OFF == led_state)
	{
		leds_off(LED0);
	} 
	else
	{
		leds_toggle(LED0);
	}
}

void ota_mgr_reset_device(void)
{
	NVIC_SystemReset();
}
static void
set_global_address(void)
{
  uip_ipaddr_t ipaddr;
  int i;
  uint8_t state;

  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
  memcpy(((uint8_t *)&global_extndaddr),&(ipaddr.u8[8]),NATIVE_ADDR_SIZE);
  printf("\r\nIPv6 addresses: ");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      uip_debug_ipaddr_print(&uip_ds6_if.addr_list[i].ipaddr);
      printf("\n");
    }
  }
}
/*---------------------------------------------------------------------------*/
static void
receiver(struct simple_udp_connection *c,const uip_ipaddr_t *sender_addr,uint16_t sender_port,
const uip_ipaddr_t *receiver_addr,uint16_t receiver_port,const uint8_t *data,uint16_t datalen)
{
	if((datalen > 0) && (NULL != data))
	{
		#if (OTA_COMMON_SUPPORT == 1)
		if(COMMON == *data)
		{
			ota_common_rcvd_frame(NATIVE_ADDR_MODE, (uint8_t *)(&sender_addr->u16[4]), datalen-1,data+1,255);
		}
		else
		#endif
		#if (OTA_UPGRADE_SUPPORT == 1)
		if (UPGRADE == *data)
		{
			ota_upgrade_rcvd_frame(NATIVE_ADDR_MODE, (uint8_t *)(&sender_addr->u16[4]), datalen-1, data+1,255);
		}
		else
		#endif
		{
			printf("\r\nERROR: Unknown data type");
		}
	}
}

void ota_mgr_init(void)
{
	uint32_t* nvm_addr_loc = (uint32_t *)NVM_APP_SHORT_ADDR;
	ota_nvm_init();
	
	nvm_addr_loc = (uint32_t *) NVM_DEVICE_INFO_LOCATION;
	if(0xFFFFFFFF == *nvm_addr_loc)
	{
		ota_nvm_write(MEMORY_ABSOLUTE_ADDRESS, (uint32_t)nvm_addr_loc, sizeof(node_info_t),(uint8_t *)&node_info);
	}
	else
	{
		ota_nvm_read(MEMORY_ABSOLUTE_ADDRESS, (uint32_t)nvm_addr_loc, sizeof(node_info_t),(uint8_t *)&node_info);
	}
	ota_common_init();
	ota_upgrade_init();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(otau_client_process, ev, data)
{
  static struct etimer server_timer;
  uip_ip6addr_t *srv_addr;
  
  PROCESS_BEGIN();
  servreg_hack_init();
  set_global_address();
  
  ota_mgr_init();
  
  simple_udp_register(&unicast_connection, UDP_PORT,
                      NULL, UDP_PORT, receiver);
  etimer_set(&server_timer, SEND_INTERVAL);					  

  while(1) {
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&server_timer));
	srv_addr = servreg_hack_lookup(SERVICE_ID);
	if(srv_addr != NULL) {
		printf("Server address configured\n");
		server_configured =1;
		configure_server_details(srv_addr);
		memcpy((uint8_t *)&(global_server_addr.u16[4]),&(srv_addr->u16[4]),NATIVE_ADDR_SIZE);
        //uip_create_linklocal_allnodes_mcast(srv_addr);
		etimer_stop(&server_timer);
	}
	else
	{
		etimer_reset(&server_timer);
		printf("server not found");
	}
  }
  PROCESS_END();
}

