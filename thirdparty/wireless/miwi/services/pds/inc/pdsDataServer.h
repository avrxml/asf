/**
* \file  pdsDataServer.h
*
* \brief The header file describes the Persistence Data Server interface
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

#ifndef _PERSISTDATASERVER_H
#define _PERSISTDATASERVER_H

/******************************************************************************
                              Description section
******************************************************************************/
/*
   - PERSISTENT DATA SERVER (PDS)
   User can specifiy parameters he(she) wants to backup in a non-volatile memory
   and restore in case of power failure. This service is provided by Persistent
   Data Server (PDS) module.
   The BitCloud (TM) stack also uses the same service for its internal structures.

   - FILE and FILE DESCRIPTOR
   Each item which user wants to backup in a non-volatile memory and restore in
   case of power failure is treated as a FILE - actual item value with associated
   service information, FILE DESCRIPTOR. Each file could be accessed by it's ID -
   a uniq 16-bit value associated with a file. File descriptor keeps information
   about item's size and it's displacement in RAM and inside the NV storage.

   The PDS_FILE_DESCR() macro is used to initialize descriptor and
   PDS_DECLARE_FILE() macro is used to place descriptor to required segment.

   - DIRECTORY and DIRECTORY DESCRIPTOR
   PDS is able to operate with separate files or with file lists - DIRECTORIES.
   Directory nesting is allowed. Each directory should be provided with
   DIRECTORY DESCRIPTOR which keeps information about associated items.
   Directories could be accessed by 16-bit ID, different from already associated
   with files.

   The PDS_DECLARE_DIR() macro is used to place a directory to required segment.
*/

/******************************************************************************
                              Includes section
******************************************************************************/


#include <pdsMemIds.h>
#include <compiler.h>

/* Includings for PDS with wear leveling support */
#ifdef PDS_ENABLE_WEAR_LEVELING
  #include <wlPdsTypes.h>
#endif /* PDS_ENABLE_WEAR_LEVELING == 1 */


/******************************************************************************
                              Types section
******************************************************************************/
#ifndef PDS_ENABLE_WEAR_LEVELING
/*! Type of PDS file or directory unique identifier. */
typedef uint16_t PDS_MemId_t;
typedef enum
{
	PDS_SUCCESS,            //!< Command completed successfully
} PDS_DataServerState_t;
#endif
/******************************************************************************
                              Defines section
******************************************************************************/
/* Highlight non-volatile memory writing with onboard LED */
//#define PDS_HIGHLIGHT_WRITING_PROCESS

/* PDS working status flags */
#define PDS_STOPPED_FLAG                 (1U << 0U)
#define PDS_WRITING_INPROGRESS_FLAG      (1U << 1U)
#define PDS_COMMITMENT_OUT_OFF_DATE_FLAG (1U << 2U)


//! \cond internal
/**************************************************************************//**
\brief Persistent Data Server initialization routine.
******************************************************************************/
void PDS_Init(void);
//! \endcond


/******************************************************************************
\brief PDS item initialization routine
******************************************************************************/
void PDS_InitItems(void);

/******************************************************************************
\brief Erases item under security control
******************************************************************************/
void PDS_EraseSecureItems(void);

/**************************************************************************//**
\brief Restores data from non-volatile storage.

\ingroup pds

PDS files not included in the current build configuration will be ignored.
Restoring process will be performed only if all files, expected for actual
configuration, are presented in NV storage.

\param[in] memoryId - an identifier of PDS file or directory to be restored
                      from non-volatile memory.

\return true, if all expected files have been restored, false - otherwise.
******************************************************************************/
bool PDS_Restore(PDS_MemId_t memoryId);

/**************************************************************************//**
\brief Stores data in non-volatile memory in background, not blocking other
       processes.

\ingroup pds

All PDS files which are absent in the current build configuration will be ignored.

\param[in] memoryId - an identifier of PDS file or directory to be stored
                      in non-volatile memory.

\return True, if storing process has begun, false - otherwise.
******************************************************************************/
bool PDS_Store(PDS_MemId_t memoryId);

/**************************************************************************//**
\brief Deletes data from non-volatile storage except the Persistant items
       depending on the parameter passed.

\param[in] includingPersistentItems - deletes persistant items if TRUE
                                      deletes all other items except persistant 
                                      items if FALSE

\return - Whether the operation was successful
******************************************************************************/
bool PDS_DeleteAll(bool includingPersistentItems);

/**************************************************************************//**
\brief Stores data in non-volatile memory in a synchronous way -
       the application execution will be blocked until the process is completed.

\ingroup pds

All PDS files which are absent in the current build configuration will be ignored.

\param[in] memoryId - an identifier of PDS file or directory to be stored
                      in non-volatile memory.

\return True, if storing process has been performed successfully, false - otherwise.
******************************************************************************/
bool PDS_BlockingStore(PDS_MemId_t memoryId);

/**************************************************************************//**
\brief Provides PDS with information about memory that should be kept up-to-date
       in the non-volatile storage.

\ingroup pds

Data will be saved im non-volatile memory upon specific events.

\param[in] memoryId - an identifier of PDS file or directory to be stored
                      in non-volatile memory.
******************************************************************************/
void PDS_StoreByEvents(PDS_MemId_t memoryId);

/**************************************************************************//**
\brief Enables periodic commitment of specified persistent items to a
       non-volatile storage.

\ingroup pds

\param[in] interval - period of time between successive commitments
\param[in] memoryId - an identifier of PDS file or directory to be stored
                      in non-volatile memory.
******************************************************************************/
void PDS_StoreByTimer(uint32_t interval, PDS_MemId_t memoryId);

