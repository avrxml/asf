/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include "lwip/opt.h"

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
#include "netif/ppp_oe.h"

#include "conf_eth.h"
#include "macb.h"

#ifndef FREERTOS_USED
#include "netif/ethernetif.h"
#endif

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

#ifdef FREERTOS_USED
#define netifGUARD_BLOCK_NBTICKS       ( 250 )
#endif

//The MAC address from MAC driver
extern unsigned char cMACAddress[ 6 ];

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
};

#ifdef FREERTOS_USED
/* Forward declaration when FreeRTOS is used. */
static void  ethernetif_input(void * );
#endif

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void
low_level_init(struct netif *netif)
{
#ifdef FREERTOS_USED
  unsigned portBASE_TYPE uxPriority;
#endif

  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* set MAC hardware address */
	netif->hwaddr[0] = cMACAddress[0];
	netif->hwaddr[1] = cMACAddress[1];
	netif->hwaddr[2] = cMACAddress[2];
	netif->hwaddr[3] = cMACAddress[3];
	netif->hwaddr[4] = cMACAddress[4];
	netif->hwaddr[5] = cMACAddress[5];

  /* maximum transfer unit */
  netif->mtu = 1500;

  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP
#if defined(DHCP_USED)
    | NETIF_FLAG_DHCP
#endif
  ;

  /* Do whatever else is needed to initialize interface. */
  /* Initialise the MACB. */
#ifdef FREERTOS_USED
  // NOTE: This routine contains code that polls status bits. If the Ethernet
  // cable is not plugged in then this can take a considerable time.  To prevent
  // this from starving lower priority tasks of processing time we lower our
  // priority prior to the call, then raise it back again once the initialization
  // is complete.

  // Read the priority of the current task.
  uxPriority = uxTaskPriorityGet( NULL );
  // Set the priority of the current task to the lowest possible.
  vTaskPrioritySet( NULL, tskIDLE_PRIORITY );
#endif
  // Init the MACB interface.
  while( xMACBInit(&AVR32_MACB) == false )
  {
    __asm__ __volatile__ ( "nop" );
  }
#ifdef FREERTOS_USED
  // Restore the priority of the current task.
  vTaskPrioritySet( NULL, uxPriority );

  /* Create the task that handles the MACB input packets. */
  sys_thread_new( "ETHINT", ethernetif_input, netif, netifINTERFACE_TASK_STACK_SIZE,
                  netifINTERFACE_TASK_PRIORITY );
#endif
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf             *q;
#ifdef FREERTOS_USED
  static xSemaphoreHandle xTxSemaphore = NULL;
#endif


  ( void )netif; // Unused param, avoid a compiler warning.

#ifdef FREERTOS_USED
  if( xTxSemaphore == NULL )
  {
    vSemaphoreCreateBinary( xTxSemaphore );
  }
#endif

#if ETH_PAD_SIZE
  pbuf_header( p, -ETH_PAD_SIZE );    /* drop the padding word */
#endif

#ifdef FREERTOS_USED
  /* Access to the MACB is guarded using a semaphore. */
  if( xSemaphoreTake( xTxSemaphore, netifGUARD_BLOCK_NBTICKS ) )
  {
#endif
    for( q = p; q != NULL; q = q->next )
    {
      /* Send the data from the pbuf to the interface, one pbuf at a time. The
      size of the data in each pbuf is kept in the ->len variable.
      This function also signals to the MACB that the packet should be sent. */
      lMACBSend(&AVR32_MACB, q->payload, q->len, ( q->next == NULL ) );
    }
#ifdef FREERTOS_USED
    xSemaphoreGive( xTxSemaphore );
  }
#endif

#if ETH_PAD_SIZE
  pbuf_header( p, ETH_PAD_SIZE );     /* reclaim the padding word */
#endif

  LINK_STATS_INC(link.xmit);  // Traces

  return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *low_level_input(struct netif *netif)
{
  struct pbuf             *p = NULL;
  struct pbuf             *q;
  u16_t                   len;
#ifdef FREERTOS_USED
  static xSemaphoreHandle xRxSemaphore = NULL;
#endif


  /* Parameter not used. */
  ( void ) netif;

#ifdef FREERTOS_USED
  if( xRxSemaphore == NULL )
  {
    vSemaphoreCreateBinary( xRxSemaphore );
  }

  /* Access to the MACB is guarded using a semaphore. */
  if( xSemaphoreTake( xRxSemaphore, netifGUARD_BLOCK_NBTICKS ) )
  {
#endif
    /* Obtain the size of the packet. */
    len = ulMACBInputLength();

    if( len )
    {
#if ETH_PAD_SIZE
      len += ETH_PAD_SIZE;    /* allow room for Ethernet padding */
#endif

      /* We allocate a pbuf chain of pbufs from the pool. */
      p = pbuf_alloc( PBUF_RAW, len, PBUF_POOL );

      if( p != NULL )
      {
#if ETH_PAD_SIZE
        pbuf_header( p, -ETH_PAD_SIZE );    /* drop the padding word */
#endif

        /* Let the driver know we are going to read a new packet. */
        vMACBRead( NULL, 0, len );

        /* We iterate over the pbuf chain until we have read the entire
        packet into the pbuf. */
        for( q = p; q != NULL; q = q->next )
        {
          /* Read enough bytes to fill this pbuf in the chain. The
          available data in the pbuf is given by the q->len variable. */
          vMACBRead( q->payload, q->len, len );
        }

#if ETH_PAD_SIZE
        pbuf_header( p, ETH_PAD_SIZE );     /* reclaim the padding word */
#endif
        LINK_STATS_INC(link.recv);
      }
      else
      {
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
      }
    }
#ifdef FREERTOS_USED
    xSemaphoreGive( xRxSemaphore );
  }
#endif

  return p;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
#ifdef FREERTOS_USED
static
#endif
void ethernetif_input(void * pvParameters)
{
  struct netif      *netif = (struct netif *)pvParameters;
  struct pbuf       *p;

#ifdef FREERTOS_USED
  for( ;; )
  {
    do
    {
#endif
      /* move received packet into a new pbuf */
      p = low_level_input( netif );
      if( p == NULL )
      {
#ifdef FREERTOS_USED
        /* No packet could be read.  Wait a for an interrupt to tell us
        there is more data available. */
        vMACBWaitForInput(100);
      }
    }while( p == NULL );
#else
        return;
      }
#endif

    if( ERR_OK != netif->input( p, netif ) )
    {
      pbuf_free(p);
      p = NULL;
    }
#ifdef FREERTOS_USED
  }
#endif
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init(struct netif *netif);
err_t
ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
#if LWIP_SNMP
  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100000000);
#endif /* LWIP_SNMP */

  netif->state = NULL;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;

  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}
