/**
* \file  wlPdsDataServer.c
*
* \brief PDS data server implementation.
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
#include <wlPdsMemIds.h>
#include <wlPdsBindings.h>
#include <S_Nv_Init.h>
#include <S_Nv.h>
#include <wlPdsTaskManager.h>
#include <wlPdsTypesConverter.h>
#include "assert.h"
#include <D_Nv_Init.h>
#include <wlPdsTypes.h>
#include <string.h>
#include "pdsDataServer.h"

/******************************************************************************
                              Defines section
******************************************************************************/
#define EVENT_TO_MEM_ID_MAPPING(event, id)  {.eventId = event, .itemId = id}
#define COMPID "wlPdsDataServer"

/******************************************************************************
                            Types section
******************************************************************************/
typedef struct _EventToMemoryIdMapping_t
{
  uint8_t        eventId;
  S_Nv_ItemId_t itemId;
} EventToMemoryIdMapping_t;

typedef uint8_t PDS_MemMask_t[PDS_ITEM_MASK_SIZE];

/******************************************************************************
                    Prototypes section
******************************************************************************/

static void pdsStoreItem(S_Nv_ItemId_t id);
static bool pdsRestoreItem(S_Nv_ItemId_t id);
static bool pdsInitItemMask(S_Nv_ItemId_t memoryId, uint8_t *itemMask);

/******************************************************************************
                    Static variables section
******************************************************************************/

static uint8_t itemsToStore[PDS_ITEM_MASK_SIZE];

/******************************************************************************
                   Prototype section
******************************************************************************/
void pdsStoreItemTaskHandler(void);


/******************************************************************************
                   Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Restores data from non-volatile storage

PDS files not included in the current build configuration will be ignored.
Restoring process will be performed only if all files, expected for actual
configuration, are presented in NV storage

\param[in] memoryId - an identifier of PDS file or directory to be restored
                      from non-volatile memory

\return true, if all expected files have been restored, false - otherwise
******************************************************************************/
bool PDS_Restore(PDS_MemId_t memoryId)
{
  uint8_t itemsToRestore[PDS_ITEM_MASK_SIZE] = {0U};
  uint8_t i, j;

  if (!pdsInitItemMask(memoryId, itemsToRestore))
    return false;

  for (i = 0U; i < PDS_ITEM_MASK_SIZE; i++)
    for (j = 0U; j < 8U; j++)
      if (itemsToRestore[i] & (1U << j))
        if (!pdsRestoreItem(((S_Nv_ItemId_t)i << 3U) + j))
          return false;

  return true;
}

/**************************************************************************//**
\brief Deletes data from non-volatile storage

\param[in] memoryId - an identifier of PDS item or directory to be deleted
                      from non-volatile memory

\return PDS_DataServerState_t - status of PDS delete
******************************************************************************/
PDS_DataServerState_t PDS_Delete(PDS_MemId_t memoryId)
{
  uint8_t itemsToDelete[PDS_ITEM_MASK_SIZE] = {0U};
  uint8_t i, j;

  if (!pdsInitItemMask(memoryId, itemsToDelete))
    return false;

  for (i = 0U; i < PDS_ITEM_MASK_SIZE; i++)
    for (j = 0U; j < 8U; j++)
      if (itemsToDelete[i] & (1U << j))
        S_Nv_Delete(((S_Nv_ItemId_t)i << 3U) + j);

  return PDS_SUCCESS;
}
/**************************************************************************//**
\brief Stores data in non-volatile memory in background, not blocking other
       processes.

\ingroup pds

All PDS files which are absent in the current build configuration will be ignored.

\param[in] memoryId - an identifier of PDS file or directory to be stored
                      in non-volatile memory.

\return True, if storing process has begun, false - otherwise.
******************************************************************************/
bool PDS_Store(PDS_MemId_t memoryId)
{
  if (!pdsInitItemMask(memoryId, itemsToStore))
    return false;
  
  pdsPostTask(PDS_STORE_ITEM_TASK_ID);

  return true;
}

