
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

#ifndef __CONTIKI_CONF_H__
#define __CONTIKI_CONF_H__

#include "thsq-conf-const.h"
#include "samr21_xplained_pro.h"
#ifdef INCLUDE_SUBPLATFORM_CONF
#include "subplatform-conf.h"
#else /* INCLUDE_SUBPLATFORM_CONF */
#include "platform-conf.h"
#endif /* INCLUDE_SUBPLATFORM_CONF */
/************************************************************************/
/* Radio configuration                                                  */
/************************************************************************/
#include "rf233.h"

#ifndef RF_CHANNEL
#define RF_CHANNEL                            11
#define DATA_RATE OQPSK_SIN_250
#endif

#define IEEE802154_CONF_PANID                 0xABCD

#ifndef NETSTACK_CONF_RADIO
#define NETSTACK_CONF_RADIO                   rf233_radio_driver
#endif

#define NETSTACK_RADIO_MAX_PAYLOAD_LEN        125

/************************************************************************/
/* Radio Duty Cycle (RDC) config options                                */
/************************************************************************/

/* RDC driver Options 
 * NULLRDC - No RDC option enabled                             
 * CONTIKIMAC - RDC enabled; Low power mode enabled             
 */
#if (RDC == NULLRDC)
#define NETSTACK_CONF_RDC           nullrdc_driver
#elif (RDC == CONTIKIMAC)
#define NETSTACK_CONF_RDC           contikimac_driver
#endif

/************************************************************************/
/* Sending IEEE 802.15.4 ACK frames from HW                             */
/* NETSTACK_CONF_HWACK 1 - Enables sending acks from Hardware.          */
/* Also enables doing CSMA backoff and Frame Retry mechanism in HW.     */
/************************************************************************/
#if (HW_ACK == 1) && (MAC == NULLMAC) && (RDC == NULLRDC)
#define NULLRDC_CONF_802154_AUTOACK_HW       1
#define HW_CSMA_FRAME_RETRIES 1
#elif (HW_ACK == 0)
#define NULLRDC_CONF_802154_AUTOACK_HW       0
#elif (HW_ACK == 1)
#error "Please note HWACK should be enabled only when MAC is NULLMAC and RDC is NULLRDC"
#endif

/* l
 * For ContikiMAC Channel Check rate has to be defined.
 * Values allowed for NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE are 2,4,8
 */
#if (RDC == CONTIKIMAC)
#ifndef NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE
#define NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE 8
#endif
#endif

/* For sending 802.15.4 ACK from SW */
#if !(HW_ACK == 1)
#define NULLRDC_CONF_802154_AUTOACK         1
#define NULLRDC_CONF_SEND_802154_ACK        1
/* radio driver blocks until ACK is received */
#define NULLRDC_CONF_ACK_WAIT_TIME          (50 * RTIMER_SECOND/1000)
#endif

/************************************************************************/
/* IEEE 802.15.4 MAC config options                                     */
/************************************************************************/

/* 
 * NULLMAC - No CSMA option enabled                                     
 * CSMAMAC - CSMA option enabled                                        
 */
#if (MAC == NULLMAC)
#define NETSTACK_CONF_MAC           nullmac_driver
#elif (MAC == CSMAMAC)
#define NETSTACK_CONF_MAC           csma_driver
#endif
/* 
 * By default stack will use framer_802154 which frames the packet as IEEE 802.15.4 packets
 * Other framers are not supported
 */
#define NETSTACK_CONF_FRAMER        framer_802154

/************************************************************************/
/* 6LoWPAN Adaptation Layer configuration                               */
/************************************************************************/

/*
 * NETSTACK_CONF_NETWORK supports only 6LoWPAN drivers.
 * Other drivers are not supported.
 */
#define NETSTACK_CONF_NETWORK                 sicslowpan_driver
/* IP header compression supported mode */
#define SICSLOWPAN_CONF_COMPRESSION           SICSLOWPAN_COMPRESSION_HC06

/* 6LoWPAn Fragmentation and Reassembly config options */
#define SICSLOWPAN_CONF_FRAG                  1

#if SICSLOWPAN_CONF_FRAG

/* MAX time 6LoWPAN reassembly handler holds the fragmented buffers in 
 * reassembly buffer 
 */
#define REASSEMBLY_TIMEOUT                    10 * CLOCK_SECOND

/* Support for having multiple reassembly buffers.
 * This enables parallel reassembly of 6LoWPAN packets
 */
#define MUL_REASSEMBLY                        1

/* No. of reassembly buffers. Default value is 4 */
#if MUL_REASSEMBLY
#define NUM_REASM_BUFS                      4
#else
#define NUM_REASM_BUFS                      1
#endif
#endif 
/************************************************************************/


/************************************************************************/
/* UIP Layer configurations                                             */
/************************************************************************/

#define UIP_CONF_IPV6                         1

#if UIP_CONF_IPV6
/* Link address size is by default 8 bytes, since 2 bytes address is not supported */
#define LINKADDR_CONF_SIZE                    8

/* Enabling ICMPv6 protocol */
#define UIP_CONF_ICMP6                        1
/* Enabling UDP layer */
#define UIP_CONF_UDP                          1
/* Enabling TCP layer */
#define UIP_CONF_TCP                          1
/* Enabling RPL - Routing protocol */
#define UIP_CONF_IPV6_RPL                     1
/* Enabling UDP checksums support */
#define UIP_CONF_UDP_CHECKSUMS                1

#endif

/* MAX IP packet size */
#ifndef UIP_CONF_BUFFER_SIZE
#define UIP_CONF_BUFFER_SIZE                  400
#endif

/* MAX no. of routes to be added in route table */
#ifndef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES                   8
#endif

/* MAX no. of neighbor table entries */
#ifndef NBR_TABLE_CONF_MAX_NEIGHBORS
#define NBR_TABLE_CONF_MAX_NEIGHBORS          20
#endif

#define ARCH_DOESNT_NEED_ALIGNED_STRUCTS      1
#define UIP_ARCH_ADD32                        0
#define UIP_ARCH_CHKSUM                       0
#define UIP_ARCH_IPCHKSUM                     0
#define UIP_CONF_LL_802154                    1
#define UIP_CONF_LLH_LEN                      0

/************************************************************************/

/************************************************************************/
/* OS related config options                                            */
/************************************************************************/

/* Max Number of buffer to stored in a Queue */
#define QUEUEBUF_CONF_NUM                     12

/* A trick to resolve a compilation error with IAR. */
#ifdef __ICCARM__
#define UIP_CONF_DS6_AADDR_NBU                1
#endif

#define LED_UDP_PORT                          8840

#endif /* CONTIKI_CONF_H */
