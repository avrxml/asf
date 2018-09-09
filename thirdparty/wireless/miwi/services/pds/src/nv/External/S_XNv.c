/**
* \file  S_XNv.c
*
* \brief External Nv Driver implementation
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
#ifdef PDS_USE_EXTERNAL_FLASH
/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "S_XNv_Bindings.h"
#include "S_XNv_Init.h"
#include "S_XNv.h"

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/

#include "D_XNv.h"
#include "N_ErrH.h"
#include "N_Log.h"
#include "N_Util.h"
#include "N_Task.h"
#include "N_Timer.h"
#include "N_Types.h"
#include "wlPdsMemIds.h"
#include <sysTimer.h>

//#include "S_Nv_Platform_Ids.h" // layering violation!

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#if defined __IAR_SYSTEMS_ICC__
//#define S_XNV_LOGGING
#endif

#define COMPID "S_XNv"

/** The maxumum number of items. */
#define MAX_ITEM_COUNT   PDS_MAX_ID
/** The maxumum length of an item. */
#define MAX_ITEM_LENGTH  1024u

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

/** Perform a compact sector operation if the sector head is larger then this (2 KiB remaining). */
#define IMMEDIATE_COMPACT_SECTOR_THRESHOLD 0xF800u

/** Perform a compact sector operation with a delay if the sector head is larger than this (8 KiB remaining). */
#define PREEMPTIVE_COMPACT_SECTOR_THRESHOLD 0xE000u

/** The size of the sector header. */
#define SECTOR_HEADER_SIZE ((uint16_t) sizeof(SectorHeader_t))
/** The size of the block header. */
#define BLOCK_HEADER_SIZE ((uint16_t) sizeof(BlockHeader_t))

/** The size of one item in the meta block. */
#define META_ITEM_SIZE ((uint16_t) sizeof(Item_t))

/** The sequence number to use for the initial sector. */
#define INITIAL_SECTOR_SEQUENCE_NUMBER 0xFFFFFFFEuL

/** The buffer size used while reading the meta block (number of items, not bytes). */
#define ITEM_BUFFER_LENGTH 4u

/** The item id used for storing the meta pointer block. */
#define META_POINTER_BLOCK_ID 0xFFFEu

/** The threshold in bytes after which the meta pointer block is updated. */
#if !defined(S_XNV_META_POINTER_BLOCK_THRESHOLD)
    #define S_XNV_META_POINTER_BLOCK_THRESHOLD 1024u
#endif

/* The index of the first sector to use for S_XNv and the number of sectors to use.
   Note that S_OTA uses sectors 0..3 */
#if !defined(S_XNV_FIRST_SECTOR)
#define S_XNV_FIRST_SECTOR 4u
#endif

#if !defined(S_XNV_SECTOR_COUNT)
#define S_XNV_SECTOR_COUNT 4u
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
typedef struct SectorHeader_t
{
    /** Is this sector active. Written with 0x0000 at the end of the compact operation. */
    uint16_t isActive;
    /** Signature to detect valid sectors. Must have the value "S_XNv2". */
    uint8_t signature[6];
    /** Counter, decreased each time a new sector becomes the active sector. */
    uint32_t sequenceNumber;
    /** Parity bits for the sequenceNumber field = sequenceNumber ^ 0xFFFFFFFFuL. */
    uint32_t sequenceParity;
} SectorHeader_t;

typedef struct BlockHeader_t
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
} BlockHeader_t;

/** Structure used to store where to find an item. */
typedef struct Item_t
{
    /** Item identifier. */
    uint16_t id;
    /** Pointer to the last written block for this item. */
    uint16_t lastBlock;
} Item_t;

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

static const uint16_t s_persistentItems[] = { };

/** The active sector. */
static uint8_t s_sector;
/** Location of the first unprogrammed byte in the active sector. */
static uint16_t s_sectorHead;

/** Location of the last written meta block in the active sector. 0x0000 if there is none. */
static uint16_t s_metaPointer = 0x0000u;

static uint8_t s_itemCount = 0u;

/** To reduce the number of meta block writes, one item (s_cachedItemId) is cached in RAM. Its location is s_cachedItemPointer. */
static uint16_t s_cachedItemId = 0u;
static uint16_t s_cachedItemPointer;

/** The sector to erase in the EVENT_ERASE_SECTOR handler. */
static uint8_t s_sectorToErase = 0xFFu;

/** The item to perform a compact operation on and the length of this item. */
static uint16_t s_compactItemId = 0x0000u;
static uint16_t s_compactItemLength = 0x0000u;

/** Callback function called before changing flash contents. */
static S_XNv_PowerSupplyCheckingFunction_t s_powerSupplyCheckingFunction = NULL;

static HAL_AppTimer_t eraseSectorTimer;
static SYS_Timer_t compactSectorTimer;
static SYS_Timer_t compactItemTimer;

/** Index to the first available index in the meta pointer block */
static uint8_t s_metaPointerBlockIndex = 0u;

/** Check to see if we need to do a compact sector at the Init */
static bool s_foundValidMetaPointerBlock = FALSE;

/** Check if the early init function is called already. */
static bool s_earlyInitDone = FALSE;

/***************************************************************************************************
* LOCAL FUNCTION DECLARATIONS
***************************************************************************************************/
static void eraseSectorTimerFired(void);
static void compactSectorTimerFired(void);
static void compactItemTimerFired(void);
static bool PowerSupplyTooLow(void);
static bool CompactSector(void);
static S_XNv_ReturnValue_t CompactItem(void);

/***************************************************************************************************
* LOCAL FUNCTIONS
***************************************************************************************************/

/** Erase sector timer callback.
*/
static void eraseSectorTimerFired(void)
{
    if (!PowerSupplyTooLow())
        D_XNv_EraseSector(s_sectorToErase);
}

/** Compact sector timer callback.
*/
static void compactSectorTimerFired(void)
{
    if (!PowerSupplyTooLow())
        if (!CompactSector())
            N_ERRH_FATAL();
}

/** Compact item timer callback.
*/
static void compactItemTimerFired(void)
{
    (void)CompactItem();
}

/** Check the power supply.
    \returns TRUE if the power supply is too low, FALSE when the the power
             supply is OK or when there is no callback installed.
*/
static bool PowerSupplyTooLow()
{
    bool result = FALSE;
    if ( s_powerSupplyCheckingFunction != NULL )
    {
        if ( !s_powerSupplyCheckingFunction() )
        {
            result = TRUE;
        }
    }
    return result;
}

