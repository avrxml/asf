/**
* \file  wlPdsTypesConverter.h
*
* \brief PDS types convertions definition
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
#ifndef _WLPDS_TYPESCONVERTER
#define _WLPDS_TYPESCONVERTER

/******************************************************************************
                   Includes section
******************************************************************************/

#include <S_Nv.h>
#include <pdsMemIds.h>
#include <compiler.h>

/******************************************************************************
                   Types section
******************************************************************************/
      
typedef struct
{
  uint64_t         extendedAddress;
  uint32_t         channelMap;
  uint16_t         panId;
  uint16_t         networkAddress;
  uint8_t          logicalChannel;
  uint8_t          capabilityInfo;
  uint8_t          nwkSecurityLevel;
} MeshGeneralInfotMem_t;

typedef struct
{
  S_Nv_ItemId_t itemId;
  uint16_t       itemSize;
  void           *itemData;
  void           (*filler)(void);
  uint8_t        flags;
} ItemIdToMemoryMapping_t;

typedef struct
{
  PDS_MemIdRec_t list;
  uint16_t    filesCount;
  PDS_MemId_t memoryId;
} PDS_DirDescr_t;

/* Directory descriptor record pointer */
typedef const PDS_DirDescr_t *PDS_DirDescrRec_t;

/******************************************************************************
                   Definitions section
******************************************************************************/
#ifndef FILE_TO_ITEM_MAPPING
  #define FILE_TO_ITEM_MAPPING
#endif

#define NO_ITEM_FLAGS               0x00U
#define SIZE_MODIFICATION_ALLOWED   0x01U
#define ITEM_UNDER_SECURITY_CONTROL 0x02U

#if defined(PROTOCOL_MESH)
#define GENERAL_INFO_ITEM_SIZE                   (sizeof(MeshGeneralInfotMem_t))
#define PDS_SECURITY_KEY_ITEM_SIZE               (SECURITY_KEY_SIZE)
#define PDS_SECURITY_COUNTER_ITEM_SIZE           (sizeof(API_UINT32_UNION))
#define PDS_BLOOM_VALUE_ITEM_SIZE                (BLOOM_FILTER_SIZE)
#define PDS_CHANNEL_UPDATE_ITEM_SIZE             (sizeof(uint8_t))
#define PDS_COMM_DEVICE_TABLE_ITEM_SIZE          (sizeof(CommDeviceTable_t))
#define PDS_COORDINATOR_TABLE_ITEM_SIZE          (sizeof(CoordinatorTable_t))
#define PDS_DEVICE_TABLE_NONSLEEP_ITEM_SIZE      (sizeof(DeviceTable_t))
#define PDS_DEVICE_TABLE_SLEEP_ITEM_SIZE         (sizeof(SleepDeviceTable_t))
#define PDS_ROUTE_TABLE_COORD_ITEM_SIZE          (sizeof(CoordRouteTable_t))
#define PDS_ROUTE_TABLE_HOP_ITEM_SIZE            (sizeof(CoordHopCount_t))
#else
#define PDS_OUTGOING_FRAME_COUNTER_ITEM_SIZE     (sizeof(API_UINT32_UNION))
#define PDS_PANID_ITEM_SIZE                      (sizeof(API_UINT16_UNION))
#define PDS_CURRENT_CHANNEL_ITEM_SIZE            (sizeof(uint8_t))
#define PDS_CONNECTION_MODE_ITEM_SIZE            (sizeof(uint8_t))
#define PDS_CONNECTION_TABLE_ITEM_SIZE           (sizeof(CONNECTION_ENTRY))
#define PDS_EDC_ITEM_SIZE                        (sizeof(uint8_t))
#if defined (PROTOCOL_STAR)
#define PDS_ROLE_ITEM_SIZE                       (sizeof(uint8_t))
#define PDS_MYINDEX_ITEM_SIZE                    (sizeof(uint8_t))
#endif
#endif

#define ITEM_ID_TO_MEM_MAPPING(item, size, pointer, func, flag) \
  {.itemId = item, .itemSize = size, .itemData = pointer, .filler = func, .flags = flag}

