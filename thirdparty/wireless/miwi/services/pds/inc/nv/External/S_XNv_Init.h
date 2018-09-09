/**
* \file  S_XNv_Init.h
*
* \brief S_XNv Initializer Driver Interface
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

#ifndef S_XNV_INIT_H
#define S_XNV_INIT_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Task.h"
#include "N_Types.h"

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/


/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/


/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Initializes the external flash service for early access.
    \note It is not required to call this function if the application does not require early access
    to external Nv. Note that the S_XNv component is fully functional after calling S_XNv_EarlyInit(),
    but until S_XNv_Init() is called, chances are much higher that the component blocks when a sector
    becomes full. Only after S_XNv_Init() is called, compacting the sector is decoupled as much as
    possible.
*/
void S_XNv_EarlyInit(void);

/** Initializes the rest of the component (timers and task). Note that N_Task and N_Timer must be
    initialised first.
*/
void S_XNv_Init(void);

/** The event handler for the component's task.
*/
bool S_XNv_EventHandler(N_Task_Event_t evt);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // S_XNV_INIT_H