/**************************************************************************//**
\brief Deletes data from non-volatile storage except the Persistant items
       depending on the parameter passed.

\param[in] includingPersistentItems - deletes persistant items if TRUE
                                      deletes all other items except persistant 
                                      items if false

\return bool - Whether the operation went successfully
******************************************************************************/
bool PDS_DeleteAll(bool includingPersistentItems)
{
  if (S_Nv_ReturnValue_Ok == S_Nv_EraseAll(includingPersistentItems))
    return true;
  else
    return false;

}

/******************************************************************************
\brief PDS store item hadler
******************************************************************************/
void pdsStoreItemTaskHandler(void)
{
  uint8_t i, j;
  static uint8_t i_found, j_found;
  bool breakLoop = false;
  uint8_t byte_index, bit_index;

  
  // run through all items and store the first item
  for (i = 0U, byte_index = i_found + 1; i < PDS_ITEM_MASK_SIZE; i++, byte_index++)
  {
    byte_index %= PDS_ITEM_MASK_SIZE;

    for (j = 0U, bit_index = j_found + 1; j < 8U; j++, bit_index++) {
      bit_index %= 8;

      if (itemsToStore[byte_index] & (1U << bit_index))
      {
        itemsToStore[byte_index] &= ~(1U << bit_index);
        breakLoop = true;
        i_found = byte_index;
        j_found = bit_index;
        break;
      }
    }
    if (breakLoop)
      break;
  }

#ifdef PDS_SECURITY_CONTROL_ENABLE
  if (!pdsIsItemUnderSecurityControl(((S_Nv_ItemId_t)byte_index << 3U) + bit_index) 
        && !S_Nv_IsItemAvailable(((S_Nv_ItemId_t)byte_index << 3U) + bit_index))
#else
  if (!S_Nv_IsItemAvailable(((S_Nv_ItemId_t)byte_index << 3U) + bit_index))
#endif
  {
    ItemIdToMemoryMapping_t itemDescr;

    if(pdsGetItemDescr(((S_Nv_ItemId_t)byte_index << 3U) + bit_index, &itemDescr))
    {
      // Not finding item so initialize it
      S_Nv_ReturnValue_t ret;
      
      if (itemDescr.filler)
        itemDescr.filler();
      ret = S_Nv_ItemInit(((S_Nv_ItemId_t)byte_index << 3U) + bit_index, itemDescr.itemSize, itemDescr.itemData);
      assert((S_Nv_ReturnValue_DidNotExist == ret) || (S_Nv_ReturnValue_Ok == ret));
    }
  }
  else
  {
    // store found item
    pdsStoreItem((((S_Nv_ItemId_t)byte_index) << 3U) + bit_index);
  }

  // check whether there is any item to store
  for (i = 0U; i < PDS_ITEM_MASK_SIZE; i++)
    if (itemsToStore[i])
    {
      pdsPostTask(PDS_STORE_ITEM_TASK_ID);
      break;
    }
}



/**************************************************************************//**
\brief Checks if the specified PDS file or directory can be restored
       from non-volatile memory
       PDS files not included in the current build configuration will be ignored

\param[in] memoryId - an identifier of PDS file or directory to be checked

\return true, if the specified memory can be restored; false - otherwise
******************************************************************************/
bool PDS_IsAbleToRestore(PDS_MemId_t memoryId)
{
  uint8_t itemsToBeAbleRestore[PDS_ITEM_MASK_SIZE] = {0U};
  uint8_t i, j;

  if (!pdsInitItemMask(memoryId, itemsToBeAbleRestore))
    return false;

  for (i = 0U; i < PDS_ITEM_MASK_SIZE; i++)
    for (j = 0U; j < 8U; j++)
      if (itemsToBeAbleRestore[i] & (1U << j))
#ifdef PDS_SECURITY_CONTROL_ENABLE
      {
        if (pdsIsItemUnderSecurityControl(((S_Nv_ItemId_t)i << 3U) + j))
        {
          if (!pdsIsSecureItemAvailable(((S_Nv_ItemId_t)i << 3U) + j))
             return false;
        }
        else
        {
          if (!S_Nv_IsItemAvailable(((S_Nv_ItemId_t)i << 3U) + j))
             return false;
        }
      }
#else
      if (!S_Nv_IsItemAvailable(((S_Nv_ItemId_t)i << 3U) + j))
          return false;
#endif

  return true;
}


