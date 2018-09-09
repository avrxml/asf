/**
* \file  miwi_nvm.h
*
* \brief MiWi NVM Configuration interface
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
/*
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
*/

#ifndef __MIWI_NVM_H
#define __MIWI_NVM_H

#include "compiler.h"
#include "common_nvm.h"

#if defined(ENABLE_NVM)
#include "miwi_config.h"
#include "miwi_api.h"

COMPILER_PACK_SET(1)
typedef struct nvmParameters{
	API_UINT32_UNION    nvmOutFrameCounter;
	API_UINT16_UNION    nvmMyPANID;
	uint8_t             nvmCurrentChannel;
	uint8_t             nvmConnMode;
	CONNECTION_ENTRY    nvmConnectionTable[CONNECTION_SIZE];
	uint8_t             nvmEdc;
#if defined (PROTOCOL_STAR)
	uint8_t             nvmRole;
	uint8_t             nvmMyIndex;
#endif
}nvmParameters_t;
COMPILER_PACK_RESET()

extern nvmParameters_t *nvmParam;

#define START_ADDR (((FLASH_ADDR + FLASH_SIZE - 1) - sizeof(nvmParameters_t)) & 0xFFFFFFC0)

#define CONNECTION_ELEMENT_SIZE (MY_ADDRESS_LENGTH + ADDITIONAL_NODE_ID_SIZE + 1)


#define nvmGetMyPANID( x )                  nvm_read(INT_FLASH, &(nvmParam->nvmMyPANID), (uint8_t *)x, 2)
#define nvmPutMyPANID( x )                  nvm_write(INT_FLASH, &(nvmParam->nvmMyPANID), (uint8_t *)x, 2)

#define nvmGetCurrentChannel( x )           nvm_read(INT_FLASH, &(nvmParam->nvmCurrentChannel), (uint8_t *)x, 1)
#define nvmPutCurrentChannel( x )           nvm_write(INT_FLASH, &(nvmParam->nvmCurrentChannel), (uint8_t *)x, 1)

#define nvmGetConnMode( x )                 nvm_read(INT_FLASH, &(nvmParam->nvmConnMode), (uint8_t *)x, 1)
#define nvmPutConnMode( x )                 nvm_write(INT_FLASH, &(nvmParam->nvmConnMode), (uint8_t *)x, 1)

#define nvmGetConnectionTable( x )          nvm_read(INT_FLASH, &(nvmParam->nvmConnectionTable), (uint8_t *)x, (uint16_t)CONNECTION_SIZE * sizeof(CONNECTION_ENTRY))
#define nvmPutConnectionTable( x )          nvm_write(INT_FLASH, &(nvmParam->nvmConnectionTable), (uint8_t *)x, (uint16_t)CONNECTION_SIZE * sizeof(CONNECTION_ENTRY))
#define nvmPutConnectionTableIndex(x, y)    nvm_write(INT_FLASH, &(nvmParam->nvmConnectionTable[y]), (uint8_t *)x, sizeof(CONNECTION_ENTRY))

#define nvmGetOutFrameCounter( x )          nvm_read(INT_FLASH, &(nvmParam->nvmOutFrameCounter), (uint8_t *)x, 4)
#define nvmPutOutFrameCounter( x )          nvm_write(INT_FLASH, &(nvmParam->nvmOutFrameCounter), (uint8_t *)x, 4);

#define nvmGetMyDC(x)                       nvm_read(INT_FLASH, &(nvmParam->nvmEdc), (uint8_t *)x, 1)
#define nvmPutMyDC(x)                       nvm_write(INT_FLASH, &(nvmParam->nvmEdc), (uint8_t *)x, 1)

#if defined (PROTOCOL_STAR)

#define nvmGetMyRole(x)       nvm_read(INT_FLASH, &(nvmParam->nvmRole), (uint8_t *)x, 1)
#define nvmPutMyRole(x)       nvm_write(INT_FLASH, &(nvmParam->nvmRole), (uint8_t *)x, 1)

#define nvmGetMyIndex(x)      nvm_read(INT_FLASH, &(nvmParam->nvmMyIndex), (uint8_t *)x, 1)
#define nvmPutMyIndex(x)      nvm_write(INT_FLASH, &(nvmParam->nvmMyIndex), (uint8_t *)x, 1)

#endif //defined (PROTOCOL_STAR)

#endif
#endif
