/**************************************************************************
 *
 * \file
 *
 * \brief Ethernet module configuration file.
 *
 * This file contains the possible external configuration of the Ethernet module.
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _CONF_ETH_H_
#define _CONF_ETH_H_

/*! location of net config file on the file system */
#define ETHERNET_CONFIG_FILE "A:/CFG/net.txt"

/*! Number of receive buffers. Max Ethernet frame size is 1526. A Rx buffer is
128 Bytes long. So 12 Rx buffers are necessary to store one max sized frame.
Multiply that by 2 for performance. */
#define ETHERNET_CONF_NB_RX_BUFFERS        24

/*! USE_RMII_INTERFACE must be defined as 1 to use an RMII interface, or 0
to use an MII interface. */
#define ETHERNET_CONF_USE_RMII_INTERFACE   1

/*! Number of Transmit buffers */
#define ETHERNET_CONF_NB_TX_BUFFERS        10

/*! Size of each Transmit buffer. */
#define ETHERNET_CONF_TX_BUFFER_SIZE       512

/*! Clock definition */
#define ETHERNET_CONF_SYSTEM_CLOCK         60000000

/*! Allow Auto negotiation */
#define ETHERNET_CONF_AN_ENABLE            1

/*! Use auto cross capability. Used by default. */
#define ETHERNET_CONF_AUTO_CROSS_ENABLE    1
/*! Use direct cable. Ignored when ETHERNET_CONF_AN_ENABLE and
    ETHERNET_CONF_AUTO_CROSS_ENABLE are enabled. */
#define ETHERNET_CONF_CROSSED_LINK         0

/* Ethernet default parameters */
/*! MAC address definition.  The MAC address must be unique on the network. */
#define ETHERNET_CONF_ETHADDR0   0x00
#define ETHERNET_CONF_ETHADDR1   0x04
#define ETHERNET_CONF_ETHADDR2   0x25
#define ETHERNET_CONF_ETHADDR3   0x1C
#define ETHERNET_CONF_ETHADDR4   0xA0
#define ETHERNET_CONF_ETHADDR5   0x02

/*! The IP address being used. */
#define ETHERNET_CONF_IPADDR0          192
#define ETHERNET_CONF_IPADDR1          168
#define ETHERNET_CONF_IPADDR2            0
#define ETHERNET_CONF_IPADDR3            2

/*! The gateway address being used. */
#define ETHERNET_CONF_GATEWAY_ADDR0    192
#define ETHERNET_CONF_GATEWAY_ADDR1    168
#define ETHERNET_CONF_GATEWAY_ADDR2      0
#define ETHERNET_CONF_GATEWAY_ADDR3      1

/*! The network mask being used. */
#define ETHERNET_CONF_NET_MASK0        255
#define ETHERNET_CONF_NET_MASK1        255
#define ETHERNET_CONF_NET_MASK2        255
#define ETHERNET_CONF_NET_MASK3          0

/*! set to 1 if Phy status changes handle an interrupt */
#define ETHERNET_CONF_USE_PHY_IT           0

#endif