static bool IsPersistent(uint16_t id)
{
    for ( uint8_t i = 0u; i < N_UTIL_ARRAY_SIZE(s_persistentItems); i++ )
    {
        if ( id == s_persistentItems[i] )
        {
            return TRUE;
        }
    }
    return FALSE;
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
            return FALSE;
        }
    }

    return TRUE;
}

static void UpdateSectorHead(uint16_t increment)
{
    // make sure that the sector head is aligned to 16 bytes
    s_sectorHead = (s_sectorHead + increment + 0x000Fu) & 0xFFF0u;
}

/** Return a pointer to the last written block for the item.
    \param id The id to find
    \returns A pointer to the last written block, or 0x0000u if the item was not found
*/
static uint16_t FindItem(uint16_t id)
{
    // first see if the item is cached
    if ( s_cachedItemId == id )
    {
        return s_cachedItemPointer;
    }

    // item not cached, read the meta block
    Item_t itemBuffer[ITEM_BUFFER_LENGTH];

    for ( uint8_t blockIndex = 0u; blockIndex < s_itemCount; blockIndex += ITEM_BUFFER_LENGTH )
    {
        uint8_t count = s_itemCount - blockIndex;
        if ( count > ITEM_BUFFER_LENGTH )
        {
            count = ITEM_BUFFER_LENGTH;
        }

        D_XNv_Read(s_sector, s_metaPointer + BLOCK_HEADER_SIZE + (blockIndex * META_ITEM_SIZE), (uint8_t*) itemBuffer, (uint16_t) (count * sizeof(Item_t)));
        // search for the id in the buffer
        for ( uint8_t i = 0u; i < count; i++ )
        {
            if ( itemBuffer[i].id == id )
            {
                return itemBuffer[i].lastBlock;
            }
        }
    }

    // item not found
    return 0x0000u;
}

static uint16_t ComputeCrc(uint8_t* pData, uint16_t length)
{
    uint16_t crc = 0xFFFFu;

    for ( /* empty */ ; length != 0u; length-- )
    {
        uint8_t x;
        x = (uint8_t) ((crc >> 8) ^ *pData);
        x = x ^ (x >> 4);
        crc = (crc << 8) ^ ((uint16_t) x << 12) ^ ((uint16_t) x << 5) ^ (uint16_t) x;

        pData++;
    }

    return crc;
}

static uint16_t ComputeHeaderCrc(BlockHeader_t* pBlockHeader)
{
    return ComputeCrc(((uint8_t*) pBlockHeader) + 2u, 12u); // skip isActive and headerCrc
}

static bool WriteAndCheck(uint16_t offset, uint8_t* pData, uint16_t length)
{
    D_XNv_Write(s_sector, offset, pData, length);
    return D_XNv_IsEqual(s_sector, offset, pData, length);
}

static void UpdateMetaPointerBlock(uint16_t metaBlockPointer)
{
    if (metaBlockPointer > (uint16_t)(s_metaPointerBlockIndex * S_XNV_META_POINTER_BLOCK_THRESHOLD))
    {
        BlockHeader_t metaPointerBlockHeader;
        D_XNv_Read(s_sector, SECTOR_HEADER_SIZE, (uint8_t*)&metaPointerBlockHeader, BLOCK_HEADER_SIZE);
        if((metaPointerBlockHeader.id == META_POINTER_BLOCK_ID) && (metaPointerBlockHeader.isActive == 0u))
        {
            // We have a meta pointer block so write it.
            uint16_t offset = SECTOR_HEADER_SIZE + BLOCK_HEADER_SIZE + (s_metaPointerBlockIndex * ((uint16_t)sizeof(metaBlockPointer)));
            D_XNv_Write(s_sector, offset, (uint8_t*)&metaBlockPointer, sizeof(metaBlockPointer));
            s_metaPointerBlockIndex++;
        }
    }
}

static bool WriteBlockHeader(BlockHeader_t* pBlockHeader)
{
    pBlockHeader->isActive = 0xFFFFu;
    pBlockHeader->headerCrc = ComputeHeaderCrc(pBlockHeader);

    if ( !WriteAndCheck(s_sectorHead, (uint8_t*) pBlockHeader, BLOCK_HEADER_SIZE) )
    {
        // failed to write header to flash.
        // check if any bits were programmed
        if ( !D_XNv_IsEmpty(s_sector, s_sectorHead, BLOCK_HEADER_SIZE) )
        {
            // the next write should skip this failed header as some bits have been programmed
            UpdateSectorHead(BLOCK_HEADER_SIZE);
            return FALSE;
        }
        else
        {
            // no bits were programmed (everything still is 0xFF).
            // the init function sees this as the start of the empty space
            // in the sector, so the next write should should attempt to
            // write to the same address
            return FALSE;
        }
    }
    else
    {
        UpdateSectorHead(BLOCK_HEADER_SIZE);
        return TRUE;
    }
}

