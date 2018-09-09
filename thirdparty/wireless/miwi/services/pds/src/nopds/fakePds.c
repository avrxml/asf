/**
* \file  fakePds.c
*
* \brief Fake PDS implementation.
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

#ifndef ENABLE_NETWORK_FREEZER
/******************************************************************************
                               Includes section
******************************************************************************/
#include <pdsDataServer.h>

/******************************************************************************
                   Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Persistent Data Server initialization routine.
******************************************************************************/
void PDS_Init(void)
{}

/******************************************************************************
\brief PDS item initialization routine
******************************************************************************/
void PDS_InitItems(void)
{}

/******************************************************************************
\brief Erases item under security control
******************************************************************************/
void PDS_EraseSecureItems(void)
{}

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
bool PDS_Restore(PDS_MemId_t memoryId)
{
  (void)memoryId;
  return false;
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
  (void)memoryId;
  return true;
}

/**************************************************************************//**
\brief Stores data in non-volatile memory in a synchronous way -
       the application execution will be blocked until the process is completed.

\ingroup pds

All PDS files which are absent in the current build configuration will be ignored.

\param[in] memoryId - an identifier of PDS file or directory to be stored
                      in non-volatile memory.

\return True, if storing process has been performed successfully, false - otherwise.
******************************************************************************/
bool PDS_BlockingStore(PDS_MemId_t memoryId)
{
  (void)memoryId;
  return true;
}

/**************************************************************************//**
\brief Provides PDS with information about memory that should be kept up-to-date
       in the non-volatile storage.

\ingroup pds

Data will be saved im non-volatile memory upon specific events.

\param[in] memoryId - an identifier of PDS file or directory to be stored
                      in non-volatile memory.
******************************************************************************/
void PDS_StoreByEvents(PDS_MemId_t memoryId)
{
  (void)memoryId;
}

/**************************************************************************//**
\brief Enables periodic commitment of specified persistent items to a
       non-volatile storage.

\ingroup pds

\param[in] interval - period of time between successive commitments
\param[in] memoryId - an identifier of PDS file or directory to be stored
                      in non-volatile memory.
******************************************************************************/
void PDS_StoreByTimer(uint32_t interval, PDS_MemId_t memoryId)
{
  (void)memoryId;
  (void)interval;
}

/*****************************************************************************//**
\brief Removes specified file records from NV Storage

\ingroup pds

\param[in] memoryId - an identifier of PDS file or directory to be removed
                      from NV memory.

\return - PDS state as an operation result.
******************************************************************************/
PDS_DataServerState_t PDS_Delete(PDS_MemId_t memoryId)
{
  (void)memoryId;
  return 0;
}

/**************************************************************************//**
\brief Deletes data from non-volatile storage except the Persistant items
       depending on the parameter passed.

\param[in] includingPersistentItems - deletes persistant items if TRUE
                                      deletes all other items except persistant 
                                      items if FALSE

\return bool - status of PDS DeleteAll
******************************************************************************/
bool PDS_DeleteAll(bool includingPersistentItems)
{
  (void)includingPersistentItems;
  return 0;
}

/**************************************************************************//**
\brief Continue Persistent Data Server operation the same way as before stopping

\ingroup pds
******************************************************************************/
void PDS_Run(void)
{}

/**************************************************************************//**
\brief Stops the Persistent Data Server

\ingroup pds

This doesn't affect a policy created through PDS_StoreByEvent() of
PDS_StoreByTimer() functions. After calling the PDS_Run() function PDS
continues working as before the stop.
******************************************************************************/
void PDS_Stop(void)
{}

/**************************************************************************//**
\brief Checks if the specified PDS file or directory can be restored
       from non-volatile memory

\ingroup pds

PDS files not included in the current build configuration will be ignored.

\param[in] memoryId - an identifier of PDS file or directory to be checked.

\return true, if the specified memory can be restored; false - otherwise.
******************************************************************************/
bool PDS_IsAbleToRestore(PDS_MemId_t memoryId)
{
  (void)memoryId;
  return false;
}

/**************************************************************************//**
\brief Checks if the specified PDS file or directory can be stored
       in non-volatile memory

\ingroup pds

\param[in] memoryId - an identifier of PDS file or directory to be checked.

\return true, if the specified memory can be stored; false - otherwise.
******************************************************************************/
bool PDS_IsAbleToStore(PDS_MemId_t memoryId)
{
  (void)memoryId;
  return false;
}

/**************************************************************************//**
\brief Checks if files specified have been restored from non-volatile memory
       during the latest restoring procedure

\ingroup pds

\param[in] memoryId - an identifier of PDS file or directory to be checked.

\return true, if the specified file(s) have been restored; false - otherwise.
******************************************************************************/
bool PDS_IsMemoryRestored(PDS_MemId_t memoryId)
{
  (void)memoryId;
  return false;
}

/**************************************************************************//**
\brief Marks the specified restored PDS files to be cleared during a reset phase
      (during ZDO_StartNetwork() execution)

\ingroup pds

\param[in] memoryId - an identifier of PDS file or directory to be cleared.
******************************************************************************/
void PDS_ClearRestoredMemory(PDS_MemId_t memoryId)
{
  (void)memoryId;
}

/******************************************************************************
\brief This function is deprecated - use PDS_BlockingStore() instead
******************************************************************************/
void PDS_FlushData(PDS_MemId_t memoryIds)
{
  (void)memoryIds;
}

/******************************************************************************
\brief This function is deprecated - use PDS_Delete() instead.
******************************************************************************/
PDS_DataServerState_t PDS_ResetStorage(PDS_MemId_t memoryId)
{
  (void)memoryId;
  return 0;
}

/******************************************************************************
\brief Checks, if PDS is busy with file writing, or not.

\return - True, if PDS is busy with file writing; false - otherwise
******************************************************************************/
bool PDS_IsBusy(void)
{
  return false;
}

#endif // _ENABLE_PERSISTENT_SERVER_
// eof fakePds.c