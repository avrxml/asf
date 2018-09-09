/**
* \file  wlPdsTypesConverter.c
*
* \brief PDS types converter implementation.
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
#if PDS_ENABLE_WEAR_LEVELING

/******************************************************************************
                    Includes section
******************************************************************************/
#include <wlPdsBindings.h>
#include <wlPdsMemIds.h>
#include <wlPdsTypesConverter.h>
#include <string.h>
#include <assert.h>
//#include "thread_pds.h"
#include "assert.h"
#include "miwi_api.h"

/******************************************************************************
                    Definitions section
******************************************************************************/
#define COMPID "wlPdsTypesConverter"

/******************************************************************************
                    Types section
******************************************************************************/


/******************************************************************************
                    Externals section
******************************************************************************/
MeshGeneralInfotMem_t genInfoMem;

/******************************************************************************
                    Prototypes section
******************************************************************************/

static void verifyItemSizes (S_Nv_ItemId_t id, uint16_t size, uint16_t oldSize);

/******************************************************************************
                    Static variables section
******************************************************************************/


/******************************************************************************
                    Implementation section
******************************************************************************/
#if defined(PROTOCOL_MESH)
/******************************************************************************
\brief Fills General Information
******************************************************************************/
void fillGeneralInformation(void)
{
  memcpy(&genInfoMem.extendedAddress, myLongAddress, LONG_ADDR_LEN);
  MiApp_Get(SHORT_ADDRESS, (uint8_t *)&genInfoMem.networkAddress);
  MiApp_Get(PANID, (uint8_t *)&genInfoMem.panId );
  MiApp_Get(CHANNEL, (uint8_t *)&genInfoMem.logicalChannel );
  MiApp_Get(CHANNELMAP, (uint8_t *)&genInfoMem.channelMap );
  MiApp_Get(CAPABILITYINFO, (uint8_t *)&genInfoMem.capabilityInfo );
#ifdef MESH_SECURITY
  genInfoMem.nwkSecurityLevel = defaultParamsRamOnly.securityLevel;
#endif
}

/******************************************************************************
\brief Updates General Information set of stack parameters

\param[in] data - pointer to data

\returns returns true general information was updated successfully,
  false - otherwise
******************************************************************************/
static bool updateGeneralInfoSet(void *data)
{
  MeshGeneralInfotMem_t *updateGenInfo = (MeshGeneralInfotMem_t *)data;
  uint8_t *ptr = (uint8_t *)data;
  bool inconsistentFlag = true;

  /* Check network parameters for inconsistence */
  for (uint8_t i = 0U; i < sizeof(MeshGeneralInfotMem_t); i++)
    if (0xFFU != ptr[i])
    {
      inconsistentFlag = false;
      break;
    }

  if (inconsistentFlag)
    return false;

  /* Update appropriate MiWi structures */
  memcpy(myLongAddress, &updateGenInfo->extendedAddress, LONG_ADDR_LEN);
  MiApp_Set(SHORT_ADDRESS, (uint8_t *)&updateGenInfo->networkAddress);
  MiApp_Set(PANID, (uint8_t *)&updateGenInfo->panId );
  MiApp_Set(CHANNEL, (uint8_t *)&updateGenInfo->logicalChannel );
  MiApp_Set(CHANNELMAP, (uint8_t *)&genInfoMem.channelMap );
  MiApp_Set(CAPABILITYINFO, (uint8_t *)&genInfoMem.capabilityInfo );
#ifdef MESH_SECURITY
  defaultParamsRamOnly.securityLevel = updateGenInfo->nwkSecurityLevel;
#endif

  return true;
}

bool pdsIsItemUnderSecurityControl(S_Nv_ItemId_t id)
{
#ifdef PDS_SECURITY_CONTROL_ENABLE
  ItemIdToMemoryMapping_t mapItem;

  if (pdsGetItemDescr(id, &mapItem))
  {
    if (mapItem.flags & ITEM_UNDER_SECURITY_CONTROL)
      return true;
  }
#else
  // To avoid compiler warning
  (void)id;
#endif
  return false;
}

