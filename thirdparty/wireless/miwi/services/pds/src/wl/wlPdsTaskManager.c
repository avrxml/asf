/**
* \file  wlPdsTaskManager.c
*
* \brief PDS task manager implementation.
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

#include <wlPdsTaskManager.h>

/******************************************************************************
                    Types section
******************************************************************************/
typedef uint8_t PdsTaskBitMask_t;
typedef void (*PdsTaskHandler_t)(void);

/******************************************************************************
                    Prototypes section
******************************************************************************/
void pdsStoreItemTaskHandler(void);

/******************************************************************************
                    Static variables section
******************************************************************************/
static PdsTaskHandler_t pdsTaskHandlers[PDS_TASKS_AMOUNT] =
{
  [PDS_STORE_ITEM_TASK_ID] = pdsStoreItemTaskHandler,
};

static PdsTaskBitMask_t pendingTasks;

/******************************************************************************
                   Implementation section
******************************************************************************/
/******************************************************************************
\brief Posts PDS task

\param[in] taskId - id of a task
******************************************************************************/

void pdsPostTask(PdsTaskId_t taskId)
{
  pendingTasks |= (1U << taskId);
//  SYS_PostTask(PDS_TASK_ID);
}

/******************************************************************************
\brief General PDS task handler
******************************************************************************/
void PDS_TaskHandler(void)
{
  if (pendingTasks)
  {
    uint8_t taskId = 0U;

    while (!(pendingTasks & (1U << taskId)))
      taskId++;

    pendingTasks ^= (1U << taskId);
    pdsTaskHandlers[taskId]();
   
  }
}

#endif

/* eof wlPdsTaskManager.c */