static bool ActivateBlock(uint16_t blockPointer)
{
    uint16_t isActive = 0x0000u;
    if ( !WriteAndCheck(blockPointer, (uint8_t*) &isActive, sizeof(uint16_t)) )
    {
        // failed to activate the block
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

static bool WriteDataBlock(BlockHeader_t* pBlockHeader, uint8_t* pData)
{
    uint16_t blockPointer = s_sectorHead;

    if ( !WriteBlockHeader(pBlockHeader) )
    {
        return FALSE;
    }

    if ( pData != NULL )
    {
        if ( !WriteAndCheck(s_sectorHead, pData, pBlockHeader->blockLength) )
        {
            // failed to write data to flash.
            // skip the invalid data for the next write (even if everything still is 0xFF)
            UpdateSectorHead(pBlockHeader->blockLength);
            return FALSE;
        }
    }
    UpdateSectorHead(pBlockHeader->blockLength);

    if ( !ActivateBlock(blockPointer) )
    {
        // failed to activate the block
        return FALSE;
    }

    return TRUE;
}

static bool WriteMetaBlock(BlockHeader_t* pBlockHeader, uint16_t newItemId, uint16_t newItemPointer)
{
    Item_t itemBuffer[ITEM_BUFFER_LENGTH];

    // construct the new meta block header
    pBlockHeader->id = 0u;
    pBlockHeader->blockOffset = 0x0000u;
    pBlockHeader->blockLength = s_itemCount * META_ITEM_SIZE;
    if ( newItemId != 0u )
    {
        // add space for new item
        pBlockHeader->blockLength += META_ITEM_SIZE;
    }
    pBlockHeader->itemLength = pBlockHeader->blockLength;
    pBlockHeader->previousBlock = 0x0000u;
    pBlockHeader->writeCount = 0u;

    uint16_t blockPointer = s_sectorHead;

    if ( !WriteBlockHeader(pBlockHeader) )
    {
        return FALSE;
    }

    for ( uint8_t blockIndex = 0u; blockIndex < s_itemCount; blockIndex += ITEM_BUFFER_LENGTH )
    {
        uint8_t count = s_itemCount - blockIndex;
        if ( count > ITEM_BUFFER_LENGTH )
        {
            count = ITEM_BUFFER_LENGTH;
        }

        // read
        D_XNv_Read(s_sector, s_metaPointer + BLOCK_HEADER_SIZE + (blockIndex * META_ITEM_SIZE), (uint8_t*) itemBuffer, (uint16_t) (count * sizeof(Item_t)));
        // check if the cached item is in the buffer. if it is, then update the value in the buffer
        for ( uint8_t bufferIndex = 0u; bufferIndex < count; bufferIndex++ )
        {
            if ( itemBuffer[bufferIndex].id == s_cachedItemId )
            {
                itemBuffer[bufferIndex].lastBlock = s_cachedItemPointer;
            }
        }
        // write
        if ( !WriteAndCheck(s_sectorHead + (blockIndex * META_ITEM_SIZE), (uint8_t*) itemBuffer, (uint16_t) (count * sizeof(Item_t))) )
        {
            // failed to write data to flash.
            // skip the invalid data for the next write (even if everything still is 0xFF)
            UpdateSectorHead(pBlockHeader->blockLength);
            return FALSE;
        }
    }
    if ( newItemId != 0u )
    {
        // add the new item
        itemBuffer[0].id = newItemId;
        itemBuffer[0].lastBlock = newItemPointer;
        if ( !WriteAndCheck(s_sectorHead + (s_itemCount * META_ITEM_SIZE), (uint8_t*) itemBuffer, sizeof(Item_t)) )
        {
            // failed to write data to flash.
            // skip the invalid data for the next write (even if everything still is 0xFF)
            UpdateSectorHead(pBlockHeader->blockLength);
            return FALSE;
        }
    }
    UpdateSectorHead(pBlockHeader->blockLength);

    if ( !ActivateBlock(blockPointer) )
    {
        // failed to activate the block
        return FALSE;
    }

    s_metaPointer = blockPointer;

    // There is always one pointer in the meta pointer block
    UpdateMetaPointerBlock(blockPointer);

    return TRUE;
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
    // pointer to end of destination in flash
    uint16_t destinationPointer = 0u;

    // prepare the correct pointer
    if ( pData != NULL )
    {
        pDestination = (uint8_t*) pData + length;
    }
    else
    {
        destinationPointer = s_sectorHead + length;
    }

    BlockHeader_t blockHeader;
    uint16_t blockStart;
    uint16_t blockEnd;
    uint16_t count;

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
                return FALSE;
            }

            // get the header of the current block
            D_XNv_Read(sourceSector, currentBlockPointer, (uint8_t*) &blockHeader, BLOCK_HEADER_SIZE);

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
        if ( pDestination != NULL )
        {
            // this is for a read operation, so copy to buffer in RAM
            sourcePointer -= count;
            pDestination -= count;
            D_XNv_Read(s_sector, sourcePointer, pDestination, count);
        }
        else
        {
            // this is for a compact operation, so copy to the destination sector in flash
            while ( count > 0u )
            {
                uint8_t buffer[16];
                uint16_t c;
                if ( count > sizeof(buffer) )
                {
                    c = sizeof(buffer);
                    count -= (uint16_t) sizeof(buffer);
                }
                else
                {
                    c = count;
                    count = 0u;
                }

                destinationPointer -= c;
                sourcePointer -= c;
                D_XNv_Read(sourceSector, sourcePointer, buffer, c);
                if ( !WriteAndCheck(destinationPointer, buffer, c) )
                {
                    return FALSE;
                }
            }
        }
    }

    return TRUE;
}

static bool InitSector(uint32_t sequenceNumber)
{
    // make sure the sector is completely erased
    if ( !D_XNv_IsEmpty(s_sector, 0u, 0u) )
    {
        D_XNv_EraseSector(s_sector);
        // check if the erase succeeded
        if ( !D_XNv_IsEmpty(s_sector, 0u, 0u) )
        {
            return FALSE;
        }
    }

    // write sector header
    SectorHeader_t sectorHeader;
    sectorHeader.isActive = 0xFFFFu;
    sectorHeader.signature[0] = (uint8_t) 'S';
    sectorHeader.signature[1] = (uint8_t) '_';
    sectorHeader.signature[2] = (uint8_t) 'X';
    sectorHeader.signature[3] = (uint8_t) 'N';
    sectorHeader.signature[4] = (uint8_t) 'v';
    sectorHeader.signature[5] = (uint8_t) '2';
    sectorHeader.sequenceNumber = sequenceNumber;
    sectorHeader.sequenceParity = sequenceNumber ^ 0xFFFFFFFFuL;
    if ( !WriteAndCheck(0u, (uint8_t*) &sectorHeader, SECTOR_HEADER_SIZE) )
    {
        return FALSE;
    }

    s_sectorHead = SECTOR_HEADER_SIZE;

    BlockHeader_t metaPointerBlockHeader;
    metaPointerBlockHeader.id = META_POINTER_BLOCK_ID;
    metaPointerBlockHeader.blockOffset = 0u;
    metaPointerBlockHeader.blockLength = (uint16_t)((D_XNV_SECTOR_SIZE / S_XNV_META_POINTER_BLOCK_THRESHOLD) * sizeof(uint16_t));
    metaPointerBlockHeader.itemLength = (uint16_t)((D_XNV_SECTOR_SIZE / S_XNV_META_POINTER_BLOCK_THRESHOLD) * sizeof(uint16_t));
    metaPointerBlockHeader.previousBlock = 0x0000u;
    metaPointerBlockHeader.writeCount = 0u;

    // We do not care for the return value here. If it fails the init, will try again next reset.
    (void)WriteDataBlock(&metaPointerBlockHeader, NULL);

    return TRUE;
}