#if defined(PAN_COORDINATOR) || defined (COORDINATOR)
/******************************************************************************
\brief Updates table

\param[in] data    - pointer to data with table;
\param[in] size    - current item size;
\param[in] oldSize - last size of item
******************************************************************************/
static void updateTable(void *data, uint16_t size, uint16_t oldSize)
{
	uint8_t *ptr = (uint8_t *)data;

	if (size > oldSize)
	memset(ptr + oldSize, 0U, size - oldSize);
}
#endif

#if defined(PAN_COORDINATOR)
/******************************************************************************
\brief Updates commissioning device table

\param[in] data    - pointer to data with commissioning device table;
\param[in] size    - current item size;
\param[in] oldSize - last size of item
******************************************************************************/
static void updatecommnDevTable(void *data, uint16_t size, uint16_t oldSize)
{
	if (size == oldSize)
	return;

	updateTable(data, size, oldSize);

	if (size > oldSize)
	{
		uint8_t *ptr = (uint8_t *)data + oldSize; // points to added records;
		CommDeviceTable_t *entry = (CommDeviceTable_t *)(void *)ptr ;
		S_Nv_ReturnValue_t ret;

		// go through all new records and mark them as free
		while (oldSize < size)
		{
			memset(&entry->deviceAddress, 0xFF, LONG_ADDR_LEN);
			entry++;
			oldSize += sizeof(CommDeviceTable_t);
		}
		ret = S_Nv_Write(PDS_COMM_DEVICE_TABLE_ID, 0U, size, data);
		assert(ret == S_Nv_ReturnValue_Ok);
	}
}

/******************************************************************************
\brief Updates coordinator table

\param[in] data    - pointer to data with commissioning device table;
\param[in] size    - current item size;
\param[in] oldSize - last size of item
******************************************************************************/
static void updateCoordTable(void *data, uint16_t size, uint16_t oldSize)
{
	if (size == oldSize)
	return;

	updateTable(data, size, oldSize);

	if (size > oldSize)
	{
		uint8_t *ptr = (uint8_t *)data + oldSize; // points to added records;
		CoordinatorTable_t *entry = (CoordinatorTable_t *)(void *)ptr;
		S_Nv_ReturnValue_t ret;

		// go through all new records and mark them as free
		while (oldSize < size)
		{
			memset(entry->ieeeaddr, 0xFF, LONG_ADDR_LEN);
			entry->currentTimeOut = 0;
			entry++;
			oldSize += sizeof(CoordinatorTable_t);
		}
		ret = S_Nv_Write(PDS_COORDINATOR_TABLE_ID, 0U, size, data);
		assert(ret == S_Nv_ReturnValue_Ok);
	}
}
#endif

#if defined(PAN_COORDINATOR) || defined (COORDINATOR)
/******************************************************************************
\brief Updates non sleep devices table

\param[in] data    - pointer to data with commissioning device table;
\param[in] size    - current item size;
\param[in] oldSize - last size of item
******************************************************************************/
static void updateNonSleepDevTable(void *data, uint16_t size, uint16_t oldSize)
{
	if (size == oldSize)
	return;

	updateTable(data, size, oldSize);

	if (size > oldSize)
	{
		uint8_t *ptr = (uint8_t *)data + oldSize; // points to added records;
		DeviceTable_t *entry = (DeviceTable_t *)(void *)ptr;
		S_Nv_ReturnValue_t ret;

		// go through all new records and mark them as free
		while (oldSize < size)
		{
			memset(entry->ieeeaddr, 0xFF, LONG_ADDR_LEN);
			entry->currentTimeOut = 0;
			entry++;
			oldSize += sizeof(DeviceTable_t);
		}
		ret = S_Nv_Write(PDS_DEVICE_TABLE_NONSLEEP_ID, 0U, size, data);
		assert(ret == S_Nv_ReturnValue_Ok);
	}
}

