/**
* \file  S_Nv_init.h
*
* \brief NV component header
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

#ifndef S_NV_INIT_H
#define S_NV_INIT_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/


#include <compiler.h>

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

/** Initializes the internal flash service for early access.
    \note It is not required to call this function if the application does not require early access
    to internal Nv. Note that the S_Nv component is fully functional after calling S_Nv_EarlyInit(),
    but until S_Nv_Init() is called, chances are much higher that the component blocks when a sector
    becomes full. Only after S_Nv_Init() is called, compacting the sector is decoupled as much as
    possible.
*/
void S_Nv_EarlyInit(void);

/** Initializes the component and tells which items are persistent against S_Nv_EraseAll(FALSE).
*/
void S_Nv_Init(void);

/** The event handler for the component's task.
*/
// bool S_Nv_EventHandler(N_Task_Event_t evt);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // S_NV_INIT_H
