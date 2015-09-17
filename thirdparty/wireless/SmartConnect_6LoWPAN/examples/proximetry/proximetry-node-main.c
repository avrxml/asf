/**
* Copyright (c) 2015 Atmel Corporation and 2012 – 2013, Thingsquare, http://www.thingsquare.com/. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. Neither the name of Atmel nor the name of Thingsquare nor the names of its contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* 4. This software may only be redistributed and used in connection with an
* Atmel microcontroller or Atmel wireless product.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
* GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*
*
*/


#include <stdio.h>
#include <string.h>

#include "contiki.h"
#include "contiki-net.h"
#include "sys/autostart.h"
#include "flash.h"
#include "serial-line.h"
#include "slip.h"
#include "watchdog.h"
#include "xmem.h"
#include "lib/random.h"

#include "netstack-aes.h"
#include "net/netstack.h"
#include "net/ip/uip.h"
#include "net/mac/frame802154.h"
#include "net/linkaddr.h"
#include "net/rime/rime.h"
#if SAMD
#include "node-id-samd21.h"
#else
#include "node-id.h"
#endif

#include "delay.h"
#include <asf.h>
#include "usart.h"
#include "sio2host.h"
#include "conf_sio2host.h"
#include "stdio_serial.h"
#include "rtc_count.h" //rtc
#include "rtc_count_interrupt.h"

#include "common.h"

#include "misc.h"
#include "settings.h"

#include "sam0_sensors.h"

/*---------------------------------------------------------------------------*/
#define _DEBUG_ 0
#if _DEBUG_
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
/*---------------------------------------------------------------------------*/

#if UIP_CONF_IPV6
PROCINIT(&etimer_process, &tcpip_process);
#else
PROCINIT(&etimer_process);
#warning "No TCP/IP process!"
#endif

/*---------------------------------------------------------------------------*/

static void print_reset_causes(void);
static void print_processes(struct process * const processes[]);
static void set_link_addr(void);
extern void configure_tc3(void);


uint8_t	sent_packets=0;
bool    ready_to_send_new_packet=true; //rtc
#if BOARD == SAMR21_XPLAINED_PRO
uint8_t *edbg_eui_read_eui64(void);
#endif

/*---------------------------------------------------------------------------*/


/**
 * @brief return node id
 *
 * TODO: refactoring
 * */
uint16_t get_nodeid(void)
{
	static uint16_t nodeid;
    volatile uip_ds6_addr_t *lladdr;

    lladdr = uip_ds6_get_link_local(-1);

    nodeid = lladdr->ipaddr.u8[14];
    nodeid <<= 8;
    nodeid |= lladdr->ipaddr.u8[15];
	return nodeid;
}

