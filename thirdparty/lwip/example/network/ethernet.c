/*****************************************************************************
 *
 * \file
 *
 * \brief Ethernet management for the Standalone lwIP example.
 *
 * Copyright (c) 2010-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 *****************************************************************************/


#include <string.h>
#include "board.h"
#include "gpio.h"
#include "display.h"
#include "ethernet.h"
#include "conf_eth.h"
#include "macb.h"
#include "timer_mgt.h"
#include "power_clocks_lib.h"

/* lwIP includes */
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/memp.h"
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "lwip/stats.h"
#include "lwip/init.h"
#if ( (LWIP_VERSION) == ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
#include "netif/loopif.h"
#else
#include "lwip/inet.h"
#endif
#include "netif/etharp.h"
#include "netif/ethernetif.h"

#if defined(HTTP_RAW_USED)
#include "httpd.h"
#endif

//_____ M A C R O S ________________________________________________________
//_____ D E F I N I T I O N S ______________________________________________

/* global variable containing MAC Config (hw addr, IP, GW, ...) */
struct netif MACB_if;

//_____ D E C L A R A T I O N S ____________________________________________

/* Initialization of Ethernet interfaces by reading config file */
static void prvEthernetConfigureInterface(void * param);
void status_callback(struct netif *netif);


/*! \brief create ethernet task, for ethernet management.
 *
 *  \param uxPriority   Input. priority for the task, it should be low
 *
 */
void init_ethernet(pcl_freq_param_t *param)
{
	static const gpio_map_t MACB_GPIO_MAP =
	{
		{EXTPHY_MACB_MDC_PIN,     EXTPHY_MACB_MDC_FUNCTION   },
		{EXTPHY_MACB_MDIO_PIN,    EXTPHY_MACB_MDIO_FUNCTION  },
		{EXTPHY_MACB_RXD_0_PIN,   EXTPHY_MACB_RXD_0_FUNCTION },
		{EXTPHY_MACB_TXD_0_PIN,   EXTPHY_MACB_TXD_0_FUNCTION },
		{EXTPHY_MACB_RXD_1_PIN,   EXTPHY_MACB_RXD_1_FUNCTION },
		{EXTPHY_MACB_TXD_1_PIN,   EXTPHY_MACB_TXD_1_FUNCTION },
		{EXTPHY_MACB_TX_EN_PIN,   EXTPHY_MACB_TX_EN_FUNCTION },
		{EXTPHY_MACB_RX_ER_PIN,   EXTPHY_MACB_RX_ER_FUNCTION },
		{EXTPHY_MACB_RX_DV_PIN,   EXTPHY_MACB_RX_DV_FUNCTION },
		{EXTPHY_MACB_TX_CLK_PIN,  EXTPHY_MACB_TX_CLK_FUNCTION}
	};

	// Assign GPIO to MACB
	gpio_enable_module(MACB_GPIO_MAP,
		sizeof(MACB_GPIO_MAP) / sizeof(MACB_GPIO_MAP[0]));

	/* Initialize timer for lwIP calls; generates interrupts every 1ms */
	init_timer(param->pba_f);

	/* Initialize lwIP. */
	lwip_init();

	/* Set hw and IP parameters, initialize MACB too */
	prvEthernetConfigureInterface(NULL);

#if defined(HTTP_RAW_USED)
	/* Bring up the web server */
	httpd_init();
#endif
}

/*!
 *  \brief status callback used to print address given by DHCP
 */
void status_callback(struct netif *netif)
{
  char mess[25];
  if (netif_is_up(netif)) {
	display_print("Network up");
    strcpy(mess,"IP=");
    strcat(mess,inet_ntoa(*(struct in_addr*)&(netif->ip_addr)));
    display_print(mess);
  } else {
	  display_print("Network down");
  }
}


/*!
 *  \brief set ethernet config
 */
static void prvEthernetConfigureInterface(void * param)
{
   struct ip_addr    xIpAddr, xNetMask, xGateway;
   extern err_t      ethernetif_init( struct netif *netif );
   unsigned char MacAddress[6];

   /* Default MAC addr. */
   MacAddress[0] = ETHERNET_CONF_ETHADDR0;
   MacAddress[1] = ETHERNET_CONF_ETHADDR1;
   MacAddress[2] = ETHERNET_CONF_ETHADDR2;
   MacAddress[3] = ETHERNET_CONF_ETHADDR3;
   MacAddress[4] = ETHERNET_CONF_ETHADDR4;
   MacAddress[5] = ETHERNET_CONF_ETHADDR5;

   /* pass the MAC address to MACB module */
   vMACBSetMACAddress( MacAddress );

#if defined(DHCP_USED)
  xIpAddr.addr  = 0;
  xNetMask.addr = 0;
  xNetMask.addr = 0;
#else
  /* Default ip addr. */
  IP4_ADDR( &xIpAddr,ETHERNET_CONF_IPADDR0,ETHERNET_CONF_IPADDR1,ETHERNET_CONF_IPADDR2,ETHERNET_CONF_IPADDR3 );

  /* Default Subnet mask. */
  IP4_ADDR( &xNetMask,ETHERNET_CONF_NET_MASK0,ETHERNET_CONF_NET_MASK1,ETHERNET_CONF_NET_MASK2,ETHERNET_CONF_NET_MASK3 );

  /* Default Gw addr. */
  IP4_ADDR( &xGateway,ETHERNET_CONF_GATEWAY_ADDR0,ETHERNET_CONF_GATEWAY_ADDR1,ETHERNET_CONF_GATEWAY_ADDR2,ETHERNET_CONF_GATEWAY_ADDR3 );
#endif

  /* add data to netif */
  netif_add( &MACB_if, &xIpAddr, &xNetMask, &xGateway, NULL, ethernetif_init, ethernet_input );

  /* make it the default interface */
  netif_set_default( &MACB_if );

  /* Setup callback function for netif status change */
  netif_set_status_callback(&MACB_if, status_callback);

  /* bring it up */
#if defined(DHCP_USED)
  display_print("LwIP: DHCP Started");
  dhcp_start( &MACB_if );
#else
  display_print("LwIP: Static IP Address Assigned");
  netif_set_up( &MACB_if );
#endif
}


/*!
 *  \brief Manage the Ethernet packets, if any received process them
 *  After processing any packets, manage the lwIP timers
 */
void ethernet_task(void)
{
	/* Check once to see if packet is ready before managing the timers */
	if(vMACBWaitForInput(1))
	{
		// Process received packet
		ethernetif_input(&MACB_if);
	}
	/* Manages all the timers lwIP requires in standalone mode */
	manage_timers();
}

