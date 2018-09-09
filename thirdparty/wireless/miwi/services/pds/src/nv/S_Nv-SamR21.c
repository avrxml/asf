/**
* \file  S_Nv-SamR21.c
*
* \brief NV component implementation.
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

#if  PDS_ENABLE_WEAR_LEVELING 
#ifndef PDS_USE_EXTERNAL_FLASH

/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "S_Nv_Bindings.h"
#include "S_Nv_Init.h"
#include "S_Nv.h"
#include "pdsDataServer.h"

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/

#include "D_Nv.h"
#include "assert.h"
#include <compiler.h>
#include "wlPdsMemIds.h"
#include "string.h"
#include "sysTimer.h"


/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#if defined __IAR_SYSTEMS_ICC__
//#define S_XNV_LOGGING
#endif

#define COMPID "S_Nv"

#if !defined(MAX_ITEM_COUNT)
/** The maximum number of items. */
#define MAX_ITEM_COUNT   PDS_MAX_ID
#endif

/** The maxumum length of an item. */
#define MAX_ITEM_LENGTH   2048u

/** Timer event used to erase a sector. */
#define EVENT_ERASE_SECTOR   0u
#define EVENT_COMPACT_SECTOR 1u
#define EVENT_COMPACT_ITEM   2u

/** Delay before erasing a sector. */
#define ERASE_SECTOR_DELAY_MS 7000u

/** Delay before performing a preemptive compact operation. */
#define COMPACT_SECTOR_DELAY_MS 10000u

/** Delay before performing a compact item operation. */
#define COMPACT_ITEM_DELAY_MS 3000u

/** Perform a compact item operation if the number of partial writes is larger than this. */
#define COMPACT_ITEM_THRESHOLD 100u

/** Perform a compact sector operation with a delay if the sector has less free space than this. */
#define PREEMPTIVE_COMPACT_SECTOR_THRESHOLD  (MAX_ITEM_LENGTH + BLOCK_HEADER_SIZE)

#define SECTOR_SIZE   D_NV_SECTOR_SIZE
#define FIRST_SECTOR  D_NV_FIRST_SECTOR
#define SECTOR_COUNT  D_NV_SECTOR_COUNT

/* address of the page from where items are stores i.e 2nd row address in the sector */
#define ITEMS_AREA_START_ADDRESS  NVMCTRL_ROW_SIZE

#define ROW_SIZE  NVMCTRL_ROW_SIZE

/** The size of the sector header. */
#define SECTOR_HEADER_SIZE ((uint16_t) sizeof(SectorHeader_t))
/** The size of the block header. same for Snv rev1 */
#define BLOCK_HEADER_SIZE ((uint16_t) sizeof(BlockHeader_t))

/** The sequence number to use for the initial sector. */
#define INITIAL_SECTOR_SEQUENCE_NUMBER 0xFFFFFFFEuL

/* Application NV items list */
#ifndef PERSISTENT_NV_ITEMS_APPLICATION
#define PERSISTENT_NV_ITEMS_APPLICATION 0xFFFu
#endif
/***************************************************************************************************
* LOCAL TYPES
***************************************************************************************************/

// make sure that the flash structures are packed
#if defined(__ICC8051__)
// the IAR compiler for 8051 does not supports "#pragma pack", but does not need it either...
#else
// ...assume that all other compilers do support it
#pragma pack(1)
#endif

/** 16 byte sector header used in flash located at the start of the active sector. */
typedef struct SectorHeaderSnv1_t
{
    /** Is this sector active. Written with 0x0000 at the end of the compact operation. */
    uint16_t isActive;
    /** Signature to detect valid sectors. Must have the value "S_Nv2". */
    uint8_t signature[6];
    /** Counter, decreased each time a new sector becomes the active sector. */
    uint32_t sequenceNumber;
    /** Parity bits for the sequenceNumber field = sequenceNumber ^ 0xFFFFFFFFuL. */
    uint32_t sequenceParity;
} SectorHeaderSnv1_t;

typedef struct BlockHeaderSNv1_t
{
    /** Is this block active. Written with 0x0000 at the end of the write operation. */
    uint16_t isActive;
    /** Item identifier for this block. 0x0000 for a meta block. */
    uint16_t id;
    /** Offset of this block within the item. */
    uint16_t blockOffset;
    /** Length of this block. */
    uint16_t blockLength;
    /** Length of the complete item. */
    uint16_t itemLength;
    /** Pointer to the previous block for this item. */
    uint16_t previousBlock;
    /** Number of partial writes to the item since the last complete write of the item. */
    uint16_t writeCount;
    /** CRC of the header, not including the isActive and headerCrc fields. */
    uint16_t headerCrc;
} BlockHeaderSNv1_t;

/** 16 byte sector header used in flash located at the start of the active sector. */
typedef struct SectorHeader_t
{
    /** Is Crc of the sector header. Written at the end of the compact operation. */
    uint16_t headerCrc;
    /** Signature to detect valid sectors. Must have the value "S_Nv2". */
    uint8_t signature[6];
    /** Counter, decreased each time a new sector becomes the active sector. */
    uint32_t sequenceNumber;
    /** Parity bits for the sequenceNumber field = sequenceNumber ^ 0xFFFFFFFFuL. */
    uint32_t sequenceParity;
    /** Next Page address where the next item update should start after compact sector operation */
    uint16_t nextPageAddressAfterCompact;

} SectorHeader_t;

typedef struct BlockHeader_t
{
    /** CRC of the data. */
    uint16_t dataCrc;
    /** Item identifier for this block. 0x0000 for a meta block. */
    uint16_t id;
    /** Offset of this block within the item. */
    uint16_t blockOffset;
    /** Length of this block. */
    uint16_t blockLength;
    /** Length of the complete item. */
    uint16_t itemLength;
    /** Pointer to the previous block for this item. */
    uint16_t previousBlock;
    /** Number of partial writes to the item since the last complete write of the item. */
    uint16_t writeCount;
    /** CRC of the header, not including the headerCrc */
    uint16_t headerCrc;
} BlockHeader_t;

/** Structure used to store where to find an item. */
typedef struct Item_t
{
    /** Item identifier. */
    uint16_t id;
    /** Pointer to the last written block for this item. */
    uint16_t lastBlock;
} Item_t;

/** Enumerations for Snv revisions. */
typedef enum
{
  SNV_REV_1 = 1,
  SNV_REV_2,
} SnvRevisioin_t;

/** Enumerations for Snv Item alignments. */
typedef enum
{
  ITEM_NO_ALIGNMENT,
  ITEM_16BYTE_ALIGNMENT,
  ITEM_64BYTE_ALIGNMENT
} ItemAlignment_t;


// back to the default packing
#if defined(__ICC8051__)
// the IAR compiler for 8051 does not supports "#pragma pack", but does not need it either...
#else
// ...assume that all other compilers do support it (only tested with visual studio)
#pragma pack()
#endif

/***************************************************************************************************
* LOCAL VARIABLES
***************************************************************************************************/

static const uint16_t s_persistentItems[] = {PERSISTENT_NV_ITEMS_APPLICATION};

/** The active sector. */
static uint8_t s_sector;
/** Location of the first unprogrammed byte in the active sector. */
static uint16_t s_sectorHead;

