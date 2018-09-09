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

#ifndef __MIWI_CONFIG_H_
#define __MIWI_CONFIG_H_

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
// ENABLE_NETWORK_FREEZER enables the network freezer feature, which
// stores critical network information into non-volatile memory, so
// that the protocol stack can recover from power loss gracefully.
// The network infor can be saved in data EPROM of MCU, external 
// EEPROM or programming space, if enhanced flash is used in MCU.
// Network freezer feature needs definition of NVM kind to be 
// used, which is specified in HardwareProfile.h
/*********************************************************************/
//#define ENABLE_NETWORK_FREEZER

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
#define EUI_6 0x22
#define EUI_5 0x33
#define EUI_4 0x44
#define EUI_3 0x55
#define EUI_2 0x66
#define EUI_1 0x77
#define EUI_0 0x88

/*********************************************************************/
// TX_BUFFER_SIZE defines the maximum size of application payload
// which is to be transmitted
/*********************************************************************/
#define TX_BUFFER_SIZE 40

/*********************************************************************/
// RX_BUFFER_SIZE defines the maximum size of application payload
// which is to be received
/*********************************************************************/
#define RX_BUFFER_SIZE 40

/*********************************************************************/
// MY_PAN_ID defines the PAN identifier. Use 0xFFFF if prefer a 
// random PAN ID.
/*********************************************************************/
#define MY_PAN_ID                       0x1234


/*********************************************************************/
// ADDITIONAL_NODE_ID_SIZE defines the size of additional payload
// will be attached to the P2P Connection Request. Additional payload 
// is the information that the devices what to share with their peers
// on the P2P connection. The additional payload will be defined by 
// the application and defined in main.c
/*********************************************************************/
#define ADDITIONAL_NODE_ID_SIZE   1


/*********************************************************************/
// P2P_CONNECTION_SIZE defines the maximum P2P connections that this 
// device allowes at the same time. 
/*********************************************************************/
#define CONNECTION_SIZE             5


/*********************************************************************/
// TARGET_SMALL will remove the support of inter PAN communication
// and other minor features to save programming space
/*********************************************************************/
//#define TARGET_SMALL


/*********************************************************************/
// ENABLE_HAND_SHAKE enables the protocol stack to hand-shake before 
// communicating with each other. Without a handshake process, RF
// transceivers can only broadcast, or hardcoded the destination address
// to perform unicast.
/*********************************************************************/
#define ENABLE_HAND_SHAKE


/*********************************************************************/
// ENABLE_SLEEP_FEATURE will enable the device to go to sleep and wake up 
// from the sleep
/*********************************************************************/
//#define ENABLE_SLEEP_FEATURE


/*********************************************************************/
// ENABLE_ED_SCAN will enable the device to do an energy detection scan
// to find out the channel with least noise and operate on that channel
/*********************************************************************/
//#define ENABLE_ED_SCAN


/*********************************************************************/
// ENABLE_ACTIVE_SCAN will enable the device to do an active scan to 
// to detect current existing connection. 
/*********************************************************************/
//#define ENABLE_ACTIVE_SCAN


/*********************************************************************/
// ENABLE_SECURITY will enable the device to encrypt and decrypt
// information transferred
/*********************************************************************/
#define ENABLE_SECURITY


/*********************************************************************/
// ENABLE_INDIRECT_MESSAGE will enable the device to store the packets
// for the sleeping devices temporily until they wake up and ask for
// the messages
/*********************************************************************/
//#define ENABLE_INDIRECT_MESSAGE


/*********************************************************************/
// ENABLE_BROADCAST will enable the device to broadcast messages for
// the sleeping devices until they wake up and ask for the messages
/*********************************************************************/
//#define ENABLE_BROADCAST


/*********************************************************************/
// RFD_WAKEUP_INTERVAL defines the wake up interval for RFDs in second.
// This definition is for the FFD devices to calculated various
// timeout. RFD depends on the setting of the watchdog timer to wake 
// up, thus this definition is not used.
/*********************************************************************/
#define RFD_WAKEUP_INTERVAL     8


/*********************************************************************/
// ENABLE_FREQUENCY_AGILITY will enable the device to change operating
// channel to bypass the sudden change of noise
/*********************************************************************/
//#define ENABLE_FREQUENCY_AGILITY


// Constants Validation
    
#if !defined(PROTOCOL_P2P) && !defined(PROTOCOL_STAR)
#error "One Microchip proprietary protocol must be defined for the wireless application."
#endif

#if defined(ENABLE_FREQUENCY_AGILITY)
    #define ENABLE_ED_SCAN
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

#if defined(ENABLE_NETWORK_FREEZER)
    #define ENABLE_NVM
#endif

#if defined(ENABLE_ACTIVE_SCAN) && defined(TARGET_SMALL)
    #error  Target_Small and Enable_Active_Scan cannot be defined together 
#endif

#if defined(ENABLE_INDIRECT_MESSAGE) && !defined(RFD_WAKEUP_INTERVAL)
    #error "RFD Wakeup Interval must be defined if indirect message is enabled"
#endif

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

#if (CONNECTION_SIZE > 0xFE)
    #error NETWORK TABLE SIZE too large.  Must be < 0xFF.
#endif

#endif