static bool ActivateSector(void)
{
    // activate sector header
    uint16_t isActive = 0x0000u;
    if ( !WriteAndCheck(0u, (uint8_t*) &isActive, sizeof(uint16_t)) )
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

/** Check the meta pointer block for a valid pointer to a meta block.
    If a pointer is found and it leads to a valid meta block. Copy the header to
    blockHeader.
    \param blockHeader Pointer to the meta pointer block header.

    \returns The address of the valid meta block or 0 if no valid meta block is found.
*/
static uint16_t GetValidMetaPointerFromMetaPointerBlock(BlockHeader_t * blockHeader)
{
    uint8_t buffer[16] = {0u};
    BlockHeader_t metaHeader;
    uint16_t lastBlock = 0u;
    if(blockHeader->itemLength > 16u)
    {
        lastBlock = ((blockHeader->itemLength - 16u) & 0xFFF0u);
    }

    // Read back all pointers in reverse. Done when an overflow occurs.
    uint16_t offset = lastBlock;
    while(offset <= lastBlock)
    {
        if(!D_XNv_IsEmpty(s_sector, s_sectorHead + BLOCK_HEADER_SIZE + offset, 16u))
        {
            D_XNv_Read(s_sector, s_sectorHead + BLOCK_HEADER_SIZE + offset, buffer, 16u);
            uint8_t index = 15u;
            // Done when an overflow occurs
            while(index <= 15u)
            {
                uint16_t metaPointer = N_Util_BuildUint16(buffer[index - 1u], buffer[index]);
                if((metaPointer != 0xFFFFu) && ((metaPointer % 16u) == 0u))
                {
                    D_XNv_Read(s_sector, metaPointer, (uint8_t*)&metaHeader, BLOCK_HEADER_SIZE);
                    uint16_t headerCrc = ComputeHeaderCrc(&metaHeader);
                    if((metaHeader.id == 0u) && (metaHeader.isActive == 0x0000u) && (metaHeader.headerCrc == headerCrc))
                    {
                        // Copy the valid header to blockHeader
                        (void)memcpy((void*)blockHeader, (void*)&metaHeader, sizeof(BlockHeader_t));
                        // calculate the meta pointer block index
                        // (currentBlock * itemsInABlock) + (currentItemInBlock) + 1 = ((offset / 16u) * 8) + ((index - 1)/2) + 1
                        uint16_t itemsLeft = (offset / 16u) * 8u;
                        uint16_t currentItemInBlock = (((uint16_t)index) - 1u) / 2u;
                        s_metaPointerBlockIndex = (uint8_t)(itemsLeft + currentItemInBlock + 1u);
                        return metaPointer;
                    }
                }
                index -= 2u;
            }
        }
        offset -= 16u;
    }
    return 0u;
}

static void LoadSector(void)
{
    // start after the sector header
    s_sectorHead = SECTOR_HEADER_SIZE;

    // keep track of the (last) item block that is after a meta block
    uint16_t lastItemId = 0u;
    uint16_t lastItemPointer = 0x0000u;

    // done when an overflow occurs: sectors are 64KiB
    while ( s_sectorHead >= SECTOR_HEADER_SIZE )
    {
        BlockHeader_t blockHeader;
        D_XNv_Read(s_sector, s_sectorHead, (uint8_t*) &blockHeader, BLOCK_HEADER_SIZE);

        if ( IsEmpty((uint8_t*) &blockHeader, BLOCK_HEADER_SIZE) )
        {
            // no header. done
            break;
        }
        else if ( blockHeader.headerCrc != ComputeHeaderCrc(&blockHeader) )
        {
            // invalid header. skip header
            UpdateSectorHead(BLOCK_HEADER_SIZE);
        }
        else if ( blockHeader.isActive != 0x0000u )
        {
            // inactive header. skip header and data
            UpdateSectorHead(BLOCK_HEADER_SIZE + blockHeader.blockLength);
        }
        else
        {
            // Valid meta pointer block found. Check validity and update sector head.
            if ( blockHeader.id == META_POINTER_BLOCK_ID )
            {
                uint16_t metaPointerBlockSize = blockHeader.blockLength;
                uint16_t metaPointer = GetValidMetaPointerFromMetaPointerBlock(&blockHeader);
                if(metaPointer != 0u)
                {
                    // Check the if the size of the meta pointer block is valid. If not we can still use it, but we should do a compact page
                    // to resize it.
                    uint16_t realMetaPointerBlockSize = (uint16_t)((D_XNV_SECTOR_SIZE / S_XNV_META_POINTER_BLOCK_THRESHOLD) * sizeof(uint16_t));
                    if(metaPointerBlockSize == realMetaPointerBlockSize)
                    {
                        // We have found a valid meta block via the meta pointer block.
                        s_foundValidMetaPointerBlock = TRUE;
                    }

                    // blockHeader now contains the header of the meta block.
                    // Only need to update the sector header, the code below will handle the meta block.
                    s_sectorHead = metaPointer;
                }
            }

            if ( blockHeader.id == 0u )
            {
                s_metaPointer = s_sectorHead;
                s_itemCount = (uint8_t) (blockHeader.itemLength / META_ITEM_SIZE);
            }
            else
            {
                // Don't need to cache the meta pointer block
                if(blockHeader.id != META_POINTER_BLOCK_ID)
                {
                    lastItemId = blockHeader.id;
                    lastItemPointer = s_sectorHead;
                }
            }
            UpdateSectorHead(BLOCK_HEADER_SIZE + blockHeader.blockLength);
        }
    }

    // initialize the cache, but only if the item is in the last written
    // meta block (it may have been deleted).
    // FindItem can be used here because the cache has not been initialized yet
    if ( lastItemId != 0u )
    {
        if ( FindItem(lastItemId) != 0x0000u )
        {
            s_cachedItemId = lastItemId;
            s_cachedItemPointer = lastItemPointer;
        }
    }
}

static bool CompactSector(void)
{
#if defined(ENABLE_NV_COMPACT_LOGGING)
    N_LOG_ALWAYS(("CompactSector(s=%hu)", s_sector));
#endif

    // stop timer for preemptive compact sector as this will not be needed any more
    // don't care if the task does does not yet exist (S_XNv_Init()) not yet called
    SYS_StopTimer(&compactSectorTimer);

    // stop the compact item as a compact sector will compact all items
    s_compactItemId = 0u;
    s_compactItemLength = 0u;

    uint8_t sourceSector = s_sector;

    // get the sector header for the source sector
    SectorHeader_t sectorHeader;
    D_XNv_Read(sourceSector, 0u, (uint8_t*) &sectorHeader, SECTOR_HEADER_SIZE);

    uint32_t nextSequenceNumber = sectorHeader.sequenceNumber - 1uL;

    // find and prepare a destination sector
    do
    {
        s_sector++;
        if ( s_sector >= (S_XNV_FIRST_SECTOR + S_XNV_SECTOR_COUNT) )
        {
            s_sector = S_XNV_FIRST_SECTOR;
        }

        if ( s_sector == sourceSector )
        {
            // all sector failed to initialize
            N_ERRH_FATAL();
        }
    }
    while ( !InitSector(nextSequenceNumber) );

    // start with a meta block. write the header now and the data when an item is compacted
    uint16_t newMetaPointer = s_sectorHead;

    BlockHeader_t blockHeader;
    blockHeader.id = 0u;
    blockHeader.blockOffset = 0x0000u;
    blockHeader.blockLength = s_itemCount * META_ITEM_SIZE;
    blockHeader.itemLength = blockHeader.blockLength;
    blockHeader.previousBlock = 0x0000u;
    blockHeader.writeCount = 0u;
    if ( !WriteBlockHeader(&blockHeader) )
    {
        return FALSE;
    }
    UpdateSectorHead(blockHeader.blockLength);

    // copy all items to the new page
    Item_t item;
    item.id = 0u;
    item.lastBlock = 0x0000u;
    for ( uint8_t blockIndex = 0u; blockIndex < s_itemCount; blockIndex++ )
    {
        // read next item from meta block
        D_XNv_Read(sourceSector, s_metaPointer + BLOCK_HEADER_SIZE + (blockIndex * META_ITEM_SIZE), (uint8_t*) &item, META_ITEM_SIZE);
        if ( item.id == s_cachedItemId )
        {
            // use the cached pointer instead of the pointer from the meta block for a cached item
            item.lastBlock = s_cachedItemPointer;
        }

        // read last written block for the item from the source sector
        D_XNv_Read(sourceSector, item.lastBlock, (uint8_t*) &blockHeader, BLOCK_HEADER_SIZE);

        uint16_t blockPointer = s_sectorHead;

        // write the block header to the destination sector. all data will be merged into one block
        blockHeader.blockOffset = 0u;
        blockHeader.blockLength = blockHeader.itemLength;
        blockHeader.previousBlock = 0x0000u;
        blockHeader.writeCount = 0u;
        if ( !WriteBlockHeader(&blockHeader) )
        {
            return FALSE;
        }

        // gather all data from the source sector and copy it to the destination sector
        if ( !GatherData(sourceSector, item.lastBlock, 0u, blockHeader.itemLength, NULL) )
        {
            return FALSE;
        }
        UpdateSectorHead(blockHeader.blockLength);

        // activate the block
        if ( !ActivateBlock(blockPointer) )
        {
            return FALSE;
        }

        // write item administration to meta block
        item.lastBlock = blockPointer;
        if ( !WriteAndCheck(newMetaPointer + BLOCK_HEADER_SIZE + (blockIndex * META_ITEM_SIZE), (uint8_t*) &item, META_ITEM_SIZE) )
        {
            return FALSE;
        }
    }
    s_cachedItemId = item.id;
    s_cachedItemPointer = item.lastBlock;

    // activate the meta block
    if ( !ActivateBlock(newMetaPointer) )
    {
        return FALSE;
    }
    s_metaPointer = newMetaPointer;

    // Update the meta pointer block
    s_metaPointerBlockIndex = 0u;
    UpdateMetaPointerBlock(newMetaPointer);

    // activate the sector
    if ( !ActivateSector() )
    {
        return FALSE;
    }

    // schedule an erase of the source sector.
    s_sectorToErase = sourceSector;

    // no problem if a task not yet active: the sector will be erased the next time it will be used.
    // Restart the timer if it is already running.
    HAL_StopAppTimer(&eraseSectorTimer);
    HAL_StartAppTimer(&eraseSectorTimer);

    return TRUE;
}

static void CompactSectorIfNeeded(void)
{
    if ( s_sectorHead > IMMEDIATE_COMPACT_SECTOR_THRESHOLD )
    {
        if ( !CompactSector() )
        {
            N_ERRH_FATAL();
        }
    }
    if ( s_sectorHead > PREEMPTIVE_COMPACT_SECTOR_THRESHOLD )
    {
        if (SYS_TIMER_STOPPED == compactSectorTimer.state)
            SYS_InitTimer(&compactSectorTimer, TIMER_ONE_SHOT_MODE, COMPACT_SECTOR_DELAY_MS, compactSectorTimerFired);

    }
}

static S_XNv_ReturnValue_t CompactItem(void)
{
#if defined(ENABLE_NV_COMPACT_LOGGING)
    N_LOG_ALWAYS(("CompactItem(id=%Xh)", s_compactItemId));
#endif

    if ( PowerSupplyTooLow() )
    {
        return S_XNv_ReturnValue_PowerSupplyTooLow;
    }

    CompactSectorIfNeeded();

    if ( (s_compactItemLength == 0u) && ( s_compactItemId == 0u ) )
    {
        // compact sector was performed since the compact item was
        // scheduled, so there is no need for another compact item unless
        // a resize of an item is needed.
        return S_XNv_ReturnValue_Ok;
    }

    uint16_t blockPointer = FindItem(s_compactItemId);
    if ( blockPointer == 0x0000u )
    {
        // trying to compact a non-existing item (item may have been deleted)
        return S_XNv_ReturnValue_Ok;
    }

    BlockHeader_t blockHeader;
    if ( s_compactItemId != s_cachedItemId )
    {
        // write meta block
        if ( !WriteMetaBlock(&blockHeader, 0u, 0x0000u) )
        {
            N_LOG_NONFATAL();
            return S_XNv_ReturnValue_Failure;
        }
    }

    // read last written item block header
    D_XNv_Read(s_sector, blockPointer, (uint8_t*) &blockHeader, BLOCK_HEADER_SIZE);

    uint16_t lastBlock = s_sectorHead;

    // write the block header to the destination sector. all data will be merged into one block
    uint16_t bytesToGather = blockHeader.itemLength;
    if ( s_compactItemLength != 0u )
    {
        N_LOG_ALWAYS(("Resizing NV item (id=%hu) from %hu to %hu", s_compactItemId, blockHeader.itemLength, s_compactItemLength));

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
    if ( !WriteBlockHeader(&blockHeader) )
    {
        N_LOG_NONFATAL();
        return S_XNv_ReturnValue_Failure;
    }

    // gather all data of the item and copy it to a new block
    if ( !GatherData(s_sector, blockPointer, 0u, bytesToGather, NULL) )
    {
        N_LOG_NONFATAL();
        return S_XNv_ReturnValue_Failure;
    }
    UpdateSectorHead(blockHeader.blockLength);

    // activate the block
    if ( !ActivateBlock(lastBlock) )
    {
        N_LOG_NONFATAL();
        return S_XNv_ReturnValue_Failure;
    }

    s_cachedItemId = s_compactItemId;
    s_cachedItemPointer = lastBlock;

    s_compactItemId = 0u;
    s_compactItemLength = 0u;

    return S_XNv_ReturnValue_Ok;
}

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

void S_XNv_EarlyInit(void)
{
    s_foundValidMetaPointerBlock = FALSE;
    s_metaPointerBlockIndex = 0u;

    s_metaPointer = 0x0000u;

    s_itemCount = 0u;
    s_cachedItemId = 0u;
    s_cachedItemPointer = 0x0000u;

    SectorHeader_t sectorHeader;

    uint8_t lastSector = 0xFFu;
    uint32_t lastSectorSequence = 0xFFFFFFFFuL;

    for ( uint8_t sector = S_XNV_FIRST_SECTOR; sector < (S_XNV_FIRST_SECTOR + S_XNV_SECTOR_COUNT); sector++ )
    {
        D_XNv_Read(sector, 0u, (uint8_t*) &sectorHeader, SECTOR_HEADER_SIZE);
        if ( (sectorHeader.isActive == 0x0000u) &&
             (sectorHeader.signature[0] == (uint8_t) 'S') &&
             (sectorHeader.signature[1] == (uint8_t) '_') &&
             (sectorHeader.signature[2] == (uint8_t) 'X') &&
             (sectorHeader.signature[3] == (uint8_t) 'N') &&
             (sectorHeader.signature[4] == (uint8_t) 'v') &&
             (sectorHeader.signature[5] == (uint8_t) '2') &&
             ((sectorHeader.sequenceNumber ^ sectorHeader.sequenceParity) == 0xFFFFFFFFuL) )
        {
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
        s_sector = S_XNV_FIRST_SECTOR;

        for ( ;; )
        {
            if ( InitSector(INITIAL_SECTOR_SEQUENCE_NUMBER) )
            {
                if ( ActivateSector() )
                {
                    break;
                }
            }

            s_sector++;
            if ( s_sector >= (S_XNV_FIRST_SECTOR + S_XNV_SECTOR_COUNT) )
            {
                // all sectors failed to initialize
                N_ERRH_FATAL();
            }
        }

        s_itemCount = 0u;
    }
    else
    {
        // load active sector
        s_sector = lastSector;

        LoadSector();
    }

    s_earlyInitDone = TRUE;
}

void S_XNv_Init(void)
{
    if (!s_earlyInitDone)
    {
        S_XNv_EarlyInit();
    }

    eraseSectorTimer.mode     = TIMER_ONE_SHOT_MODE;
    eraseSectorTimer.callback = eraseSectorTimerFired;
    eraseSectorTimer.interval = ERASE_SECTOR_DELAY_MS;
    
    if(!s_foundValidMetaPointerBlock)
    {
        // Have not found a valid meta pointer block. Schedule a compact page to create one.
        SYS_InitTimer(&compactSectorTimer, TIMER_ONE_SHOT_MODE, COMPACT_SECTOR_DELAY_MS, compactSectorTimerFired);
    }
}

/** Interface function, see \ref S_XNv_ItemInit. */
#if defined(S_XNV_LOGGING)
static S_XNv_ReturnValue_t S_XNv_ItemInit_Original(S_XNv_ItemId_t id, uint16_t itemLength, void* pDefaultData)
#else
S_XNv_ReturnValue_t S_XNv_ItemInit_Impl(S_XNv_ItemId_t id, uint16_t itemLength, void* pDefaultData)
#endif
{
    N_ERRH_ASSERT_FATAL((id != 0u) && (id < 0xF000u) && (itemLength <= MAX_ITEM_LENGTH));

    if ( FindItem(id) != 0x0000u )
    {
#if (!defined(DISABLE_NV_RESIZE))
        // Item is already initialized. Check if the item length has been changed.
        uint16_t oldItemLength = S_XNv_ItemLength(id);
        if ( oldItemLength != itemLength )
        {
            // Resize this item to the new length
            s_compactItemId = id;
            s_compactItemLength = itemLength;

            S_XNv_ReturnValue_t resizeResult = CompactItem();
            if ( resizeResult != S_XNv_ReturnValue_Ok )
            {
                return resizeResult;
            }
        }
#endif
        if (pDefaultData != NULL)
        {
            return S_XNv_Read(id, 0u, itemLength, pDefaultData);
        }
        else
        {
            return S_XNv_ReturnValue_Ok;
        }
    }

    if ( PowerSupplyTooLow() )
    {
        return S_XNv_ReturnValue_PowerSupplyTooLow;
    }

    CompactSectorIfNeeded();

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
    if ( !WriteDataBlock(&blockHeader, (uint8_t*) pDefaultData) )
    {
        return S_XNv_ReturnValue_Failure;
    }

    if ( !WriteMetaBlock(&blockHeader, newItemId, newItemPointer) )
    {
        return S_XNv_ReturnValue_Failure;
    }

    // first write the new meta block, as the item count should not be increased if this fails
    s_itemCount++;

    // first write the new meta block, as this needs the old cached data
    s_cachedItemId = newItemId;
    s_cachedItemPointer = newItemPointer;

    return S_XNv_ReturnValue_DidNotExist;
}

/** Interface function, see \ref S_XNv_Write. */
#if defined(S_XNV_LOGGING)
static S_XNv_ReturnValue_t S_XNv_Write_Original(S_XNv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
#else
S_XNv_ReturnValue_t S_XNv_Write_Impl(S_XNv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
#endif
{
    N_ERRH_ASSERT_FATAL((id != 0u) && (id < 0xF000u) && (pData != NULL));

    if ( PowerSupplyTooLow() )
    {
        return S_XNv_ReturnValue_PowerSupplyTooLow;
    }

    CompactSectorIfNeeded();

    uint16_t blockPointer = FindItem(id);
    if ( blockPointer == 0x0000u )
    {
        // item does not exist
        return S_XNv_ReturnValue_DoesNotExist;
    }

    BlockHeader_t blockHeader;
    // If the sector header is over S_XNV_META_POINTER_BLOCK_THRESHOLD because only cached items have been written,
    // write a new meta block
    if ( (s_cachedItemId != id) || ((s_sectorHead - s_metaPointer) > S_XNV_META_POINTER_BLOCK_THRESHOLD) )
    {
        // write meta block
        if ( !WriteMetaBlock(&blockHeader, 0u, 0x0000u) )
        {
            return S_XNv_ReturnValue_Failure;
        }
    }

    // read last written item block header
    D_XNv_Read(s_sector, blockPointer, (uint8_t*) &blockHeader, BLOCK_HEADER_SIZE);

    // check that we do not write beyond the length of the item
    if ( ((uint32_t) offset + (uint32_t) dataLength) > (uint32_t) blockHeader.itemLength )
    {
        return S_XNv_ReturnValue_BeyondEnd;
    }

    uint16_t lastBlock = s_sectorHead;

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
    if ( !WriteDataBlock(&blockHeader, (uint8_t*) pData) )
    {
        return S_XNv_ReturnValue_Failure;
    }

    if ( blockHeader.writeCount > COMPACT_ITEM_THRESHOLD )
    {
        // schedule a compact item operation for this item.
        // this reads the complete item and writes a new block containing
        // the complete item. this limits the worst case read and compact
        // sector time.
        s_compactItemId = blockHeader.id;
        s_compactItemLength = 0u;           // no need to resize this item here

        // if a timer cannot be started (S_XNv_Init() not done) - the operation
        // is not required for a correct operation of the component
        if (SYS_TIMER_STOPPED == compactItemTimer.state)
            SYS_InitTimer(&compactItemTimer, TIMER_ONE_SHOT_MODE, COMPACT_ITEM_DELAY_MS, compactItemTimerFired);
    }

    s_cachedItemId = blockHeader.id;
    s_cachedItemPointer = lastBlock;

    return S_XNv_ReturnValue_Ok;
}

/** Interface function, see \ref S_XNv_Read. */
#if defined(S_XNV_LOGGING)
static S_XNv_ReturnValue_t S_XNv_Read_Original(S_XNv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
#else
S_XNv_ReturnValue_t S_XNv_Read_Impl(S_XNv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
#endif
{
    N_ERRH_ASSERT_FATAL((id != 0u) && (id < 0xF000u) && (pData != NULL));

    // get the pointer to the last written block for the item
    uint16_t lastBlockPointer = FindItem(id);
    if ( lastBlockPointer == 0x0000u )
    {
        // item does not exist
        return S_XNv_ReturnValue_DoesNotExist;
    }

    // prevent overflow when calculating the range of bytes to read
    if ( ((uint32_t) offset + (uint32_t) dataLength) >= 0x10000uL )
    {
        return S_XNv_ReturnValue_BeyondEnd;
    }

    // gather the data into the destination buffer
    if ( !GatherData(s_sector, lastBlockPointer, offset, dataLength, pData) )
    {
        return S_XNv_ReturnValue_BeyondEnd;
    }

    return S_XNv_ReturnValue_Ok;
}

/** Interface function, see \ref S_XNv_ItemLength. */
uint16_t S_XNv_ItemLength_Impl(S_XNv_ItemId_t id)
{
    N_ERRH_ASSERT_FATAL((id != 0u) && (id < 0xF000u));

    uint16_t blockPointer = FindItem(id);
    if ( blockPointer == 0x0000u )
    {
        // item does not exist
        return 0u;
    }

    // read last written item block header
    BlockHeader_t blockHeader;
    D_XNv_Read(s_sector, blockPointer, (uint8_t*) &blockHeader, BLOCK_HEADER_SIZE);
    return blockHeader.itemLength;
}

/** Interface function, see \ref S_XNv_Delete. */
S_XNv_ReturnValue_t S_XNv_Delete_Impl(S_XNv_ItemId_t id)
{
    N_ERRH_ASSERT_FATAL((id != 0u) && (id < 0xF000u));

    if ( FindItem(id) == 0x0000u )
    {
        return S_XNv_ReturnValue_DoesNotExist;
    }

    if ( PowerSupplyTooLow() )
    {
        return S_XNv_ReturnValue_PowerSupplyTooLow;
    }

    CompactSectorIfNeeded();

    BlockHeader_t blockHeader;
    Item_t item;

    uint8_t newItemCount = s_itemCount - 1u;

    // construct the new meta block header
    blockHeader.id = 0u;
    blockHeader.blockOffset = 0x0000u;
    blockHeader.blockLength = newItemCount * META_ITEM_SIZE;
    blockHeader.itemLength = blockHeader.blockLength;
    blockHeader.previousBlock = 0x0000u;
    blockHeader.writeCount = 0u;

    uint16_t blockPointer = s_sectorHead;

    if ( !WriteBlockHeader(&blockHeader) )
    {
        return S_XNv_ReturnValue_Failure;
    }

    uint8_t destinationIndex = 0u;
    for ( uint8_t sourceIndex = 0u; sourceIndex < s_itemCount; sourceIndex++ )
    {
        D_XNv_Read(s_sector, s_metaPointer + BLOCK_HEADER_SIZE + (sourceIndex * META_ITEM_SIZE), (uint8_t*) &item, (uint16_t) sizeof(Item_t));
        if ( item.id != id )
        {
            if ( item.id == s_cachedItemId )
            {
                item.lastBlock = s_cachedItemPointer;
            }
            if ( !WriteAndCheck(s_sectorHead + (destinationIndex * META_ITEM_SIZE), (uint8_t*) &item, (uint16_t) sizeof(Item_t)) )
            {
                // failed to write data to flash.
                // skip the invalid data for the next write (even if everything still is 0xFF)
                UpdateSectorHead(blockHeader.blockLength);
                return S_XNv_ReturnValue_Failure;
            }
            destinationIndex++;
        }
    }
    UpdateSectorHead(blockHeader.blockLength);

    if ( !ActivateBlock(blockPointer) )
    {
        // failed to activate the block
        return S_XNv_ReturnValue_Failure;
    }

    s_metaPointer = blockPointer;

    // clear cache if the cached item is the deleted item
    if ( s_cachedItemId == id )
    {
        s_cachedItemId = 0u;
        s_cachedItemPointer = 0x0000u;
    }

    s_itemCount = newItemCount;

    return S_XNv_ReturnValue_Ok;
}

/** Interface function, see \ref S_XNv_EraseAll. */
S_XNv_ReturnValue_t S_XNv_EraseAll_Impl(bool includingPersistentItems)
{
    if ( PowerSupplyTooLow() )
    {
        return S_XNv_ReturnValue_PowerSupplyTooLow;
    }

    if ( includingPersistentItems )
    {
        for ( uint8_t sector = S_XNV_FIRST_SECTOR; sector < (S_XNV_FIRST_SECTOR + S_XNV_SECTOR_COUNT); sector++ )
        {
            D_XNv_EraseSector(sector);
        }
    }
    else
    {
        CompactSectorIfNeeded();

        BlockHeader_t blockHeader;
        Item_t item;

        // read the old meta block to see how many items are persistent
        uint8_t newItemCount = 0u;
        for ( uint8_t blockIndex = 0u; blockIndex < s_itemCount; blockIndex++ )
        {
            D_XNv_Read(s_sector, s_metaPointer + BLOCK_HEADER_SIZE + (blockIndex * META_ITEM_SIZE), (uint8_t*) &item, (uint16_t) sizeof(Item_t));
            // count persistent items, but skip the cached item
            if ( IsPersistent(item.id) )
            {
                newItemCount++;
            }
        }

        // construct the new meta block header
        blockHeader.id = 0u;
        blockHeader.blockOffset = 0x0000u;
        blockHeader.blockLength = newItemCount * META_ITEM_SIZE;
        blockHeader.itemLength = blockHeader.blockLength;
        blockHeader.previousBlock = 0x0000u;
        blockHeader.writeCount = 0u;

        uint16_t blockPointer = s_sectorHead;

        if ( !WriteBlockHeader(&blockHeader) )
        {
            return S_XNv_ReturnValue_Failure;
        }

        uint8_t destinationIndex = 0u;
        for ( uint8_t sourceIndex = 0u; sourceIndex < s_itemCount; sourceIndex++ )
        {
            D_XNv_Read(s_sector, s_metaPointer + BLOCK_HEADER_SIZE + (sourceIndex * META_ITEM_SIZE), (uint8_t*) &item, (uint16_t) sizeof(Item_t));
            if ( IsPersistent(item.id) )
            {
                if ( item.id == s_cachedItemId )
                {
                    item.lastBlock = s_cachedItemPointer;
                }
                if ( !WriteAndCheck(s_sectorHead + (destinationIndex * META_ITEM_SIZE), (uint8_t*) &item, (uint16_t) sizeof(Item_t)) )
                {
                    // failed to write data to flash.
                    // skip the invalid data for the next write (even if everything still is 0xFF)
                    UpdateSectorHead(blockHeader.blockLength);
                    return S_XNv_ReturnValue_Failure;
                }
                destinationIndex++;
            }
        }
        UpdateSectorHead(blockHeader.blockLength);

        if ( !ActivateBlock(blockPointer) )
        {
            // failed to activate the block
            return S_XNv_ReturnValue_Failure;
        }

        s_metaPointer = blockPointer;

        // clear cache as the cached item may be non-persistent
        s_cachedItemId = 0u;
        s_cachedItemPointer = 0x0000u;

        s_itemCount = newItemCount;
    }

    return S_XNv_ReturnValue_Ok;
}

/** Interface function, see \ref S_XNv_SetPowerSupplyCheckingFunction. */
void S_XNv_SetPowerSupplyCheckingFunction_Impl(S_XNv_PowerSupplyCheckingFunction_t pf)
{
    s_powerSupplyCheckingFunction = pf;
}

/** Interface function, see \ref S_XNv_IsItemAvailable. */
bool S_XNv_IsItemAvailable_Impl(S_XNv_ItemId_t id)
{
  return ( FindItem(id) != 0x0000u );
}

#if defined(S_XNV_LOGGING)

S_XNv_ReturnValue_t S_XNv_ItemInit_Impl(S_XNv_ItemId_t id, uint16_t itemLength, void* pDefaultData)
{
    //hal_flash_readCount = 0u;
    //hal_flash_readSize = 0u;
    //hal_flash_writeCount = 0u;
    //hal_flash_writeSize = 0u;
    uint32_t stopwatch = N_Util_TimerStart();

    S_XNv_ReturnValue_t ret = S_XNv_ItemInit_Original(id, itemLength, pDefaultData);

    N_LOG_ALWAYS(("item_init(id=%Xh, l=%u, t=%lu, r=%u:%u, w=%u:%lu, h=%u): %hu",
        id, itemLength,
        N_Util_TimerElapsed(stopwatch),
//        hal_flash_readCount, hal_flash_readSize, hal_flash_writeCount, 4uL * hal_flash_writeSize,
        0u, 0u, 0u, 0uL,
        s_sectorHead,
        ret));

    return ret;
}

S_XNv_ReturnValue_t S_XNv_Write_Impl(S_XNv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
{
    //hal_flash_readCount = 0u;
    //hal_flash_readSize = 0u;
    //hal_flash_writeCount = 0u;
    //hal_flash_writeSize = 0u;
    uint32_t stopwatch = N_Util_TimerStart();

    S_XNv_ReturnValue_t ret = S_XNv_Write_Original(id, offset, dataLength, pData);

    N_LOG_ALWAYS(("write(id=%Xh, o=%u, l=%u, t=%lu, r=%u:%u, w=%u:%lu, h=%u): %hu",
        id, offset, dataLength,
        N_Util_TimerElapsed(stopwatch),
//        hal_flash_readCount, hal_flash_readSize, hal_flash_writeCount, 4uL * hal_flash_writeSize,
        0u, 0u, 0u, 0uL,
        s_sectorHead,
        ret));

    return ret;
}

S_XNv_ReturnValue_t S_XNv_Read_Impl(S_XNv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
{
    //hal_flash_readCount = 0u;
    //hal_flash_readSize = 0u;
    //hal_flash_writeCount = 0u;
    //hal_flash_writeSize = 0u;
    uint32_t stopwatch = N_Util_TimerStart();

    S_XNv_ReturnValue_t ret = S_XNv_Read_Original(id, offset, dataLength, pData);

    N_LOG_ALWAYS(("read(id=%Xh, o=%u, l=%u, t=%lu, r=%u:%u): %hu",
        id, offset, dataLength,
        N_Util_TimerElapsed(stopwatch),
//        hal_flash_readCount, hal_flash_readSize,
        0u, 0u,
        ret));

    return ret;
}

#endif
#endif // #ifdef PDS_USE_EXTERNAL_FLASH
#endif
