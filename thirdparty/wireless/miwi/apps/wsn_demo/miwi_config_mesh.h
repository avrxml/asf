/**
* \file  miwi_config_mesh.h
*
* \brief Configuration file for MiWi Mesh Protocol.
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

#ifndef __CONFIGURE_MESH_H
#define __CONFIGURE_MESH_H

   #include "miwi_config.h"

   #if defined(PROTOCOL_MESH)

    /* Channel Map for Network Operation - Configurable */
    #if (defined(PHY_AT86RF212B))
        #define CHANNEL_MAP    (1 << 2)
    /* Range for default configuration: 1 to 10
    Note: TX Power and PHY Mode Setting needs to be modified as per the 
    recommendation from Data Sheet for European band (ie.,Channel 0)*/
    #else
        #define CHANNEL_MAP    (1 << 25)
    /* Range: 11 to 26 */
    #endif

    #ifdef MESH_SECURITY
    /* Security Level Used for transmission of frames..Configurable */
    #define MESH_SECURITY_LEVEL   5
	#endif

    /* Public Key of the Network. It is preinstalled key on all MiWi Mesh Devices  */
    #define PUBLIC_KEY_DEFAULT  {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F}

    /* Network Key, Key Decided by PAN Coordinator. This is the key used by default  */
    #define NETWORK_KEY_DEFAULT {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}

	/* Interval between keep alive message transmission in seconds for coordinator */
	#define KEEP_ALIVE_COORDINATOR_SEND_INTERVAL    120

	/* Keep alive coordinator timeout in seconds */
    #define KEEP_ALIVE_COORDINATOR_TIMEOUT_IN_SEC    KEEP_ALIVE_COORDINATOR_SEND_INTERVAL * 10

	/* Interval between keep alive message transmission in seconds for RxonWhenIdle End device */
	#define KEEP_ALIVE_RXONENDDEVICE_SEND_INTERVAL  120

	/* Keep alive Rx on End device timeout in seconds */
	#define KEEP_ALIVE_RXONENDDEVICE_TIMEOUT_IN_SEC    KEEP_ALIVE_RXONENDDEVICE_SEND_INTERVAL * 10

    /* The Periodic Polling data request interval for sleeping end devices - Configurable  */
    #define DATA_REQUEST_SEND_INTERVAL       3

    /* End Device Key Alive Timeout - Configurable */
    #define RXOFF_DEVICE_TIMEOUT_IN_SEC      DATA_REQUEST_SEND_INTERVAL * 20


	#define MAX_NUMBER_OF_DEVICES_IN_NETWORK     32

    /* Capability of Device Types - Macros - Don't Change */
    #define CAPABILITY_INFO_COORD       0x05
    #define CAPABILITY_INFO_ED          0x06
    #define CAPABILITY_INFO_ED_RXON     0x07

    #define JOINWISH_ENDEVICE           0x01
    #define JOINWISH_COORD_ALONE        0x02
    #define JOINWISH_ANY                0x03


    #ifdef COORDINATOR
       /* JoinWish of Coordinator Device - Configurable */
       #define JOIN_WISH         JOINWISH_ANY
       /* Capability of of Coordinator Device - Don't change */
       #define CAPABILITY_INFO   CAPABILITY_INFO_COORD
    #else
       /* JoinWish and Capability of of End Device - Don't change */
       #define JOIN_WISH         JOINWISH_ENDEVICE
       #define CAPABILITY_INFO  CAPABILITY_INFO_ED
    #endif

	#define ROLE_UPGRADE_INTERVAL_IN_SEC     25
	#define CONNECTION_RESPONSE_WAIT_IN_SEC  5

    /* Maximum number of coordinators in the network - Configurable  */
    #define NUM_OF_COORDINATORS         64
    /* Maximum number of Non Sleep End devices in the network - Configurable  */
    #define NUM_OF_NONSLEEPING_ENDDEVICES        5
    /* Maximum number of Sleeping End devices in the network - Configurable  */
    #define NUM_OF_SLEEPING_ENDDEVICES           5
    /* The Periodic Route Update Initiation interval - Configurable  */
	#define ROUTE_UPDATE_INTERVAL       60
    /* The wait time for receiving route reply after initiating route request - Configurable*/
    #define ROUTE_REQ_WAIT_INTERVAL     5

    /* The wait time to maintain the indirect data in queue - Configurable*/
    #define INDIRECT_DATA_WAIT_INTERVAL 25

    /* The Max number of failures to reach the parent before raising failure callback  - Configurable  */
    #define ED_LINK_FAILURE_ATTEMPTS    15

    /* The time to wait for acknowledgment before next retry of the packet  - Configurable  */
    #define FRAME_ACK_WAIT_INTERVAL     5

    /* The Max number of frame retries in the network layer  - Configurable  */
    #define FRAME_RETRY                 3

    /* Number of Rebroadcast Table Entries  - Configurable  */
    #define REBROADCAST_TABLE_SIZE      10

    /* The time to hold the rebroadcasted entry in the table  - Configurable  */
    #define REBROADCAST_TIMEOUT         5

    /* The number of maximum beacon results to store during scan procedure - Configurable*/
    #define MAX_BEACON_RESULTS          5
  #endif
#endif

