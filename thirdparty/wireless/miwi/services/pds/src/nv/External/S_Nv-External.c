/**
* \file  S_Nv-External.c
*
* \brief S_Nv External Flash Driver implementation
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

#include "S_Nv_Bindings.h"

#include "S_Nv_Init.h"
#include "S_Nv_Stack_Ids.h"
#include "S_Nv.h"
#include "pdsDataServer.h"

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/

#include "N_ErrH.h"
#include "N_Log.h"
#include "N_Util.h"
#include "N_Types.h"

#include "S_XNv_Bindings.h"
#include "S_XNv.h"

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#define COMPID "S_Nv-External"

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

S_Nv_ReturnValue_t S_Nv_ItemInit_Impl(S_Nv_ItemId_t id, uint16_t itemLength, void* pDefaultData)
{
    return (S_Nv_ReturnValue_t)S_XNv_ItemInit((S_XNv_ItemId_t)id, itemLength, pDefaultData);
}

S_Nv_ReturnValue_t S_Nv_Read_Impl(S_Nv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
{
    return (S_Nv_ReturnValue_t)S_XNv_Read((S_XNv_ItemId_t)id, offset, dataLength, pData);
}

S_Nv_ReturnValue_t S_Nv_Write_Impl(S_Nv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
{
    return (S_Nv_ReturnValue_t)S_XNv_Write((S_XNv_ItemId_t)id, offset, dataLength, pData);
}

S_Nv_ReturnValue_t S_Nv_Delete_Impl(S_Nv_ItemId_t id)
{
    return (S_Nv_ReturnValue_t)S_XNv_Delete((S_XNv_ItemId_t)id);
}

uint16_t S_Nv_ItemLength_Impl(S_Nv_ItemId_t id)
{
    return S_XNv_ItemLength((S_XNv_ItemId_t)id);
}

S_Nv_ReturnValue_t S_Nv_EraseAll_Impl(bool includingPersistentItems)
{
    PDS_EraseSecureItems();
    return (S_Nv_ReturnValue_t)S_XNv_EraseAll(includingPersistentItems);
}

void S_Nv_SetPowerSupplyCheckingFunction_Impl(S_Nv_PowerSupplyCheckingFunction_t pf)
{
    S_XNv_SetPowerSupplyCheckingFunction(pf);
}

bool S_Nv_IsItemAvailable_Impl(S_Nv_ItemId_t id)
{
  return S_XNv_IsItemAvailable((S_XNv_ItemId_t)id);
}

void S_Nv_Init(void)
{
}
#endif //#ifdef PDS_USE_EXTERNAL_FLASH
#endif 