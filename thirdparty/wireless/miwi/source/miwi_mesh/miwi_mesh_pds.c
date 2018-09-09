/**
* \file  miwi_mesh_pds.c
*
* \brief MiWi Mesh PDS Definitions
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
#include "string.h"
#include "miwi_config_mesh.h"
#include "miwi_api.h"
#include <pdsDataServer.h>
#include <wlPdsTypesConverter.h>
#include <wlPdsMemIds.h>
#include <D_Nv.h>

#define PROGMEM_DECLARE(x) const x

/******************************************************************************
                    Extern definitions
******************************************************************************/
extern CommDeviceTable_t commnDeviceTable[MAX_NUMBER_OF_DEVICES_IN_NETWORK];
extern DeviceTable_t deviceTable[NUM_OF_NONSLEEPING_ENDDEVICES];
extern SleepDeviceTable_t sleepdeviceTable[NUM_OF_SLEEPING_ENDDEVICES];
extern CoordinatorTable_t coordinatorTable[NUM_OF_COORDINATORS];
extern CoordRouteTable_t coordRouteTable[NUM_OF_COORDINATORS];
extern CoordHopCount_t coordHopCount[HOP_TABLE_COUNT];
extern uint8_t networkKey[SECURITY_KEY_SIZE];
extern uint8_t bloomFilterValue[BLOOM_FILTER_SIZE];
#if defined(ENABLE_FREQUENCY_AGILITY)
extern uint8_t newChannelToUpdate;
#endif

/******************************************************************************
                    Files definitions
******************************************************************************/
PDS_DECLARE_ITEM(PDS_GENERAL_INFO_ID, GENERAL_INFO_ITEM_SIZE, &genInfoMem, fillGeneralInformation, NO_ITEM_FLAGS);
#ifdef MESH_SECURITY
PDS_DECLARE_ITEM(PDS_SECURITY_KEY_ID, PDS_SECURITY_KEY_ITEM_SIZE, &networkKey, NULL, NO_ITEM_FLAGS);
PDS_DECLARE_ITEM(PDS_SECURITY_COUNTER_ID, PDS_SECURITY_COUNTER_ITEM_SIZE, &meshOutgoingFrameCounter, NULL, NO_ITEM_FLAGS);
#endif
PDS_DECLARE_ITEM(PDS_BLOOM_VALUE_ID, PDS_BLOOM_VALUE_ITEM_SIZE, &bloomFilterValue, NULL, NO_ITEM_FLAGS);
#if defined(ENABLE_FREQUENCY_AGILITY)
PDS_DECLARE_ITEM(PDS_CHANNEL_UPDATE_ID, PDS_CHANNEL_UPDATE_ITEM_SIZE, &newChannelToUpdate, NULL, NO_ITEM_FLAGS);
#endif
#if defined(PAN_COORDINATOR)
PDS_DECLARE_ITEM(PDS_COMM_DEVICE_TABLE_ID, PDS_COMM_DEVICE_TABLE_ITEM_SIZE*MAX_NUMBER_OF_DEVICES_IN_NETWORK, &commnDeviceTable, NULL, SIZE_MODIFICATION_ALLOWED);
PDS_DECLARE_ITEM(PDS_COORDINATOR_TABLE_ID, PDS_COORDINATOR_TABLE_ITEM_SIZE*NUM_OF_COORDINATORS, &coordinatorTable, NULL, SIZE_MODIFICATION_ALLOWED);
#endif
#if defined(PAN_COORDINATOR) || defined (COORDINATOR)
PDS_DECLARE_ITEM(PDS_DEVICE_TABLE_NONSLEEP_ID, PDS_DEVICE_TABLE_NONSLEEP_ITEM_SIZE*NUM_OF_NONSLEEPING_ENDDEVICES, &deviceTable, NULL, SIZE_MODIFICATION_ALLOWED);
PDS_DECLARE_ITEM(PDS_DEVICE_TABLE_SLEEP_ID, PDS_DEVICE_TABLE_SLEEP_ITEM_SIZE*NUM_OF_SLEEPING_ENDDEVICES, &sleepdeviceTable, NULL, SIZE_MODIFICATION_ALLOWED);
PDS_DECLARE_ITEM(PDS_ROUTE_TABLE_COORD_ID, PDS_ROUTE_TABLE_COORD_ITEM_SIZE*NUM_OF_COORDINATORS, &coordRouteTable, NULL, SIZE_MODIFICATION_ALLOWED);
PDS_DECLARE_ITEM(PDS_ROUTE_TABLE_HOP_ID, PDS_ROUTE_TABLE_HOP_ITEM_SIZE*NUM_OF_COORDINATORS, &coordHopCount, NULL, SIZE_MODIFICATION_ALLOWED);
#endif

PROGMEM_DECLARE(S_Nv_ItemId_t MiWiItemIdsTable[]) =
{
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
    PDS_ROUTE_TABLE_HOP_ID
#endif
};

/* MiWi All Memory Memory Descriptor */
PDS_DECLARE_DIR(PDS_DirDescr_t MiWiFullParamsDirDescr) =
{
  .list       = MiWiItemIdsTable,
  .filesCount = ARRAY_SIZE(MiWiItemIdsTable),
  .memoryId   = MIWI_ALL_MEMORY_MEM_ID,
};
#endif