static uint16_t s_nextPageAddressAfterCompact;

/** The number of read, and thus cached, items */
static uint8_t s_itemCount = 0u;
static Item_t s_itemCache[MAX_ITEM_COUNT];
static uint8_t dataBlock[ROW_SIZE];

/** The sector to erase in the EVENT_ERASE_SECTOR handler. */
static uint8_t s_sectorToErase = 0xFFu;

/** The item to perform a compact operation on and the length of this item. */
static uint16_t s_compactItemId = 0x0000u;
static uint16_t s_compactItemLength = 0x0000u;

static uint16_t compactBlockOffset = 0u;
static uint16_t currentCompactLength = 0u;

/** Callback function called before changing flash contents. */
static S_Nv_PowerSupplyCheckingFunction_t s_powerSupplyCheckingFunction = NULL;

static SYS_Timer_t eraseSectorTimer;
static SYS_Timer_t compactSectorTimer;
static SYS_Timer_t compactItemTimer;

/** Check if the early init function is called already. */
static bool s_earlyInitDone = false;

/***************************************************************************************************
* LOCAL FUNCTION DECLARATIONS
***************************************************************************************************/
static void eraseSectorTimerFired(struct SYS_Timer_t *timer);
static void compactSectorTimerFired(struct SYS_Timer_t *timer);
static void compactItemTimerFired(struct SYS_Timer_t *timer);
static bool PowerSupplyTooLow(void);
static bool CompactSector(void);
static S_Nv_ReturnValue_t CompactItem(void);
static bool GatherData(uint8_t sourceSector, uint16_t lastBlockPointer, uint16_t offset, uint16_t length, void* pData);
void S_Nv_CompactSector_Impl(void);
/***************************************************************************************************
* LOCAL FUNCTIONS
***************************************************************************************************/
/** Erase sector timer callback.
*/
static void eraseSectorTimerFired(struct SYS_Timer_t *timer)
{
    if (!PowerSupplyTooLow())
        D_Nv_EraseSector(s_sectorToErase);
}

/** Compact sector timer callback.
*/
static void compactSectorTimerFired(struct SYS_Timer_t *timer)
{
  bool status;

    if (!PowerSupplyTooLow()) {
      status = CompactSector();
      assert(false);
    }  

  // Keep compiler happy
  status = status;

}

/** Compact item timer callback.
*/
static void compactItemTimerFired(struct SYS_Timer_t *timer)
{
    (void)CompactItem();
}

/** Check the power supply.
    \returns TRUE if the power supply is too low, FALSE when the the power
             supply is OK or when there is no callback installed.
*/
static bool PowerSupplyTooLow()
{
    bool result = false;
    if ( s_powerSupplyCheckingFunction != NULL )
    {
        if ( !s_powerSupplyCheckingFunction() )
        {
            result = true;
        }
    }
    return result;
}

static bool IsPersistent(uint16_t id)
{
    for ( uint8_t i = 0u; i < sizeof(s_persistentItems); i++ )
    {
        if ( id == s_persistentItems[i] )
        {
            return true;
        }
    }
    return false;
}

/** Check if the RAM buffer contains all 0xFF values (flash erased)
    \param pData The buffer to check
    \param length The length of the buffer
    \returns TRUE if the buffer contains all 0xFF, FALSE otherwise
*/
static bool IsEmpty(uint8_t* pData, uint16_t length)
{
    for ( uint16_t i = 0u; i < length; i++ )
    {
        if ( pData[i] != 0xFFu )
        {
            return false;
        }
    }

    return true;
}

/** Update the sector head with increment as per Alignement
    \param increment The length updated
*/

static void UpdateSectorHead(uint16_t increment, ItemAlignment_t itemAlignment )
{
    // Adjust the sector head according to the increment and alignment given
    switch(itemAlignment)
    {
        case ITEM_NO_ALIGNMENT:
           s_sectorHead = (s_sectorHead + increment);
           break;
        case ITEM_16BYTE_ALIGNMENT:
           s_sectorHead = (s_sectorHead + increment + 0x000Fu) & 0xFFF0u;
           break;
        case ITEM_64BYTE_ALIGNMENT:
           s_sectorHead = (s_sectorHead + increment + 0x003Fu) & 0xFFC0u;
           break;
        default:
           break;
    }
}

/** Return a pointer to the cache for the item.
    \param id The id to find
    \returns A pointer to the cache for the ID, or NULL if it was not found
*/
static Item_t *FindItemCache(uint16_t id)
{
    for ( uint8_t cacheIndex = 0u; cacheIndex < s_itemCount; cacheIndex++ )
    {
        Item_t *cache = &s_itemCache[cacheIndex];
        if ( cache->id == id )
        {
            assert(cache->lastBlock != 0x0000);
            return cache;
        }
    }
    return NULL;
}

/** Return a pointer to a cache for a new item.
    \param id The id to create cache for
    \returns A pointer to the cache for the ID
*/
static Item_t *CreateItemCache(uint16_t id)
{
    assert(FindItemCache(id) == NULL);
    assert(s_itemCount < MAX_ITEM_COUNT);

    Item_t *cache = &s_itemCache[s_itemCount++];
    cache->id = id;

    return cache;
}

/** Removes the specified item from the cache array.
    \param id The id to delete the cache for
*/
static void DeleteItemCache(uint16_t id)
{
    assert(FindItemCache(id) != NULL);

    // Overwrite specified item cache with the last one
    Item_t *cache = FindItemCache(id);
    *cache = s_itemCache[--s_itemCount];
}

/** Return a pointer to the last written block for the item.
    \param id The id to find
    \returns A pointer to the last written block, or 0x0000u if the item was not found
*/
static uint16_t FindItem(uint16_t id)
{
    Item_t *cache = FindItemCache(id);

    if (cache)
    {
        return cache->lastBlock;
    }

    // item not found
    return 0x0000u;
}

static uint16_t ComputeCrc(uint8_t* pData, uint16_t length, uint16_t crc)
{
    for ( /* empty */ ; length != 0u; length-- )
    {
        uint8_t x;
        uint8_t data;

        if (pData == NULL)
            data = 0xFF;
        else
            data = *pData;

        x = (uint8_t) ((crc >> 8) ^ data);
        x = x ^ (x >> 4);
        crc = (crc << 8) ^ ((uint16_t) x << 12) ^ ((uint16_t) x << 5) ^ (uint16_t) x;

        if ( pData != NULL )
          pData++;
    }

    return crc;
}

static uint16_t ComputeSectorHeaderCrc(SectorHeader_t* pSectorHeader)
{
    return ComputeCrc(((uint8_t*) pSectorHeader) + sizeof(pSectorHeader->headerCrc), (sizeof(SectorHeader_t)- sizeof(pSectorHeader->headerCrc)), 0xFFFFu); // skip headerCrc field
}

static uint16_t ComputeHeaderCrc(BlockHeader_t* pBlockHeader)
{
    return ComputeCrc(((uint8_t*) pBlockHeader), (sizeof(BlockHeader_t) - sizeof(pBlockHeader->headerCrc)), 0xFFFFu); // skip headerCrc field
}

