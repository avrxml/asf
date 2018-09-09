/**
* \file  miwi_config.h
*
* \brief Configuration for MiWi Protocol
*
* Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries. 
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
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, 
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
*/
/*
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
*/
#ifndef __CONFIG_APP_H_
#define __CONFIG_APP_H_

/*********************************************************************/
// following codes defines the platforms as well as the hardware 
// configuration
/*********************************************************************/

/*********************************************************************/
// ENABLE_CONSOLE will enable the print out on the hyper terminal
// this definition is very helpful in the debugging process
/*********************************************************************/
#define ENABLE_CONSOLE

/*********************************************************************/
// FRAME_COUNTER_UPDATE_INTERVAL defines the NVM update interval for
// frame counter, when security is enabled. The same interval will be
// added to the frame counter read from NVM when Network Freezer
// feature is enabled.
/*********************************************************************/
#define FRAME_COUNTER_UPDATE_INTERVAL 1024

/*********************************************************************/
// MY_ADDRESS_LENGTH defines the size of wireless node permanent 
// address in byte. This definition is not valid for IEEE 802.15.4
// compliant RF transceivers.
/*********************************************************************/
#define MY_ADDRESS_LENGTH       8

/*********************************************************************/
// EUI_x defines the xth byte of permanent address for the wireless
// node
/*********************************************************************/
#define EUI_7 0x11
#define EUI_6 0x66
#define EUI_5 0x55
#define EUI_4 0x44
#define EUI_3 0x33
#define EUI_2 0x22
#define EUI_1 0x11
#define EUI_0 0xAB

/*********************************************************************/
// TX_BUFFER_SIZE defines the maximum size of application payload
// which is to be transmitted
/*********************************************************************/
#define TX_BUFFER_SIZE 40

/*********************************************************************/
// RX_BUFFER_SIZE defines the maximum size of application payload
// which is to be received
/*********************************************************************/
#define RX_BUFFER_SIZE 66

/*********************************************************************/
// MY_PAN_ID defines the PAN identifier. Use 0xFFFF if prefer a 
// random PAN ID.
/*********************************************************************/
#define MY_PAN_ID                       0x1234

/*********************************************************************/
// ENABLE_ED_SCAN will enable the device to do an energy detection scan
// to find out the channel with least noise and operate on that channel
/*********************************************************************/
//#define ENABLE_ED_SCAN

// Constants Validation
#if !defined(PROTOCOL_MESH)
    #error "PROTOCOL_MESH Microchip proprietary protocol must be defined for this wireless application."
#endif

#if defined(PROTOCOL_MESH)
#if !defined(PAN_COORDINATOR) && !defined(COORDINATOR) && !defined(ENDDEVICE)
#error "One device type for MiWi Mesh need to be defined."
#endif
#endif

#if MY_ADDRESS_LENGTH > 8
    #error "Maximum address length is 8"
#endif

#if MY_ADDRESS_LENGTH < 2
    #error "Minimum address length is 2"
#endif

#define IEEE_802_15_4
#undef MY_ADDRESS_LENGTH
#define MY_ADDRESS_LENGTH 8

#if (RX_BUFFER_SIZE > 127)
#error RX BUFFER SIZE too large. Must be <= 127.
#endif

#if (TX_BUFFER_SIZE > 127)
#error TX BUFFER SIZE too large. Must be <= 127.
#endif

#if (RX_BUFFER_SIZE < 10)
#error RX BUFFER SIZE too small. Must be >= 10.
#endif

#if (TX_BUFFER_SIZE < 10)
#error TX BUFFER SIZE too small. Must be >= 10.
#endif

#endif