/*****************************************************************************//**
\brief Removes specified file records from NV Storage

\ingroup pds

\param[in] memoryId - an identifier of PDS file or directory to be removed
                      from NV memory.

\return - Whether the operation was successful
******************************************************************************/
PDS_DataServerState_t PDS_Delete(PDS_MemId_t memoryId);

/**************************************************************************//**
\brief Continue Persistent Data Server operation the same way as before stopping

\ingroup pds
******************************************************************************/
void PDS_Run(void);

/**************************************************************************//**
\brief Stops the Persistent Data Server

\ingroup pds

This doesn't affect a policy created through PDS_StoreByEvent() of
PDS_StoreByTimer() functions. After calling the PDS_Run() function PDS
continues working as before the stop.
******************************************************************************/
void PDS_Stop(void);

/**************************************************************************//**
\brief Checks if the specified PDS file or directory can be restored
       from non-volatile memory

\ingroup pds

PDS files not included in the current build configuration will be ignored.

\param[in] memoryId - an identifier of PDS file or directory to be checked.

\return true, if the specified memory can be restored; false - otherwise.
******************************************************************************/
bool PDS_IsAbleToRestore(PDS_MemId_t memoryId);

/**************************************************************************//**
\brief Checks if the specified PDS file or directory can be stored
       in non-volatile memory

\ingroup pds

\param[in] memoryId - an identifier of PDS file or directory to be checked.

\return true, if the specified memory can be stored; false - otherwise.
******************************************************************************/
bool PDS_IsAbleToStore(PDS_MemId_t memoryId);

/**************************************************************************//**
\brief Checks if files specified have been restored from non-volatile memory
       during the latest restoring procedure

\ingroup pds

\param[in] memoryId - an identifier of PDS file or directory to be checked.

\return true, if the specified file(s) have been restored; false - otherwise.
******************************************************************************/
bool PDS_IsMemoryRestored(PDS_MemId_t memoryId);

/**************************************************************************//**
\brief Marks the specified restored PDS files to be cleared during a reset phase
      (during ZDO_StartNetwork() execution)

\ingroup pds

\param[in] memoryId - an identifier of PDS file or directory to be cleared.
******************************************************************************/
void PDS_ClearRestoredMemory(PDS_MemId_t memoryId);

/******************************************************************************
\brief This function is deprecated - use PDS_BlockingStore() instead
******************************************************************************/
void PDS_FlushData(PDS_MemId_t memoryIds);

/******************************************************************************
\brief This function is deprecated - use PDS_Delete() instead.
******************************************************************************/
PDS_DataServerState_t PDS_ResetStorage(PDS_MemId_t memoryId);

/******************************************************************************
\brief Checks, if PDS is busy with file writing, or not.

\return - True, if PDS is busy with file writing; false - otherwise
******************************************************************************/
bool PDS_IsBusy(void);

#ifdef PDS_ENABLE_WEAR_LEVELING
/**************************************************************************//**
\brief Creates a file in NV memory and initializes it with provided pattern.
       If file already exists, no action will be performed and PDS_SUCCESS
       status will be returned.

\ingroup pds

\param[in] memoryId - an identifier of PDS file to create
\param[in] initialValue - file data initial value; if NULL provided, then file
                       will be initialized with all 0xFF. The size of this
                       pattern should be the same as a file size, specified
                       in file descriptor.

\return operation status
******************************************************************************/
PDS_Status_t PDS_CreateFile(PDS_MemId_t memoryId, void *initialValue);

/**************************************************************************//**
\brief Writes a portion of data to a file. File could have no permanent entity
       in RAM.

\ingroup pds

\param[in] memoryId - an identifier of PDS file to update
\param[in] offset - offset of data part within a file
\param[in] dataLength - length of part to write
\param[in] data - data to write

\return operation status
******************************************************************************/
PDS_Status_t PDS_StoreFilePart(PDS_MemId_t memoryId, uint16_t offset,
  PDS_DataSize_t dataLength, void *data);

/**************************************************************************//**
\brief Reads a portion of data from a file. File could have no permanent entity
       in RAM.

\ingroup pds

\param[in] memoryId - an identifier of PDS file to read
\param[in] offset - offset of data part within a file
\param[in] dataLength - length of part to read
\param[in] data - buffer to place data part in

\return operation status
******************************************************************************/
PDS_Status_t PDS_RestoreFilePart(PDS_MemId_t memoryId, uint16_t offset,
  PDS_DataSize_t dataLength, void *data);

#endif /* PDS_ENABLE_WEAR_LEVELING == 1 */

/**************************************************************************//**
\brief Reads data from the user's area of NV memory.

\ingroup pds

\param[in] offset   - offset of data in user's area
\param[in] data     - pointer to memory, for reading data into
\param[in] length   - data length
\param[in] callback - pointer to a callback function; if callback is NULL,
                      then data will be read syncronously

\return - PDS state as a result of data reading operation
******************************************************************************/
PDS_DataServerState_t PDS_ReadUserData(uint16_t offset, uint8_t *data,
                                       uint16_t length, void (*callback)(void));

/**************************************************************************//**
\brief Writes data to the user's area of NV memory.

\ingroup pds

\param[in] offset   - offset to place data in user's area
\param[in] data     - pointer to data
\param[in] length   - data length
\param[in] callback - pointer to a callback function; if callback is NULL,
                      then data will be written syncronously

\return - PDS state as a result of data writing operation
******************************************************************************/
PDS_DataServerState_t PDS_WriteUserData(uint16_t offset, uint8_t *data,
                                        uint16_t length, void (*callback)(void));


#endif  //#ifndef _PERSISTDATASERVER_H