static uint16_t ComputeHeaderCrcSnv1(BlockHeaderSNv1_t* pBlockHeader)
{
    return ComputeCrc(((uint8_t*) pBlockHeader) + sizeof(pBlockHeader->isActive), sizeof(BlockHeaderSNv1_t) - (sizeof(pBlockHeader->isActive)  + sizeof(pBlockHeader->headerCrc)), 0xFFFF); // skip is Active and headerCrc fields
}

static uint16_t ComputeDataCrc(uint8_t sourceSector, uint16_t blockPointer, BlockHeader_t* pBlockHeader)
{
    uint16_t dataLength, offset, length;
    uint16_t crc = 0xFFFFu;

    length = pBlockHeader->blockLength;
    offset = pBlockHeader->blockOffset;
    for(int8_t i= (length/sizeof(dataBlock)); i>=0; i--)
    {
        if (i)
        {
            dataLength = sizeof(dataBlock);
            length = length - sizeof(dataBlock);
        }
        else
        {
            dataLength = length;
        }
        if (dataLength == 0)
            break;
 
        if (GatherData(sourceSector, blockPointer, offset, dataLength, dataBlock))
        {
            offset = offset+dataLength;
            crc = ComputeCrc(((uint8_t*) &dataBlock), dataLength, crc);
        }
    }
    return crc;
}

static bool WriteAndCheck(uint16_t offset, uint8_t* pData, uint16_t length)
{
    D_Nv_Write(s_sector, offset, pData, length);
    return D_Nv_IsEqual(s_sector, offset, pData, length);
}

static bool WriteBlockHeader(BlockHeader_t* pBlockHeader)
{
    pBlockHeader->dataCrc = 0xFFFFu;

    pBlockHeader->headerCrc = ComputeHeaderCrc(pBlockHeader);

    memset(dataBlock , 0xFF, BLOCK_HEADER_SIZE);
    memcpy(dataBlock, pBlockHeader, BLOCK_HEADER_SIZE);

    if ( !WriteAndCheck(s_sectorHead, (uint8_t*) dataBlock, BLOCK_HEADER_SIZE) )
    {
        // failed to write header to flash.
        // check if any bits were programmed
        if ( !D_Nv_IsEmpty(s_sector, s_sectorHead, BLOCK_HEADER_SIZE) )
        {
            // the next write should skip this failed header as some bits have been programmed
            UpdateSectorHead(BLOCK_HEADER_SIZE, ITEM_64BYTE_ALIGNMENT);
            return false;
        }
        else
        {
            // no bits were programmed (everything still is 0xFF).
            // the init function sees this as the start of the empty space
            // in the sector, so the next write should should attempt to
            // write to the same address
            return false;
        }
    }
    else
    {
        UpdateSectorHead(BLOCK_HEADER_SIZE, ITEM_64BYTE_ALIGNMENT); //used in only for delete item case
        return true;
    }
}

static bool WriteDataBlockAndHeader(BlockHeader_t* pBlockHeader, uint8_t* pData)
{
    uint16_t dataBlockOffset = 0;
    uint16_t currentLength = 0;

    memset(dataBlock, 0xFF, sizeof(dataBlock));

    /* start filling the header data Crc and Header Crc */
    pBlockHeader->dataCrc = ComputeCrc(pData, pBlockHeader->blockLength, 0xFFFF);

    pBlockHeader->headerCrc = ComputeHeaderCrc(pBlockHeader);

    memcpy(dataBlock, pBlockHeader, BLOCK_HEADER_SIZE);

    dataBlockOffset = BLOCK_HEADER_SIZE;

    currentLength = pBlockHeader->blockLength + BLOCK_HEADER_SIZE;

    if (pData != NULL )
    {
        if(currentLength <= ROW_SIZE)
        {
            memcpy(dataBlock + BLOCK_HEADER_SIZE , pData, pBlockHeader->blockLength);
        }
        else // (currentLength > ROW_SIZE)
        {
            uint16_t bytesToCommit;
            uint16_t inDataOffset = 0;

            do
            {
                bytesToCommit = (currentLength > ROW_SIZE) ? ROW_SIZE : currentLength;

                memcpy(dataBlock + dataBlockOffset, pData + inDataOffset, bytesToCommit - dataBlockOffset);

                if ( !WriteAndCheck(s_sectorHead, dataBlock, bytesToCommit))
                {
                    return false;
                }
                UpdateSectorHead(bytesToCommit, ITEM_64BYTE_ALIGNMENT);
                currentLength -= bytesToCommit;
                inDataOffset += (bytesToCommit - dataBlockOffset);
                dataBlockOffset = 0;

            } while(currentLength > 0);

            return true;
        }
    }
    if ( !WriteAndCheck(s_sectorHead, dataBlock, MIN((BLOCK_HEADER_SIZE + pBlockHeader->blockLength), ROW_SIZE)) )
    {
        // failed to write data to flash.
        // skip the invalid data for the next write (even if everything still is 0xFF)
        UpdateSectorHead((pBlockHeader->blockLength + BLOCK_HEADER_SIZE), ITEM_64BYTE_ALIGNMENT);
        return false;
    }
    UpdateSectorHead((pBlockHeader->blockLength + BLOCK_HEADER_SIZE), ITEM_64BYTE_ALIGNMENT );

    return true;

}


/** Gather data from an item for a read or compact operation.
    \param sourceSector
    \param lastBlockPointer Pointer to the last block written for the item
    \param offset The start of the range of bytes to copy from the item
    \param length The size of the range of bytes to copy from the item
    \param pData Pointer to destination buffer in RAM for a read operation. NULL for a compact operation.

    This function is used by both the read and the compact operation.
     - For a read operation, the pBuffer parameter points to a buffer in RAM.
     - For a compact operation, pBuffer parameter is NULL and the data will
       be copied to the flash memory \ref s_sectorHead in sector \ref s_sector.

*/