/******************************************************************************
\brief Updates sleep devices table

\param[in] data    - pointer to data with commissioning device table;
\param[in] size    - current item size;
\param[in] oldSize - last size of item
******************************************************************************/
static void updateSleepDevTable(void *data, uint16_t size, uint16_t oldSize)
{
	if (size == oldSize)
	return;

	updateTable(data, size, oldSize);

	if (size > oldSize)
	{
		uint8_t *ptr = (uint8_t *)data + oldSize; // points to added records;
		SleepDeviceTable_t *entry = (SleepDeviceTable_t *)(void *)ptr;
		S_Nv_ReturnValue_t ret;

		// go through all new records and mark them as free
		while (oldSize < size)
		{
			memset(entry->ieeeaddr, 0xFF, LONG_ADDR_LEN);
			entry->currentTimeOut = 0;
			entry->actualTimeOut = 0;
			entry++;
			oldSize += sizeof(SleepDeviceTable_t);
		}
		ret = S_Nv_Write(PDS_DEVICE_TABLE_SLEEP_ID, 0U, size, data);
		assert(ret == S_Nv_ReturnValue_Ok);
	}
}

/******************************************************************************
\brief Updates coord route table

\param[in] data    - pointer to data with commissioning device table;
\param[in] size    - current item size;
\param[in] oldSize - last size of item
******************************************************************************/
static void updateCoordRouteTable(void *data, uint16_t size, uint16_t oldSize)
{
	if (size == oldSize)
	return;

	updateTable(data, size, oldSize);

	if (size > oldSize)
	{
		uint8_t *ptr = (uint8_t *)data + oldSize; // points to added records;
		CoordRouteTable_t *entry = (CoordRouteTable_t *)ptr;
		S_Nv_ReturnValue_t ret;

		// go through all new records and mark them as free
		while (oldSize < size)
		{
			entry->nextHopAddr = 0xFF;
			entry->lqi = 0;
			entry->score = 0;
			entry++;
			oldSize += sizeof(CoordRouteTable_t);
		}
		ret = S_Nv_Write(PDS_ROUTE_TABLE_COORD_ID, 0U, size, data);
		assert(ret == S_Nv_ReturnValue_Ok);
	}
}

/******************************************************************************
\brief Updates coord hop table

\param[in] data    - pointer to data with commissioning device table;
\param[in] size    - current item size;
\param[in] oldSize - last size of item
******************************************************************************/
static void updateCoordHopTable(void *data, uint16_t size, uint16_t oldSize)
{
	if (size == oldSize)
	return;

	updateTable(data, size, oldSize);

	if (size > oldSize)
	{
		uint8_t *ptr = (uint8_t *)data + oldSize; // points to added records;
		CoordHopCount_t *entry = (CoordHopCount_t *)ptr;
		S_Nv_ReturnValue_t ret;

		// go through all new records and mark them as free
		while (oldSize < size)
		{
			entry->lsb = 0;
			entry->msb = 0;
			entry++;
			oldSize += sizeof(CoordHopCount_t);
		}
		ret = S_Nv_Write(PDS_ROUTE_TABLE_HOP_ID, 0U, size, data);
		assert(ret == S_Nv_ReturnValue_Ok);
	}
}
#endif
#endif
/******************************************************************************
\brief Updates BC parameters after restoring taking into account possible size
  changes

\param[in] id      - item id;
\param[in] data    - pointer to data;
\param[in] size    - current item size;
\param[in] oldSize - last size of item

returns true if memory was updated successfully, false - otherwise
******************************************************************************/
bool pdsUpdateMemory(S_Nv_ItemId_t id, void *data, uint16_t size, uint16_t oldSize)
{
  verifyItemSizes(id, size, oldSize);
  
  switch (id)
  {
#if defined(PROTOCOL_MESH)
    case PDS_GENERAL_INFO_ID:
      updateGeneralInfoSet (data);    
      break;
#if defined(PAN_COORDINATOR)	  
	case PDS_COMM_DEVICE_TABLE_ID:
	  updatecommnDevTable (data, size, oldSize);
	  break;
	  
	case PDS_COORDINATOR_TABLE_ID:
	  updateCoordTable (data, size, oldSize);
	  break;
#endif
#if defined(PAN_COORDINATOR) || defined (COORDINATOR)
	case PDS_DEVICE_TABLE_NONSLEEP_ID:
	  updateNonSleepDevTable (data, size, oldSize);
	  break;
	  
	case PDS_DEVICE_TABLE_SLEEP_ID:
	updateSleepDevTable (data, size, oldSize);
	break;
	
	case PDS_ROUTE_TABLE_COORD_ID:
	updateCoordRouteTable (data, size, oldSize);
	break;
	
	case PDS_ROUTE_TABLE_HOP_ID:
	updateCoordHopTable (data, size, oldSize);
	break;
#endif
#endif
	  
    default:
      break;
  }
    

  return true;
}

