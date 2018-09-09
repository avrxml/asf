/**
* \file  mimem.h
*
* \brief Memory Buffer handling for MiWi Protocol interface
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

#ifndef __MIMEM_H_
#define __MIMEM_H_

/************************ HEADERS **********************************/


/************************ DEFINITIONS ******************************/
#if defined(PROTOCOL_MESH)
#if defined(PAN_COORDINATOR)
#define NUMBER_OF_MIMEM_BUFFERS  40
#elif defined(COORDINATOR)
#define NUMBER_OF_MIMEM_BUFFERS  30
#elif defined(ENDDEVICE)
#define NUMBER_OF_MIMEM_BUFFERS  20
#endif
#else
#define NUMBER_OF_MIMEM_BUFFERS  10
#endif
/************************ DATA TYPES *******************************/

/************************ Prototypes ********************************/
void MiMem_Init(void);
uint8_t* MiMem_Alloc(uint8_t size);
void MiMem_Free(void* buffPtr);
uint8_t MiMem_PercentageOfFreeBuffers(void);
#endif

