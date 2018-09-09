/**
* \file  wlPdsTaskManager.h
*
* \brief PDS task manager interface.
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

#ifndef _WLPDSTASKMANAGER_H_
#define _WLPDSTASKMANAGER_H_
/******************************************************************************
                    Includes section
******************************************************************************/

#include <compiler.h>

/******************************************************************************
                    Types section
******************************************************************************/
/* PDS task identifiers */
typedef enum _PdsTaskId_t
{
  PDS_STORE_ITEM_TASK_ID,
  PDS_TASKS_AMOUNT
} PdsTaskId_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
\brief Posts PDS task

\param[in] taskId - id of a task
******************************************************************************/
void pdsPostTask(PdsTaskId_t taskId);

void PDS_TaskHandler (void);

#endif /* _WLPDSTASKMANAGER_H_ */
/* eof wlPdsTaskManager.h */