static bool GatherData(uint8_t sourceSector, uint16_t lastBlockPointer, uint16_t offset, uint16_t length, void* pData)
{
    // pointer to end of destination in RAM
    uint8_t* pDestination = NULL;
    BlockHeader_t blockHeader;
    uint16_t blockStart;
    uint16_t blockEnd;
    uint16_t count;

    // prepare the correct pointer
    if ( pData != NULL )
    {
        pDestination = (uint8_t*) pData + length;
    }

    // start with the last written block
    uint16_t currentBlockPointer = lastBlockPointer;

    // [readStart, readEnd> is the range of data that can be read during the current
    // pass over the blocks (the range includes readStart, but not readEnd).
    // it is initialized with the requested range
    uint16_t readStart = offset;
    uint16_t readEnd = offset + length;

    // continue until we have all the data that was requested
    while ( readStart != readEnd )
    {
        // find the block that contains the **last byte** that we want to read.
        for ( ;; )
        {
            if ( currentBlockPointer == 0x0000u )
            {
                // reached first block without finding the data.
                // either the flash is corrupt, or this is a read beyond the item length
                return false;
            }

            // get the header of the current block
            D_Nv_Read(sourceSector, currentBlockPointer, (uint8_t*) &blockHeader, BLOCK_HEADER_SIZE);

            // [blockStart, blockEnd> is the range of bytes in this block
            blockStart = blockHeader.blockOffset;
            blockEnd = blockHeader.blockOffset + blockHeader.blockLength;

            if ( (readEnd <= blockStart) || (readStart >= blockEnd) )
            {
                // this block does not contain any bytes that we want to read.
                // continue with previous written block...
            }
            else if ( readEnd > blockEnd )
            {
                // this block contains some bytes that we want to read, but not the last byte.
                // adjust the read range to prevent reading an older version of these bytes
                readStart = blockEnd;
                // continue with previous written block...
            }
            else
            {
                // this block contains the last byte that we want to read.
                // read all data that we can from this block...
                break;
            }

            // ...continue with previous written block
            currentBlockPointer = blockHeader.previousBlock;
        }

        // ...read all data that we can from this block

        // pointer to the last byte that we want to read
        uint16_t sourcePointer = (currentBlockPointer + BLOCK_HEADER_SIZE) + (readEnd - blockStart);

        // check how many of the bytes that we want are in this block
        if ( readStart < blockStart )
        {
            // the block does not contain all data we want to read this pass
            count = readEnd - blockStart;

            // continue with the current pass after reading this block
            currentBlockPointer = blockHeader.previousBlock;
        }
        else
        {
            // the block contains all data we want to read this pass
            count = readEnd - readStart;

            // start with a new pass after reading this block (unless we have all requested data)
            currentBlockPointer = lastBlockPointer;
            readStart = offset;
        }
        readEnd -= count;

        // read the data that we want from the source sector
        if ( pDestination != NULL)
        {
            // this is for a read operation, so copy to buffer in RAM
            sourcePointer -= count;
            pDestination -= count;
            D_Nv_Read(sourceSector, sourcePointer, (uint8_t*)pDestination, count);
        }
    }
    return true;
}

static bool EraseSector(void)
{
    // Erase the sector
    D_Nv_EraseSector(s_sector);
    // check if the erase succeeded
    if ( !D_Nv_IsEmpty(s_sector, 0u, SECTOR_SIZE) )
    {
        return false;
    }
    s_sectorHead =  ITEMS_AREA_START_ADDRESS;

    return true;
}

static bool WriteSectorHeader(uint32_t sequenceNumber)
{
    // write sector header
    SectorHeader_t sectorHeader;
    sectorHeader.signature[0] = (uint8_t) 'A';
    sectorHeader.signature[1] = (uint8_t) 'T';
    sectorHeader.signature[2] = (uint8_t) 'S';
    sectorHeader.signature[3] = (uint8_t) 'N';
    sectorHeader.signature[4] = (uint8_t) 'v';
    sectorHeader.signature[5] = (uint8_t) '2';
    sectorHeader.sequenceNumber = sequenceNumber;
    sectorHeader.sequenceParity = sequenceNumber ^ 0xFFFFFFFFuL;
    sectorHeader.nextPageAddressAfterCompact = s_nextPageAddressAfterCompact;
    sectorHeader.headerCrc = ComputeSectorHeaderCrc(&sectorHeader);
    if ( !WriteAndCheck(0u, (uint8_t*) &sectorHeader, SECTOR_HEADER_SIZE) )
    {
        return false;
    }
    return true;
}


static void LoadSector(SnvRevisioin_t revisionNumber)
{
    if (SNV_REV_1 == revisionNumber)
    {
        s_sectorHead = sizeof(SectorHeaderSnv1_t);
        // Done when sectorhead reaches end of sector
        while ( s_sectorHead < SECTOR_SIZE )
        {
            BlockHeaderSNv1_t blockHeader;
            D_Nv_Read(s_sector, s_sectorHead, (uint8_t*) &blockHeader, BLOCK_HEADER_SIZE);
            if ( IsEmpty((uint8_t*) &blockHeader, BLOCK_HEADER_SIZE) )
            {
                // no header. done
                break;
            }
            else if ( blockHeader.headerCrc != ComputeHeaderCrcSnv1(&blockHeader) )
            {
                // invalid header. skip header
                UpdateSectorHead(BLOCK_HEADER_SIZE, ITEM_16BYTE_ALIGNMENT);
            }
            else if ( blockHeader.isActive != 0x0000u )
            {
                // inactive header. skip header and data
                UpdateSectorHead((BLOCK_HEADER_SIZE + blockHeader.blockLength), ITEM_16BYTE_ALIGNMENT);
            }
            else
            {
                // Try to find cache for current ID -- if none found, it's a new item!
                uint16_t id = blockHeader.id;
                Item_t *cache = FindItemCache(id);

                if (cache == NULL)
                {
                    cache = CreateItemCache(id);
                }

                cache->lastBlock = s_sectorHead;

                // If item length is zero, the item had been deleted -- remove the cache
                if ( blockHeader.itemLength == 0u )
                {
                    DeleteItemCache(id);
                }
                UpdateSectorHead((BLOCK_HEADER_SIZE + blockHeader.blockLength), ITEM_16BYTE_ALIGNMENT);
                
            }
        }
        //To avoid handling multiple revisions of items do a force compact operation once.
        if ( !CompactSector() )
        {
            assert(false);
        }
    }
    else if (SNV_REV_2 == revisionNumber)
    {
        SectorHeader_t sectorHeader;
        BlockHeader_t blockHeader;
        ItemAlignment_t itemAlignment = ITEM_64BYTE_ALIGNMENT;

        s_sectorHead = ITEMS_AREA_START_ADDRESS;
        D_Nv_Read(s_sector, 0, (uint8_t*) &sectorHeader, sizeof(SectorHeader_t));

          // Done when sectorhead reaches end of sector
        while ( s_sectorHead < SECTOR_SIZE )
        {
            if ((sectorHeader.nextPageAddressAfterCompact != 0xFFFFu)  && (s_sectorHead < sectorHeader.nextPageAddressAfterCompact))
            {
                itemAlignment = ITEM_NO_ALIGNMENT;
            }
            else
            {
                //no compact operation would have not done before reset or 
                //item updated after compact operation i.e the page aligned item updates
                itemAlignment = ITEM_64BYTE_ALIGNMENT;
                UpdateSectorHead(0, itemAlignment);
            }
            D_Nv_Read(s_sector, s_sectorHead, (uint8_t*) &blockHeader, BLOCK_HEADER_SIZE);

            if ( IsEmpty((uint8_t*) &blockHeader, BLOCK_HEADER_SIZE) )
            {
                // no header. done
                break;
            }
            else if ( blockHeader.headerCrc != ComputeHeaderCrc(&blockHeader) )
            {
                // invalid header. skip header
                UpdateSectorHead(BLOCK_HEADER_SIZE, itemAlignment);
            }
            else
            {
                uint16_t crc = ComputeDataCrc(s_sector, s_sectorHead, &blockHeader);
                if (crc != blockHeader.dataCrc)
                {
                    // inactive header. skip header and data
                    UpdateSectorHead((BLOCK_HEADER_SIZE + blockHeader.blockLength), itemAlignment);
                }
                else
                {
                    // Try to find cache for current ID -- if none found, it's a new item!
                    uint16_t id = blockHeader.id;
                    Item_t *cache = FindItemCache(id);

                    if (cache == NULL)
                    {
                        cache = CreateItemCache(id);
                    }

                    cache->lastBlock = s_sectorHead;

                    // If item length is zero, the item had been deleted -- remove the cache
                    if ( blockHeader.itemLength == 0u )
                    {
                        DeleteItemCache(id);
                    }
                    // Just update the sector head based on the compacted, not aligned or page aligned
                    UpdateSectorHead((BLOCK_HEADER_SIZE + blockHeader.blockLength), itemAlignment);
                }

            }
        }
    }
}