/******************************************************************************
\brief Verifies whether size change is allowed for given item

\param[in] id      - item id;
\param[in] size    - current item size;
\param[in] oldSize - last size of item
******************************************************************************/
static void verifyItemSizes(S_Nv_ItemId_t id, uint16_t size, uint16_t oldSize)
{
  ItemIdToMemoryMapping_t mapItem;

  if (size == oldSize)
    return;

  if (pdsGetItemDescr(id, &mapItem))
  {
    assert (mapItem.flags & SIZE_MODIFICATION_ALLOWED);
  }
}


/******************************************************************************
\brief To get the item descriptor for the given item ID

\param[in] itemId            - item id;
\param[in] itemDescrToGet    - pointer to item descriptor to be loaded;

returns true if descriptor is found out for the given item ID, false - otherwise
******************************************************************************/
bool pdsGetItemDescr(S_Nv_ItemId_t itemId, ItemIdToMemoryMapping_t *itemDescrToGet )
{
  ItemIdToMemoryMapping_t *itemDescrPtr = (ItemIdToMemoryMapping_t *)PDS_FF_START;

  /* Calculate raw data size */
  for (uint8_t fileIdx = 0; fileIdx < PDS_ITEM_AMOUNT; fileIdx++)
  {
    ItemIdToMemoryMapping_t itemDescr;

    memcpy(&itemDescr, (void const *)itemDescrPtr, sizeof(ItemIdToMemoryMapping_t));

    if(itemDescr.itemId == itemId)
    {
      memcpy(itemDescrToGet, &itemDescr, sizeof(ItemIdToMemoryMapping_t));
      return true;
    }
    itemDescrPtr++;

  }
  return false;
}
/******************************************************************************
\brief To get the directory descriptor for the given dir ID

\param[in] itemId           - item id;
\param[in] dirDescrToGet    - pointer to dir descriptor to be loaded;

returns true if descriptor is found out for the given dir ID, false - otherwise
******************************************************************************/
bool pdsGetDirDescr(S_Nv_ItemId_t itemId, PDS_DirDescr_t *dirDescrToGet )
{
  PDS_DirDescr_t *dirDescrPtr = (PDS_DirDescr_t *)PDS_FD_START;

  /* Calculate raw data size */
  for (uint8_t fileIdx = 0; fileIdx < PDS_DIRECTORIES_AMOUNT; fileIdx++)
  {
    PDS_DirDescr_t dirDescr;

    memcpy(&dirDescr, (void const *)dirDescrPtr, sizeof(PDS_DirDescr_t));
    if(dirDescr.memoryId == itemId)
    {
      memcpy(dirDescrToGet, &dirDescr, sizeof(PDS_DirDescr_t));
      return true;
    }
    dirDescrPtr++;
  }
  return false;
}
#endif 

// eof wlPdsTypesConverter.c