int
main(void)
{
	
  node_id_restore();
  /* init system: clocks, board etc */
  system_init();
  sio2host_init();

  system_interrupt_enable_global();

  flash_init();

  delay_init();

  /* Initialize Contiki and our processes. */

  #ifdef LOW_POWER_MODE
  configure_tc3();
  #else
  clock_init();
  #endif


  process_init();
  ctimer_init();

  rtimer_init();
  process_start(&etimer_process, NULL);

  /* Set MAC address and node ID */
#ifdef NODEID
  node_id = NODEID;
#endif /* NODE_ID */


  printf("\r\n\n\n\n Starting the SmartConnect-6LoWPAN \r\n Platform : Atmel IoT device \r\n");
  print_reset_causes();

  netstack_init();
  
  #if BOARD == SAMR21_XPLAINED_PRO
  eui64 = edbg_eui_read_eui64();
  SetIEEEAddr(eui64);
  #else
  SetIEEEAddr(node_mac);
  #endif

  set_link_addr();
  rf_set_channel(RF_CHANNEL);
  printf("\r\n Configured RF channel: %d\r\n", rf_get_channel());
  energest_init();

  ENERGEST_ON(ENERGEST_TYPE_CPU);
  if(node_id > 0) {
    printf(" Node id %u.\r\n", node_id);
  } else {
    printf(" Node id not set.\r\n");
  }

  /* Setup nullmac-like MAC for 802.15.4 */
#if SAMD
  memcpy(&uip_lladdr.addr, node_mac, sizeof(uip_lladdr.addr));
#else
  memcpy(&uip_lladdr.addr, eui64, sizeof(uip_lladdr.addr));
#endif

  queuebuf_init();
  printf(" %s %lu %d\r\n",
         NETSTACK_RDC.name,
         (uint32_t) (CLOCK_SECOND / (NETSTACK_RDC.channel_check_interval() == 0 ? 1:
                         NETSTACK_RDC.channel_check_interval())),
         RF_CHANNEL);

  process_start(&tcpip_process, NULL);
  printf(" IPv6 Address: ");
  {
    uip_ds6_addr_t *lladdr;
    int i;
    lladdr = uip_ds6_get_link_local(-1);
    for(i = 0; i < 7; ++i) {
      printf("%02x%02x:", lladdr->ipaddr.u8[i * 2],
             lladdr->ipaddr.u8[i * 2 + 1]);
    }
    printf("%02x%02x\r\n", lladdr->ipaddr.u8[14], lladdr->ipaddr.u8[15]);
  }

  {
    uip_ipaddr_t ipaddr;
    int i;
    uip_ip6addr(&ipaddr, 0xfc00, 0, 0, 0, 0, 0, 0, 0);
    uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
    uip_ds6_addr_add(&ipaddr, 0, ADDR_TENTATIVE);
    printf("Tentative global IPv6 address ");
    for(i = 0; i < 7; ++i) {
      printf("%02x%02x:",
             ipaddr.u8[i * 2], ipaddr.u8[i * 2 + 1]);
    }
    printf("%02x%02x\r\n",
           ipaddr.u8[7 * 2], ipaddr.u8[7 * 2 + 1]);
  }
  print_processes(autostart_processes);
  /* set up AES key */
#if ((THSQ_CONF_NETSTACK) & THSQ_CONF_AES)
#ifndef NETSTACK_AES_KEY
	#error Please define NETSTACK_AES_KEY!
#endif /* NETSTACK_AES_KEY */
  {
    const uint8_t key[] = NETSTACK_AES_KEY;
    netstack_aes_set_key(key);
  }
  printf("AES encryption is enabled\r\n");
#else /* ((THSQ_CONF_NETSTACK) & THSQ_CONF_AES) */
  printf("\r\n Warning: AES encryption is disabled\r\n");
#endif /* ((THSQ_CONF_NETSTACK) & THSQ_CONF_AES) */

  autostart_start(autostart_processes);
  
  while(1){
    int r = 0;
  do {
     r = process_run();
    } while(r > 0);
    
  }
}

/*---------------------------------------------------------------------------*/
static void
set_link_addr(void)
{
  linkaddr_t addr;
  unsigned int i;

  memset(&addr, 0, sizeof(linkaddr_t));
#if UIP_CONF_IPV6
#if SAMR21
  memcpy(addr.u8, eui64, sizeof(addr.u8));
#else
  memcpy(addr.u8, node_mac, sizeof(addr.u8));
#endif
#else   /* UIP_CONF_IPV6 */
  if(node_id == 0) {
    for(i = 0; i < sizeof(linkaddr_t); ++i) {
#if SAMR21
      addr.u8[i] = eui64 [7 - i];
#else
	 addr.u8[i] = node_mac [7 - i];
#endif
    }
  } else {
    addr.u8[0] = node_id & 0xff;
    addr.u8[1] = node_id >> 8;
  }
#endif  /* UIP_CONF_IPV6 */
  linkaddr_set_node_addr(&addr);
  printf("Link layer addr ");
  for(i = 0; i < sizeof(addr.u8) - 1; i++) {
    printf("%u:", addr.u8[i]);
  }
  printf("%u, ", addr.u8[i]);
  for(i = 0; i < sizeof(addr.u8) - 1; i++) {
    printf("%02x:", addr.u8[i]);
  }
  printf("%02xr\n", addr.u8[i]);
}
/*---------------------------------------------------------------------------*/
static void
print_processes(struct process * const processes[])
{
  printf("Starting");
  while(*processes != NULL) {
    printf(" %s", (*processes)->name);
    processes++;
  }
  putchar('\n');
}

/*---------------------------------------------------------------------------*/
#define REG_RCAUSE     (0x40000438U)
/**
 * \brief      Print out all known reset causes
 * \return     Returns nothing
 */
static void
print_reset_causes(void)
{
  uint8_t rcause = *((uint8_t *)REG_RCAUSE);
  printf("Last reset cause: ");
  if(rcause & (1 << 6)) {
    printf("System Reset Request\r\n");
  }
  if(rcause & (1 << 5)) {
    printf("Watchdog Reset\r\n");
  }
  if(rcause & (1 << 4)) {
    printf("External Reset\r\n");
  }
  if(rcause & (1 << 2)) {
    printf("Brown Out 33 Detector Reset\r\n");
  }
  if(rcause & (1 << 1)) {
    printf("Brown Out 12 Detector Reset\r\n");
  }
  if(rcause & (1 << 0)) {
    printf("Power-On Reset\r\n");
  }
}