/* Smart compacting is way of dumping data in to other sector as 256 byte chunks
 * without following any alignements as normal item updates.Items are filled into
  256 byte buffer(= row size), committed in shot which takes 4 page writs maximum
 */

static bool SmartCompacting(uint8_t* compactBlock, uint16_t curItemLength)
{
    //manupulate the current compact length
    currentCompactLength += curItemLength;

    // if the complete buffer cannot be filled with the current item
    if ( currentCompactLength < ROW_SIZE)
    {
        memcpy(compactBlock + compactBlockOffset, dataBlock, curItemLength);
        compactBlockOffset += curItemLength;

        return true;

    } //if with the current item, it cannot be fit into one buffer
    else  //(currentCompactLength >= ROW_SIZE) cases
    {
        uint16_t dataBlockOffset = 0;


        memcpy(compactBlock + compactBlockOffset, dataBlock, (ROW_SIZE - compactBlockOffset));
        if ( !WriteAndCheck(s_sectorHead, compactBlock, ROW_SIZE ))
        {
            return false;
        }
        UpdateSectorHead(ROW_SIZE, ITEM_NO_ALIGNMENT);
        dataBlockOffset += (ROW_SIZE - compactBlockOffset);
        compactBlockOffset = 0;
        currentCompactLength -= ROW_SIZE;
        if (currentCompactLength)
        {
            memcpy(compactBlock, dataBlock + dataBlockOffset, currentCompactLength);
            compactBlockOffset = currentCompactLength;
        }
    }

    return true;

}

/* Important: if CompactSector fails, the only fix is to reinitialize!
 * This is because the itemCache, sector head and sector selector will
 * be messed up.
 */

static bool CompactSector(void)
{
#if defined(ENABLE_NV_COMPACT_LOGGING)
    PRINTA(("CompactSector(s=%X)", s_sector));
#endif
    uint8_t compactBlock[ROW_SIZE];

    // stop timer for preemptive compact sector as this will not be needed any more
    SYS_TimerStop(&compactSectorTimer);

    // stop the compact item as a compact sector will compact all items
    s_compactItemId = 0u;
    s_compactItemLength = 0u;

    compactBlockOffset = 0;
    currentCompactLength = 0;

    uint8_t sourceSector = s_sector;

    // get the sector header for the source sector
    SectorHeader_t sectorHeader;
    D_Nv_Read(sourceSector, 0u, (uint8_t*) &sectorHeader, SECTOR_HEADER_SIZE);

    uint32_t nextSequenceNumber = sectorHeader.sequenceNumber - 1uL;

    // find and prepare a destination sector
    do
    {
        s_sector++;
        if ( s_sector >= (FIRST_SECTOR + SECTOR_COUNT) )
        {
            s_sector = FIRST_SECTOR;
        }

        if ( s_sector == sourceSector )
        {
            // all sector failed to initialize
            assert(false);
        }
    }
    while ( !EraseSector());

    // Traverse the item cache, writing all items to the new sector
    for (uint8_t cacheIndex = 0; cacheIndex < s_itemCount; cacheIndex++) {
        Item_t *cache = &s_itemCache[cacheIndex];


        uint16_t lastBlockPointer = cache->lastBlock;
        uint8_t  dataBlockOffset = BLOCK_HEADER_SIZE;
        // Start by reading out header of last block
        //this could be old or new block header, but used new block header
        //as except first field(old isActive and new-dtatCrc),
        //all other fileds are same and first field has not been used 
        //directly form the read header

        BlockHeader_t blockHeader;
        D_Nv_Read(sourceSector, lastBlockPointer, (uint8_t*) &blockHeader, BLOCK_HEADER_SIZE);

        // Construct header for a single block with contiguous data
        blockHeader.blockOffset = 0x0000u;
        blockHeader.blockLength = blockHeader.itemLength;
        blockHeader.previousBlock = 0x0000u;
        blockHeader.writeCount = 0u;

        uint16_t currLength = BLOCK_HEADER_SIZE + blockHeader.itemLength;

        blockHeader.dataCrc =  ComputeDataCrc(sourceSector, lastBlockPointer, &blockHeader);

        blockHeader.headerCrc = ComputeHeaderCrc(&blockHeader);

        memset(dataBlock, 0xFF, sizeof(dataBlock));

        memcpy(dataBlock, &blockHeader, BLOCK_HEADER_SIZE);

        cache->lastBlock = s_sectorHead  + compactBlockOffset;

        if (currLength <= ROW_SIZE)
        {
            if (!GatherData(sourceSector, lastBlockPointer, 0u, blockHeader.itemLength, (dataBlock + BLOCK_HEADER_SIZE )))
            {
                return false;
            }

            cache->lastBlock = s_sectorHead  + compactBlockOffset;
            if (!SmartCompacting(compactBlock, currLength))
            {
                return false;
            }
        }
        else   //greater than 256 case
        {
            uint16_t bytesToGather;
            uint16_t inDataOffset = 0;

            do
            {
              bytesToGather =  (currLength > ROW_SIZE) ? ROW_SIZE : currLength; 
              
              if (!GatherData(sourceSector, lastBlockPointer, inDataOffset, (bytesToGather - dataBlockOffset) , (dataBlock + dataBlockOffset)))
              {
                  return false;
              }

              if (!SmartCompacting(compactBlock, bytesToGather))
              {
                  return false;
              }
              currLength -= bytesToGather;
              inDataOffset += (bytesToGather - dataBlockOffset);
              dataBlockOffset = 0;

            } while(currLength > 0);
        }
    }
    // if some uncommitted data avaialble, committ it
    if ( currentCompactLength)
    {
        if ( !WriteAndCheck(s_sectorHead, compactBlock, currentCompactLength) )
        {
            return false;
        }
    }
    // Just update sector header as per the the length of the data committed, no alignments
    UpdateSectorHead(currentCompactLength, ITEM_NO_ALIGNMENT);

    s_nextPageAddressAfterCompact = s_sectorHead;

    //TODO
    /* Before activating the new sector, make sure the the item got updated 
       new structure if there any */
    //SYS_PostEvent(BC_EVENT_UPGRADE_STRUCT_CHANGED_ITEMS, 0U);

    // update nextPageAddressAfterCompact in the sector header
    // All items moved, so now we just need to Write the Sector Header with 
    // nextPageAddressAfterCompact at the end of compact sector operation
    if ( !WriteSectorHeader(nextSequenceNumber))
    {
        return false;
    }
    // Done with compact sector opration, Set the Sector Head to next page address for normal item update 
    UpdateSectorHead(0, ITEM_64BYTE_ALIGNMENT);

    // schedule an erase of the source sector,Restart the timer if it is already running.
    s_sectorToErase = sourceSector;
    SYS_TimerStop(&eraseSectorTimer);
    SYS_TimerStart(&eraseSectorTimer);

    return true;
}

