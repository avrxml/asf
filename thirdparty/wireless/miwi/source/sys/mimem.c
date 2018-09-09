/**
* \file  mimem.c
*
* \brief Memory Buffer handling for MiWi Protocol implementation
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

/************************ HEADERS **********************************/
#include <string.h>
#include "compiler.h"
#include "mimem.h"

/************************ MACRO DEFINITIONS ******************************/
#define BYTE_ALIGNMENT   4U
#define ALIGNMENT_FACTOR 2U

#define HEAP_SIZE      (6144UL)

#define ALIGN(X)       ((((X-1) >> ALIGNMENT_FACTOR) << ALIGNMENT_FACTOR) + BYTE_ALIGNMENT)

#define HEAP_MINIMUM_BLOCK_SIZE	 (( size_t )( blockMetaDataSize + 4U)) //4 is min bytes being allocated with alignment

/************************ TYPE DEFINITIONS ******************************/
typedef struct _Block_t
{
	struct _Block_t* next;
	struct _Block_t* prev;
	size_t size;
	bool free;
} Block_t;

/************************ FUNCTION PROTOTYPES **********************/
static void splitBlock (Block_t* blockTobeSplitted, size_t size);

/************************ VARIABLES ********************************/
static uint8_t heapMem[HEAP_SIZE];
static Block_t* base = NULL;
static size_t totalFreeBytesRemaining;
static const size_t blockMetaDataSize = ALIGN(sizeof(Block_t));

/************************ FUNCTIONS ********************************/

/*********************************************************************
* Function:         void MiMem_Init()
*
* PreCondition:     none
*
* Input:		    none
*
* Output:		    none
*
* Overview:		    This function will initialize the Mimem buffers
********************************************************************/
void MiMem_Init(void)
{
	size_t startAddress =  ALIGN((size_t)heapMem);

	/*Aligned heap is the base */
	base = (Block_t*) startAddress;
	base->size = totalFreeBytesRemaining = HEAP_SIZE - (startAddress - (size_t)heapMem);
	base->free = true;
	base->next = NULL;
	base->prev = NULL;
}

/*********************************************************************
* Function:         uint8_t* MiMem_Alloc(uint8_t size)
*
* PreCondition:     none
*
* Input:		    size  - Required number of bytes
*
* Output:		    uint8_t*_t - Pointer to the allocated memory or NULL
*
* Side Effects:	    none
*
* Overview:		    This function returns valid pointer if memory is allocated
*  or returns NULL if no memory available
*
* Note:			    none
********************************************************************/
uint8_t* MiMem_Alloc(uint8_t size)
{
	size_t requestedSize = 0U;
	void* requestedMemPtr = NULL;

	/* Initialize the Heap */
	if (!base)
	{
		MiMem_Init();
	}
	/* if requested size is of non zero */
	if (size)
	{
		requestedSize =  size + blockMetaDataSize;
		requestedSize = ALIGN(requestedSize);
		if (requestedSize <= totalFreeBytesRemaining)
		{
			size_t receivedSize = (size_t)~0U;
			Block_t *requestedBlock = NULL;
			Block_t *blockPtr = base;

			/* Find best fit free Block */
			while (blockPtr)
			{
				if ((blockPtr->free) && (blockPtr->size >= requestedSize) && (blockPtr->size < receivedSize))
				{
					receivedSize = blockPtr->size;
					requestedBlock = blockPtr;
				}
				blockPtr = blockPtr->next;
			}

			if (requestedBlock)
			{
				if ((requestedBlock->size - requestedSize) > HEAP_MINIMUM_BLOCK_SIZE)
				splitBlock (requestedBlock, requestedSize);
				requestedBlock->free = false;
				totalFreeBytesRemaining -= requestedBlock->size;
				requestedMemPtr = ( void* )(((uint8_t*)requestedBlock) + blockMetaDataSize);
			}
		}
	}
	if (NULL != requestedMemPtr)
	{
		memset(requestedMemPtr, 0, size);
	}
	return requestedMemPtr;
}

/*********************************************************************
* Function:         uint8_t MiMem_Free(uint8_t* buffPtr)
*
* PreCondition:     none
*
* Input:		    buffPtr  - Pointer to memory to be deallocated
*
* Output:		    void
*
* Side Effects:	    none
*
* Overview:		    This function returns SUCCESS if it deallocates
* the given memory else returns error
*
* Note:			    none
********************************************************************/
void MiMem_Free(void *ptr)
{
	Block_t* blockPtr = base;
	Block_t* freeBlockPtr = (Block_t*)((uint8_t*)ptr - (uint8_t*)blockMetaDataSize);

	for(; ((blockPtr != NULL) && (blockPtr != freeBlockPtr)); blockPtr = blockPtr->next);

	if (blockPtr)
	{
		blockPtr->free = true;
		totalFreeBytesRemaining += blockPtr->size;

		/* if the previous block this freed block is free, merge them */
		if (blockPtr->prev && blockPtr->prev->free)
		{
			blockPtr->prev->size += blockPtr->size;
			if (blockPtr->next)
			blockPtr->next->prev = blockPtr->prev;
			blockPtr->prev->next = blockPtr->next;
			blockPtr = blockPtr->prev;
		}
		/* if the next block of this freed block is free, merge them */
		if (blockPtr->next && blockPtr->next->free)
		{
			blockPtr->size += blockPtr->next->size ;
			blockPtr->next = blockPtr->next->next;
			if (blockPtr->next)
			blockPtr->next->prev = blockPtr;
		}
	}
}

/******************************************************************************
  \brief Split the large chunk into two if the existing memory is larger than 
    the requested size
  \param[in] blockTobeSplitted - block to be splited
  \param[in] size size of the memory chunk requested
  \return None.
 ******************************************************************************/
static void splitBlock(Block_t* blockTobeSplitted, size_t size)
{
	uint8_t* ptr = (uint8_t*)blockTobeSplitted + size;
	Block_t* newBlock = (Block_t*)ptr;

	newBlock->size = blockTobeSplitted->size - size;

	if (blockTobeSplitted->next)
		blockTobeSplitted->next->prev = newBlock;

	newBlock->next = blockTobeSplitted->next;
	newBlock->prev = blockTobeSplitted;
	newBlock->free = true;

	blockTobeSplitted->next = newBlock;
	blockTobeSplitted->size = size;
}

uint8_t MiMem_PercentageOfFreeBuffers(void)
{
	return (totalFreeBytesRemaining * 100) / HEAP_SIZE;;
}
