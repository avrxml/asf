/**
* \file  miwi_p2p_pds.c
*
* \brief MiWi P2P & Star PDS Definitions
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

#if defined(ENABLE_NETWORK_FREEZER)
/************************ HEADERS **********************************/
#include "system.h"
#include "string.h"
#include "miwi_config.h"          //MiWi Application layer configuration file
#include "miwi_config_p2p.h"      //MiWi Protocol layer configuration file
#include "miwi_api.h"
#include <pdsDataServer.h>
#include <wlPdsTypesConverter.h>
#include <wlPdsMemIds.h>
#include <D_Nv.h>

#define PROGMEM_DECLARE(x) const x

#ifdef ENABLE_SECURITY
extern API_UINT32_UNION OutgoingFrameCounter;
#endif
#if defined(IEEE_802_15_4)
extern API_UINT16_UNION  myPANID;
#endif
extern uint8_t ConnMode;
extern uint8_t LatestConnection;

#ifdef ENABLE_SECURITY
PDS_DECLARE_ITEM(PDS_OUTGOING_FRAME_COUNTER_ID, PDS_OUTGOING_FRAME_COUNTER_ITEM_SIZE, &OutgoingFrameCounter, NULL, NO_ITEM_FLAGS);
#endif
PDS_DECLARE_ITEM(PDS_PANID_ID, PDS_PANID_ITEM_SIZE, &myPANID, NULL, NO_ITEM_FLAGS);
PDS_DECLARE_ITEM(PDS_CURRENT_CHANNEL_ID, PDS_CURRENT_CHANNEL_ITEM_SIZE, &currentChannel, NULL, NO_ITEM_FLAGS);
PDS_DECLARE_ITEM(PDS_CONNECTION_MODE_ID, PDS_CONNECTION_MODE_ITEM_SIZE, &ConnMode, NULL, NO_ITEM_FLAGS);
PDS_DECLARE_ITEM(PDS_CONNECTION_TABLE_ID, PDS_CONNECTION_TABLE_ITEM_SIZE * CONNECTION_SIZE, ConnectionTable, NULL, NO_ITEM_FLAGS);
PDS_DECLARE_ITEM(PDS_EDC_ID, PDS_EDC_ITEM_SIZE, &conn_size, NULL, NO_ITEM_FLAGS);
#if defined (PROTOCOL_STAR)
PDS_DECLARE_ITEM(PDS_ROLE_ID, PDS_ROLE_ITEM_SIZE, &role, NULL, NO_ITEM_FLAGS);
PDS_DECLARE_ITEM(PDS_MYINDEX_ID, PDS_MYINDEX_ITEM_SIZE, &myConnectionIndex_in_PanCo, NULL, NO_ITEM_FLAGS);
#endif
#endif