static void CompactSectorIfNeeded(uint16_t immediateThreshold)
{
    uint16_t freeSpace = SECTOR_SIZE - s_sectorHead;

    if ( freeSpace < immediateThreshold )
    {
        if ( !CompactSector() )
        {
            assert(false);
        }
        return;
    }
    if ( freeSpace < PREEMPTIVE_COMPACT_SECTOR_THRESHOLD )
    {
        if (!SYS_TimerStarted(&compactSectorTimer))
        {
        compactSectorTimer.handler = compactSectorTimerFired;
        compactSectorTimer.timeout = COMPACT_SECTOR_DELAY_MS;
        compactSectorTimer.interval = COMPACT_SECTOR_DELAY_MS;
        compactSectorTimer.mode = SYS_TIMER_INTERVAL_MODE;
        //TODO: check timer to be started or not
        //SYS_StartTimer(&compactSectorTimer);
        }
    }
}

static S_Nv_ReturnValue_t CompactItem(void)
{
#if defined(ENABLE_NV_COMPACT_LOGGING)
    PRINTA(("CompactItem(id=%X)", s_compactItemId));
#endif

    if ( PowerSupplyTooLow() )
    {
        return S_Nv_ReturnValue_PowerSupplyTooLow;
    }

    if ( (s_compactItemLength == 0u) && ( s_compactItemId == 0u ) )
    {
        // compact sector was performed since the compact item was
        // scheduled, so there is no need for another compact item unless
        // a resize of an item is needed.
        return S_Nv_ReturnValue_Ok;
    }

    if (s_compactItemLength != 0)
        CompactSectorIfNeeded(s_compactItemLength + BLOCK_HEADER_SIZE);

    Item_t *cache = FindItemCache(s_compactItemId);
    if ( cache == NULL )
    {
        // trying to compact a non-existing item (item may have been deleted)
        return S_Nv_ReturnValue_Ok;
    }

    uint16_t blockPointer = cache->lastBlock;

    BlockHeader_t blockHeader;
    // read last written item block header
    D_Nv_Read(s_sector, blockPointer, (uint8_t*) &blockHeader, BLOCK_HEADER_SIZE);

    if (s_compactItemLength == 0)
    {
        CompactSectorIfNeeded(blockHeader.itemLength + BLOCK_HEADER_SIZE);

        cache = FindItemCache(s_compactItemId);
        assert(cache != NULL);
        blockPointer = cache->lastBlock;
        // read last written item block header
        D_Nv_Read(s_sector, blockPointer, (uint8_t*) &blockHeader, BLOCK_HEADER_SIZE);
    }

    uint16_t lastBlock = s_sectorHead;

    // write the block header to the destination sector. all data will be merged into one block
    uint16_t bytesToGather = blockHeader.itemLength;
    if ( s_compactItemLength != 0u )
    {
        //TODO
      //PRINTA (("Resizing NV item (id=%X) from %X to %X", s_compactItemId, blockHeader.itemLength, s_compactItemLength));

        // Change the length of the item
        blockHeader.itemLength = s_compactItemLength;

        if ( bytesToGather > blockHeader.itemLength )
        {
            // Item will be truncated as the new length is smaller than the original length.
            bytesToGather = blockHeader.itemLength;
        }
    }

    blockHeader.blockOffset = 0u;
    blockHeader.blockLength = blockHeader.itemLength;
    blockHeader.previousBlock = 0x0000u;
    blockHeader.writeCount = 0u;

    //manipulate data buffer and then commit
    uint16_t currLength = BLOCK_HEADER_SIZE + bytesToGather;

    memcpy(dataBlock, &blockHeader, BLOCK_HEADER_SIZE);

    uint16_t dataBlockOffset = BLOCK_HEADER_SIZE;

    if (currLength <= ROW_SIZE)
    {
        if (!GatherData(s_sector, blockPointer, 0u, bytesToGather, (dataBlock + BLOCK_HEADER_SIZE )))
        {
            // N_LOG_NONFATAL();
            return S_Nv_ReturnValue_Failure;
        }

        memcpy(dataBlock, &blockHeader, BLOCK_HEADER_SIZE);
        if ( !WriteAndCheck(s_sectorHead, dataBlock, blockHeader.blockLength + BLOCK_HEADER_SIZE) )
        {
            return false;
        }
        UpdateSectorHead((BLOCK_HEADER_SIZE + blockHeader.blockLength), ITEM_64BYTE_ALIGNMENT);
    }
    else
    {
        uint16_t bytesToGatherAndCommit;
        uint16_t inDataOffset = 0;

        do
        {
            bytesToGatherAndCommit = (currLength > ROW_SIZE) ? ROW_SIZE : currLength;
            if (!GatherData(s_sector, blockPointer, inDataOffset, bytesToGatherAndCommit - dataBlockOffset , (dataBlock + dataBlockOffset)))
            {
                // N_LOG_NONFATAL();
                return S_Nv_ReturnValue_Failure;
            }

            if ( !WriteAndCheck(s_sectorHead, dataBlock, bytesToGatherAndCommit) )
            {
                return false;
            }
            UpdateSectorHead(bytesToGatherAndCommit, ITEM_64BYTE_ALIGNMENT);
            currLength -= bytesToGatherAndCommit;
            inDataOffset += (bytesToGatherAndCommit - dataBlockOffset);
            dataBlockOffset = 0;
        } while(currLength > 0);
    }

    s_compactItemId = 0u;
    s_compactItemLength = 0u;

    cache->lastBlock = lastBlock;

    return S_Nv_ReturnValue_Ok;
}

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

void S_Nv_EarlyInit(void)
{
    SnvRevisioin_t revisionNumber;
    s_itemCount = 0u;

    SectorHeader_t sectorHeader;

    uint8_t lastSector = 0xFFu;
    uint32_t lastSectorSequence = 0xFFFFFFFFuL;

    for ( uint8_t sector = FIRST_SECTOR; sector < (FIRST_SECTOR + SECTOR_COUNT); sector++ )
    {
        D_Nv_Read(sector, 0u, (uint8_t*) &sectorHeader, SECTOR_HEADER_SIZE);
        if (((sectorHeader.sequenceNumber ^ sectorHeader.sequenceParity) == 0xFFFFFFFFuL) &&
            (sectorHeader.signature[0] == (uint8_t) 'A') &&
            (sectorHeader.signature[1] == (uint8_t) 'T') &&
            (sectorHeader.signature[2] == (uint8_t) 'S') &&
            (sectorHeader.signature[3] == (uint8_t) 'N') &&
            (sectorHeader.signature[4] == (uint8_t) 'v') )
        {
            if ( sectorHeader.signature[5] == (uint8_t) '1')
            {
                SectorHeaderSnv1_t* pSectorHeaderSnv1 = (SectorHeaderSnv1_t*)&sectorHeader;
                if (pSectorHeaderSnv1->isActive == 0x0000u)

                {
                    revisionNumber = SNV_REV_1;
                }
                else
                  continue;
            }
            else if (sectorHeader.signature[5] == (uint8_t) '2')
            {
              uint16_t headerCrc = ComputeSectorHeaderCrc(&sectorHeader);
              if (headerCrc == sectorHeader.headerCrc)
              {
                  revisionNumber = SNV_REV_2;
              }
              else
                continue;
            }
            else
            {
                 continue;
            }
            // active sector
            if ( sectorHeader.sequenceNumber < lastSectorSequence )
            {
                lastSector = sector;
                lastSectorSequence = sectorHeader.sequenceNumber;
            }
        }
    }

    if ( lastSector == 0xFFu )
    {
        // no active sector. initialize one
        s_sector = FIRST_SECTOR;

        for ( ;; )
        {
            if ( EraseSector())
            {
                s_nextPageAddressAfterCompact = 0xFFFFu;
                if ( WriteSectorHeader(INITIAL_SECTOR_SEQUENCE_NUMBER))
                {
                    break;
                }
            }

            s_sector++;
            if ( s_sector >= (FIRST_SECTOR + SECTOR_COUNT) )
            {
                // all sectors failed to initialize
                assert(false);
            }
        }

        s_itemCount = 0u;
    }
    else
    {
        // load active sector
        s_sector = lastSector;

        LoadSector(revisionNumber);
    }
    s_earlyInitDone = true;
}