/* PDS item declaration */
#define PDS_DECLARE_ITEM(item, size, pointer, func, flag) \
  PDS_FF_OBJECT(ItemIdToMemoryMapping_t pds_ff_##item) = \
    ITEM_ID_TO_MEM_MAPPING(item, size, pointer, func, flag);


#ifdef FILE_TO_ITEM_MAPPING
#define PDS_DECLARE_FILE(id, dataSize, ramAddr, fileMarks) \
        PDS_DECLARE_ITEM(id, dataSize, ramAddr, NULL, 0x00)
#else
#define PDS_DECLARE_FILE(id, dataSize, ramAddr, fileMarks)
#endif

#ifdef __IAR_SYSTEMS_ICC__
  /* Declaration to place object to PDS_FF (files) segment in MCU Flash */
  #define PDS_FF_OBJECT(object) \
    _Pragma("location=\"PDS_FF\"") __root PROGMEM_DECLARE (object)
  /* Declaration to place object to PDS_FD (directories) segment in MCU Flash */
  #define PDS_DECLARE_DIR(object) \
    _Pragma("location=\"PDS_FD\"") __root PROGMEM_DECLARE (object)
#elif __GNUC__
  #define PDS_FF_OBJECT(object) \
    __attribute__((__section__(".pds_ff"))) const object
  #define PDS_DECLARE_DIR(object) \
    __attribute__((__section__(".pds_fd"))) const object
#else
  #error "Unsupported compiler"
#endif

#ifdef  __IAR_SYSTEMS_ICC__
#pragma segment="PDS_FF"
  #define PDS_FF_START       (ItemIdToMemoryMapping_t *) __sfb("PDS_FF")
  #define PDS_FF_END         (ItemIdToMemoryMapping_t *) __sfe("PDS_FF")
  #define PDS_FF_SIZE         __sfs("PDS_FF")
#pragma segment="PDS_FD"
  #define PDS_FD_START       (PDS_DirDescr_t *) __sfb("PDS_FD")
  #define PDS_FD_END         (PDS_DirDescr_t *) __sfe("PDS_FD")
  #define PDS_FD_SIZE        __sfs("PDS_FD")
#elif __GNUC__
  #define PDS_FF_START       (ItemIdToMemoryMapping_t *) &__pds_ff_start
  #define PDS_FF_END         (ItemIdToMemoryMapping_t *) &__pds_ff_end
  #define PDS_FF_SIZE        (&__pds_ff_end - &__pds_ff_start)
  #define PDS_FD_START       (PDS_DirDescrRec_t) &__pds_fd_start
  #define PDS_FD_END         (PDS_DirDescrRec_t) &__pds_fd_end
  #define PDS_FD_SIZE        (&__pds_fd_end - &__pds_fd_start)
#else
  #error "Unsupported compiler"
#endif

/******************************************************************************
                   Extern section
******************************************************************************/
extern MeshGeneralInfotMem_t genInfoMem;

#ifdef __GNUC__
extern uint32_t __pds_ff_start;
extern uint32_t __pds_ff_end;
extern uint32_t __pds_fd_start;
extern uint32_t __pds_fd_end;
#endif /* __GNUC__ */

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
\brief Checks whether item is under security control

\param[in] id - item id

\returns true if item is under security control, false - otherwise
******************************************************************************/
bool pdsIsItemUnderSecurityControl(S_Nv_ItemId_t id);

/******************************************************************************
\brief Updates BC parameters after restoring taking into account possible size
  changes

\param[in] id      - item id;
\param[in] data    - pointer to data;
\param[in] size    - current item size;
\param[in] oldSize - last size of item

returns true if memory was updated successfully, false - otherwise
******************************************************************************/
bool pdsUpdateMemory(S_Nv_ItemId_t id, void *data, uint16_t size, uint16_t oldSize);

/******************************************************************************
\brief To get the directory descriptor for the given dir ID

\param[in] itemId           - item id;
\param[in] dirDescrToGet    - pointer to dir descriptor to be loaded;

returns true if descriptor is found out for the given dir ID, false - otherwise
******************************************************************************/
bool pdsGetDirDescr(S_Nv_ItemId_t itemId, PDS_DirDescr_t *dirDescrToGet );

/******************************************************************************
\brief To get the item descriptor for the given item ID

\param[in] itemId            - item id;
\param[in] itemDescrToGet    - pointer to item descriptor to be loaded;

returns true if descriptor is found out for the given item ID, false - otherwise
******************************************************************************/
bool pdsGetItemDescr(S_Nv_ItemId_t itemId, ItemIdToMemoryMapping_t *itemDescrToGet );

/**************************************************************************//**
\brief Observes which all item id's are stored and raise the event 

\ingroup pds

\param[in] memoryId   - memoryId which is stored in flash
\return - None
******************************************************************************/
void pdsDataStoreIndicator(PDS_MemId_t memoryId);
/******************************************************************************
\brief Fills General Information
******************************************************************************/
void fillGeneralInformation(void);
#endif // _WLPDS_TYPESCONVERTER
/* eof wlPdsTypesConverter.h */
