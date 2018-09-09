/**
* \file  wlPdsMemIds.h
*
* \brief PDS file and directory memory identificators definitions
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

#ifndef _WLPDSMEMIDS_H_
#define _WLPDSMEMIDS_H_

/******************************************************************************
                               Includes section
******************************************************************************/
#include "compiler.h"

#if defined(PROTOCOL_MESH)
// Required to be consecutive starting from 0
typedef enum {
	PDS_NULL_ID = 0,    // To be maintained as the first entry
	PDS_GENERAL_INFO_ID,
#ifdef MESH_SECURITY
	PDS_SECURITY_KEY_ID,
	PDS_SECURITY_COUNTER_ID,
#endif
	PDS_BLOOM_VALUE_ID,
#if defined(ENABLE_FREQUENCY_AGILITY)
	PDS_CHANNEL_UPDATE_ID,
#endif
#if defined(PAN_COORDINATOR)
	PDS_COMM_DEVICE_TABLE_ID,
	PDS_COORDINATOR_TABLE_ID,
#endif
#if defined(PAN_COORDINATOR) || defined (COORDINATOR)
	PDS_DEVICE_TABLE_NONSLEEP_ID,
	PDS_DEVICE_TABLE_SLEEP_ID,
	PDS_ROUTE_TABLE_COORD_ID,
	PDS_ROUTE_TABLE_HOP_ID,
#endif
	PDS_MAX_ID          // To be maintained as the last entry
} miwi_mesh_pds_id_t;

#else
// Required to be consecutive starting from 0
typedef enum {
    PDS_NULL_ID = 0,    // To be maintained as the first entry
    PDS_OUTGOING_FRAME_COUNTER_ID,
    PDS_PANID_ID,
    PDS_CURRENT_CHANNEL_ID,
    PDS_CONNECTION_MODE_ID,
    PDS_CONNECTION_TABLE_ID,
    PDS_EDC_ID,
#if defined (PROTOCOL_STAR)
    PDS_ROLE_ID,
    PDS_MYINDEX_ID,
#endif
    PDS_MAX_ID          // To be maintained as the last entry
} miwi_p2pstar_pds_id_t;

#endif
/* Total files and directories amount supported by PDS */
#define PDS_ITEM_AMOUNT                      PDS_MAX_ID
#define PDS_DIRECTORIES_AMOUNT               1
#define PDS_ITEM_IDS_AMOUNT                 (PDS_ITEM_AMOUNT +  PDS_DIRECTORIES_AMOUNT)

#define PDS_ITEM_MASK_SIZE                  (PDS_ITEM_AMOUNT / 8U + (PDS_ITEM_AMOUNT % 8U ? 1U : 0U))

/* Directory mask */
#define  PDS_DIRECTORY_MASK                 0x4000U
#define  MIWI_ALL_MEMORY_MEM_ID             0x4001U

#define PDS_ALL_EXISTENT_MEMORY             0x4002U



/******************************************************************************
                               Types section
******************************************************************************/


/*! Type of PDS file or directory unique identifier. */
typedef uint16_t PDS_MemId_t;

/* Memory identifier record pointer */
typedef const PDS_MemId_t * PDS_MemIdRec_t;

#endif // _WLPDSMEMIDS_H_
/* eof wlPdsMemIds.h */