void S_Nv_Init(void)
{
    if (!s_earlyInitDone)
    {
        S_Nv_EarlyInit();
    }

    eraseSectorTimer.handler = eraseSectorTimerFired;
    eraseSectorTimer.timeout = ERASE_SECTOR_DELAY_MS;
    eraseSectorTimer.interval = ERASE_SECTOR_DELAY_MS;
    eraseSectorTimer.mode = SYS_TIMER_INTERVAL_MODE;
}

/** Interface function, see \ref S_Nv_ItemInit. */
#if defined(S_XNV_LOGGING)
static S_Nv_ReturnValue_t S_Nv_ItemInit_Original(S_Nv_ItemId_t id, uint16_t itemLength, void* pDefaultData)
#else
S_Nv_ReturnValue_t S_Nv_ItemInit_Impl(S_Nv_ItemId_t id, uint16_t itemLength, void* pDefaultData)
#endif
{
    assert((id != 0u) && (id < 0xF000u) && (itemLength <= MAX_ITEM_LENGTH));

    if ( FindItem(id) != 0x0000u )
    {
#if (!defined(DISABLE_NV_RESIZE))
        // Item is already initialized. Check if the item length has been changed.
        uint16_t oldItemLength = S_Nv_ItemLength(id);
        if ( oldItemLength != itemLength )
        {
            // Resize this item to the new length
            s_compactItemId = id;
            s_compactItemLength = itemLength;

            S_Nv_ReturnValue_t resizeResult = CompactItem();
            if ( resizeResult != S_Nv_ReturnValue_Ok )
            {
                return resizeResult;
            }
        }
#endif
        if (pDefaultData != NULL)
        {
            return S_Nv_Read(id, 0u, itemLength, pDefaultData);
        }
        else
        {
            return S_Nv_ReturnValue_Ok;
        }
    }

    if ( PowerSupplyTooLow() )
    {
        return S_Nv_ReturnValue_PowerSupplyTooLow;
    }

    CompactSectorIfNeeded(BLOCK_HEADER_SIZE + itemLength);

    BlockHeader_t blockHeader;

    // need to add a new item. check if possible
    assert(s_itemCount < MAX_ITEM_COUNT);

    uint16_t newItemId = id;
    uint16_t newItemPointer = s_sectorHead;

    blockHeader.id = newItemId;
    blockHeader.blockOffset = 0x0000u;
    blockHeader.blockLength = itemLength;
    blockHeader.itemLength = itemLength;
    blockHeader.previousBlock = 0x0000u;
    blockHeader.writeCount = 0u;
    if ( !WriteDataBlockAndHeader(&blockHeader, (uint8_t*) pDefaultData) )
    {
        return S_Nv_ReturnValue_Failure;
    }

    // After successful write, create the cache
    Item_t *newItemCache = CreateItemCache(newItemId);
    newItemCache->lastBlock = newItemPointer;

    return S_Nv_ReturnValue_DidNotExist;
}