/******************************************************************************
\brief Stores item

\param[in] id - item id to store
******************************************************************************/
static void pdsStoreItem(S_Nv_ItemId_t id)
{
  ItemIdToMemoryMapping_t itemDescr;

  if (pdsGetItemDescr(id, &itemDescr))
  {
    assert((0U != itemDescr.itemSize) && (NULL != itemDescr.itemData));
    
    if (itemDescr.filler)
      itemDescr.filler();

#ifdef PDS_SECURITY_CONTROL_ENABLE
    if (pdsIsItemUnderSecurityControl(id))
      pdsStoreSecuredItem(id, itemDescr.itemSize, itemDescr.itemData);
    else
#endif
    {
      S_Nv_ReturnValue_t ret;

      ret = S_Nv_Write(id, 0U, itemDescr.itemSize, itemDescr.itemData);
      assert(ret == S_Nv_ReturnValue_Ok);
      //pdsDataStoreIndicator(id);
    }
  }
}

/******************************************************************************
\brief Restores item

\param[in] id - item id to store

\return true item was restored, false otherwise
******************************************************************************/
static bool pdsRestoreItem(S_Nv_ItemId_t id)
{
  ItemIdToMemoryMapping_t itemDescr;
  uint16_t oldSize;
  
  if (pdsGetItemDescr(id, &itemDescr))
  {
    assert((0U != itemDescr.itemSize) && (NULL != itemDescr.itemData));

    if (itemDescr.filler)
      itemDescr.filler();

#ifdef PDS_SECURITY_CONTROL_ENABLE
    if (pdsIsItemUnderSecurityControl(id))
    {
      /* No update of the item is required since it is taken care in the migration */
      return pdsRestoreSecuredItem(id, itemDescr.itemSize, itemDescr.itemData, NULL);
    }
    else
#endif
    {
      oldSize = S_Nv_ItemLength(id);
      S_Nv_ReturnValue_t ret = S_Nv_ItemInit(id, itemDescr.itemSize, itemDescr.itemData);

      assert((S_Nv_ReturnValue_DidNotExist == ret) || (S_Nv_ReturnValue_Ok == ret));

      if (S_Nv_ReturnValue_Ok == ret)
        if (!pdsUpdateMemory(id, itemDescr.itemData, itemDescr.itemSize, oldSize))
          return false;
      return (S_Nv_ReturnValue_Ok == ret);
    }
  }
  return false;
}

/******************************************************************************
\brief To init the itemmask for the given memory ID

\param[in] memoryId - memeory id to initialize itemMask
\param[in] itemMask - itemMask to be initialized

\return true itemMask is initialized, false otherwise
******************************************************************************/
static bool pdsInitItemMask(S_Nv_ItemId_t memoryId, uint8_t *itemMask)
{
  if (PDS_ALL_EXISTENT_MEMORY == memoryId)
  {
    memset(itemMask, 0xFFU, PDS_ITEM_MASK_SIZE);
    itemMask[0] &= 0xFEU;
  }
  else if (PDS_DIRECTORY_MASK & memoryId)
  {
    PDS_DirDescr_t dirDescr;
    PDS_MemId_t memId;

    if (pdsGetDirDescr(memoryId, &dirDescr))
    {
      for (uint8_t index=0U; index < dirDescr.filesCount; index++)
      {
        memcpy(&memId, (void const FLASH_PTR*)(dirDescr.list + index), sizeof(uint16_t));
        itemMask[memId / 8U] |= 1U << (memId % 8U);
      }
    }
    else
      return false;
  }
  else
  {
    itemMask[memoryId / 8] |= 1 << (memoryId % 8);
  }

  return true;
}

#endif 

// eof wlpdsDataServer.c