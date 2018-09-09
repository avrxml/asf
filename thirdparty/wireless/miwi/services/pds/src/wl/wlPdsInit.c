/**
* \file  wlpdsInit.c
*
* \brief Persistent Data Server initialization module
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
#include <S_Nv_Init.h>
#include <S_Nv.h>
#include "assert.h"
#include <D_Nv_Init.h>
#include <wlPdsTypesConverter.h>
#include <wlPdsMemIds.h>
//#include <flash.h>
#include <pdsDataServer.h>

/******************************************************************************
                   Definitions section
******************************************************************************/
#define COMPID "pwlPdsInit"

/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
\brief Persistent Data Server initialization routine.
******************************************************************************/


void PDS_Init (void)
{
  D_Nv_Init();
  S_Nv_Init();
}

/******************************************************************************
\brief PDS item initialization routine
******************************************************************************/
void PDS_InitItems(void)
{
  S_Nv_ReturnValue_t ret;
  ItemIdToMemoryMapping_t itemDesc;

  for (S_Nv_ItemId_t i = 1U; i < PDS_ITEM_AMOUNT; i++)
  {
    if (pdsGetItemDescr(i, &itemDesc))
    {
      assert(itemDesc.itemSize);

#ifdef PDS_SECURITY_CONTROL_ENABLE
      if (pdsIsItemUnderSecurityControl(i))
      {
        pdsSecureItemInit(i,itemDesc.itemSize);
      }
      else
#endif
      {
        ret = S_Nv_ItemInit(i, itemDesc.itemSize, NULL);
        assert((S_Nv_ReturnValue_DidNotExist == ret) || (S_Nv_ReturnValue_Ok == ret));
      }
    }
  }
}

#endif 

/* eof pdsInit.c */