/** Interface function, see \ref S_Nv_Write. */
#if defined(S_XNV_LOGGING)
static S_Nv_ReturnValue_t S_Nv_Write_Original(S_Nv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
#else
S_Nv_ReturnValue_t S_Nv_Write_Impl(S_Nv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
#endif
{
    assert((id != 0u) && (pData != NULL));

    if ( PowerSupplyTooLow() )
    {
        return S_Nv_ReturnValue_PowerSupplyTooLow;
    }

    CompactSectorIfNeeded(BLOCK_HEADER_SIZE + dataLength);

    Item_t *cache = FindItemCache(id);

    // Caches will not exist for deleted or uninitialized items
    if ( cache == NULL )
    {
        return S_Nv_ReturnValue_DoesNotExist;
    }

    uint16_t blockPointer = cache->lastBlock;

    BlockHeader_t blockHeader;

    // read last written item block header
    D_Nv_Read(s_sector, blockPointer, (uint8_t*) &blockHeader, BLOCK_HEADER_SIZE);

    // check that we do not write beyond the length of the item
    if ( ((uint32_t) offset + (uint32_t) dataLength) > (uint32_t) blockHeader.itemLength )
    {
        return S_Nv_ReturnValue_BeyondEnd;
    }

    // write item block
    blockHeader.blockOffset = offset;
    blockHeader.blockLength = dataLength;
    blockHeader.previousBlock = blockPointer;
    if ( blockHeader.blockLength == blockHeader.itemLength )
    {
        // complete write. reset counter
        blockHeader.writeCount = 0u;
    }
    else
    {
        // partial write. increment counter
        blockHeader.writeCount++;
    }

    // Need current sector head to update cache after write
    uint16_t newBlockPointer = s_sectorHead;

    if ( !WriteDataBlockAndHeader(&blockHeader, (uint8_t*) pData) )
    {
        return S_Nv_ReturnValue_Failure;
    }

    // Write succeeded, so update the cache
    cache->lastBlock = newBlockPointer;

    if ( blockHeader.writeCount > COMPACT_ITEM_THRESHOLD )
    {
        // schedule a compact item operation for this item.
        // this reads the complete item and writes a new block containing
        // the complete item. this limits the worst case read and compact
        // sector time.
        s_compactItemId = blockHeader.id;
        s_compactItemLength = 0u;           // no need to resize this item here

        // ignore the error if a timer cannot be started: the operation
        // is not required for a correct operation of the component
        if (!SYS_TimerStarted(&compactItemTimer))
        {
        compactItemTimer.handler = compactItemTimerFired;
        compactItemTimer.timeout = COMPACT_ITEM_DELAY_MS;
        compactItemTimer.interval = COMPACT_ITEM_DELAY_MS;
        compactItemTimer.mode = SYS_TIMER_INTERVAL_MODE;
        //TODO: check timer to be started or not
        //SYS_StartTimer(&compactItemTimer);
        }
    }

    return S_Nv_ReturnValue_Ok;
}

/** Interface function, see \ref S_Nv_Read. */
#if defined(S_XNV_LOGGING)
static S_Nv_ReturnValue_t S_Nv_Read_Original(S_Nv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
#else
S_Nv_ReturnValue_t S_Nv_Read_Impl(S_Nv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
#endif
{
    assert((id != 0u) && (pData != NULL));

    // get the pointer to the last written block for the item
    uint16_t lastBlockPointer = FindItem(id);
    if ( lastBlockPointer == 0x0000u )
    {
        // item does not exist
        return S_Nv_ReturnValue_DoesNotExist;
    }

    // prevent overflow when calculating the range of bytes to read
    if ( ((uint32_t) offset + (uint32_t) dataLength) >= 0x10000uL )
    {
        return S_Nv_ReturnValue_BeyondEnd;
    }

    // gather the data into the destination buffer

    if ( !GatherData(s_sector, lastBlockPointer, offset, dataLength, pData ))
    {
        return S_Nv_ReturnValue_BeyondEnd;
    }

    return S_Nv_ReturnValue_Ok;
}

/** Interface function, see \ref S_Nv_ItemLength. */
uint16_t S_Nv_ItemLength_Impl(S_Nv_ItemId_t id)
{
    assert(id != 0u);

    uint16_t blockPointer = FindItem(id);
    if ( blockPointer == 0x0000u )
    {
        // item does not exist
        return 0u;
    }

    // read last written item block header
    BlockHeader_t blockHeader;
    D_Nv_Read(s_sector, blockPointer, (uint8_t*) &blockHeader, BLOCK_HEADER_SIZE);
    return blockHeader.itemLength;
}

/** Interface function, see \ref S_Nv_Delete. */
S_Nv_ReturnValue_t S_Nv_Delete_Impl(S_Nv_ItemId_t id)
{
    assert(id != 0u);

    if ( FindItem(id) == 0x0000u )
    {
        return S_Nv_ReturnValue_DoesNotExist;
    }

    if ( PowerSupplyTooLow() )
    {
        return S_Nv_ReturnValue_PowerSupplyTooLow;
    }

    CompactSectorIfNeeded(BLOCK_HEADER_SIZE);

    // Delete item by writing a new header for it, stating 0 length
    BlockHeader_t blockHeader;

    blockHeader.id = id;
    blockHeader.blockOffset = 0x0000u;
    blockHeader.blockLength = 0;
    blockHeader.itemLength = 0u;
    blockHeader.previousBlock = 0x0000u;
    blockHeader.writeCount = 0u;

    if ( !WriteBlockHeader(&blockHeader) )
    {
        return S_Nv_ReturnValue_Failure;
    }
    DeleteItemCache(id);

    return S_Nv_ReturnValue_Ok;
}

/** Interface function, see \ref S_Nv_EraseAll.
 *
 * Important: EraseAll leaves the file system in a non-working state if
 * persistent items are also deleted. This is because all sectors are
 * deleted without initializing a new one or even clearing the cache,
 * so reinitialization of the driver is needed.
 */
S_Nv_ReturnValue_t S_Nv_EraseAll_Impl(bool includingPersistentItems)
{
    //PDS_EraseSecureItems();

    if ( PowerSupplyTooLow() )
    {
        return S_Nv_ReturnValue_PowerSupplyTooLow;
    }

    if ( includingPersistentItems )
    {
        for ( uint8_t sector = FIRST_SECTOR; sector < (FIRST_SECTOR + SECTOR_COUNT); sector++ )
        {
            D_Nv_EraseSector(sector);
        }
    }
    else
    {
        uint8_t deletedItems = 0;
        uint8_t totalItems = s_itemCount;

        // Traverse the item cache, removing all the non-persistent
        for ( uint8_t cacheIndex = 0; cacheIndex < totalItems; cacheIndex++ )
        {
            uint16_t id = s_itemCache[cacheIndex].id;

            if ( !IsPersistent(id) )
            {
                DeleteItemCache(id);
                deletedItems++;
            }
        }

        // Were any items deleted? If so, do sector compaction!
        if ( deletedItems > 0 )
        {
            if ( !CompactSector() )
            {
                assert(false);
            }
        }
    }

    // Only way we end up here is if all went well.
    return S_Nv_ReturnValue_Ok;
}

/** Interface function, see \ref S_Nv_SetPowerSupplyCheckingFunction. */
void S_Nv_SetPowerSupplyCheckingFunction_Impl(S_Nv_PowerSupplyCheckingFunction_t pf)
{
    s_powerSupplyCheckingFunction = pf;
}
/** Interface function, see \ref S_Nv_IsItemAvailable.
 *
 * Important: This will check whether the item is found in the storage area
 */
bool S_Nv_IsItemAvailable_Impl(S_Nv_ItemId_t id)
{
  return ( FindItem(id) != 0x0000u );
}

/* Compact sector for application control */
void S_Nv_CompactSector_Impl(void)
{
   if ( !CompactSector() )
      {
          assert(false);
      }
}

#if defined(S_XNV_LOGGING)

S_Nv_ReturnValue_t S_Nv_ItemInit_Impl(S_Nv_ItemId_t id, uint16_t itemLength, void* pDefaultData)
{
    //hal_flash_readCount = 0u;
    //hal_flash_readSize = 0u;
    //hal_flash_writeCount = 0u;
    //hal_flash_writeSize = 0u;
 //   uint32_t stopwatch = N_Util_TimerStart();

    S_Nv_ReturnValue_t ret = S_Nv_ItemInit_Original(id, itemLength, pDefaultData);

    //PRINTA (("item_init(id=%X, l=%u, t=%lu, r=%u:%u, w=%u:%lu, h=%u): %hu",
        //id, itemLength,
        //N_Util_TimerElapsed(stopwatch),
////        hal_flash_readCount, hal_flash_readSize, hal_flash_writeCount, 4uL * hal_flash_writeSize,
        //0u, 0u, 0u, 0uL,
        //s_sectorHead,
        //ret));

    return ret;
}

S_Nv_ReturnValue_t S_Nv_Write_Impl(S_Nv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
{
    //hal_flash_readCount = 0u;
    //hal_flash_readSize = 0u;
    //hal_flash_writeCount = 0u;
    //hal_flash_writeSize = 0u;
    uint32_t stopwatch = N_Util_TimerStart();

    S_Nv_ReturnValue_t ret = S_Nv_Write_Original(id, offset, dataLength, pData);

    N_LOG_ALWAYS(("write(id=%Xh, o=%u, l=%u, t=%lu, r=%u:%u, w=%u:%lu, h=%u): %hu",
        id, offset, dataLength,
        N_Util_TimerElapsed(stopwatch),
//        hal_flash_readCount, hal_flash_readSize, hal_flash_writeCount, 4uL * hal_flash_writeSize,
        0u, 0u, 0u, 0uL,
        s_sectorHead,
        ret));

    return ret;
}

S_Nv_ReturnValue_t S_Nv_Read_Impl(S_Nv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
{
    //hal_flash_readCount = 0u;
    //hal_flash_readSize = 0u;
    //hal_flash_writeCount = 0u;
    //hal_flash_writeSize = 0u;
    uint32_t stopwatch = N_Util_TimerStart();

    S_Nv_ReturnValue_t ret = S_Nv_Read_Original(id, offset, dataLength, pData);

    N_LOG_ALWAYS(("read(id=%Xh, o=%u, l=%u, t=%lu, r=%u:%u): %hu",
        id, offset, dataLength,
        N_Util_TimerElapsed(stopwatch),
//        hal_flash_readCount, hal_flash_readSize,
        0u, 0u,
        ret));

    return ret;
}

#endif  // #if defined(S_XNV_LOGGING)

#endif // PDS_USE_EXTERNAL_FLASH
#endif 
