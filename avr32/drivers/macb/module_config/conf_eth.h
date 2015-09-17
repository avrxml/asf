/*****************************************************************************
 *
 * \file
 *
 * \brief Configuration template file for the AVR UC3 MACB software driver.
 *
 * To be used as a reference, copy/paste to each project using the MAC software
 * driver then customize according to the application.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef __CONF_ETH_H__
#define __CONF_ETH_H__

#include "board.h"


/*! Number of receive buffers */
#define ETHERNET_CONF_NB_RX_BUFFERS        20

/*! USE_RMII_INTERFACE must be defined as 1 to use an RMII interface, or 0
to use an MII interface. */
#define ETHERNET_CONF_USE_RMII_INTERFACE   1

/*! Number of Transmit buffers */
#define ETHERNET_CONF_NB_TX_BUFFERS        10

/*! Size of each Transmit buffer. */
#define ETHERNET_CONF_TX_BUFFER_SIZE       512

/*! Clock definition */
#define ETHERNET_CONF_SYSTEM_CLOCK         48000000

/*! this MAC address is an Atmel Corporation example */
//! @{
#define ETHERNET_CONF_ETHADDR0    0x00
#define ETHERNET_CONF_ETHADDR1    0x04
#define ETHERNET_CONF_ETHADDR2    0x25
#define ETHERNET_CONF_ETHADDR3    0x1C
#define ETHERNET_CONF_ETHADDR4    0xA0
#define ETHERNET_CONF_ETHADDR5    0x02
//! @}

/*! EVK1100 IP Address (192.168.0.2) */
//! @{
#define ETHERNET_CONF_IPADDR0          192
#define ETHERNET_CONF_IPADDR1          168
#define ETHERNET_CONF_IPADDR2            0
#define ETHERNET_CONF_IPADDR3            2
//! @}

/*! HOST IP Address (192.168.0.1) */
//! @{
#define ETHERNET_CONF_GATEWAY_ADDR0    192
#define ETHERNET_CONF_GATEWAY_ADDR1    168
#define ETHERNET_CONF_GATEWAY_ADDR2      0
#define ETHERNET_CONF_GATEWAY_ADDR3      1
//! @}

/*! The network mask being used. */
//! @{
#define ETHERNET_CONF_NET_MASK0        255
#define ETHERNET_CONF_NET_MASK1        255
#define ETHERNET_CONF_NET_MASK2        255
#define ETHERNET_CONF_NET_MASK3          0
//! @}

/*! Use Auto negotiation to get speed and duplex */
#define ETHERNET_CONF_AN_ENABLE            1

/*! Use auto cross capability. Used by default. */
#define ETHERNET_CONF_AUTO_CROSS_ENABLE    1

#if !ETHERNET_CONF_AUTO_CROSS_ENABLE
  /*! if not using auto cross capability, define if using cross link or not */
  #define ETHERNET_CONF_CROSSED_LINK       0
#endif

#if !ETHERNET_CONF_AN_ENABLE
  /*! if not using auto negotiation */
  #define ETHERNET_CONF_USE_100MB          1
  #define ETHERNET_CONF_USE_FULL_DUPLEX    1
#endif

/*! set to 1 if Phy status changes handle an interrupt */
#define ETHERNET_CONF_USE_PHY_IT           0

#if ETHERNET_CONF_USE_PHY_IT == 1
  #if !defined(EXTPHY_MACB_INTERRUPT_PIN) 
  #warning The MACB configuration does not exist in the board definition file. Using default settings.
  #define EXTPHY_MACB_INTERRUPT_PIN   AVR32_PIN_PA01 /* MACB Interrupt pin, dummy default value */
#endif
#endif


#endif // __CONF_ETH_H__
