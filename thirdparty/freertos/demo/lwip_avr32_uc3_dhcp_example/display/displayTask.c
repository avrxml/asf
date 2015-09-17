/*****************************************************************************
 *
 * \file
 *
 * \brief Display task
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 *****************************************************************************/


#include <string.h>

// Scheduler includes.
#include "FreeRTOS.h"
#include "task.h"
#include "displayTask.h"

// Demo includes.
#include "portmacro.h"
#include "partest.h"
#include "intc.h"
#include "display.h"

#define DISPLAY_STACK_SIZE	        ( configMINIMAL_STACK_SIZE  )

//! The supervisor queue length.
#define DISPLAY_QUEUE_SIZE   1

/*! pointer to the memory image of the User Mess line */
portCHAR Line[20];

/*! pointer to the memory image of the User Menu line */
portCHAR * UserMessScreen = Line;

xQueueHandle xDISPLAYQueue = 0;

void vDisplay_Start( unsigned portBASE_TYPE uxPriority )
{
   /* Spawn the Sentinel task. */
   xTaskCreate( vDisplay, ( const signed portCHAR * )"DISPLAY",
                DISPLAY_STACK_SIZE, NULL, uxPriority, ( xTaskHandle * )NULL );

   // Create the supervisor queue to deal with MMI actions
   xDISPLAYQueue = xQueueCreate( DISPLAY_QUEUE_SIZE, sizeof(bool *) );
   if( 0 == xDISPLAYQueue )
   {
      // not able to creeat the Display Queue.
      while( 1 );
   }
}

//! Basic SMTP client task definition
portTASK_FUNCTION( vDisplay , pvParameters )
{
  char *temp_string;

  display_init(FOSC0 * 4);
  for (;;)
  {
    /* 1) Suspend ourselves. */
   // vTaskSuspend( NULL ); // TO KEEP
    // get queue information
      if ( pdTRUE == xQueueReceive( xDISPLAYQueue, &temp_string, ( portTickType ) 0 ) )
      {
        /* 2) Perform the action. */
        if (temp_string != NULL)
        {
          display_print((const char*)temp_string);
        }
      }
      vTaskDelay(100);
  }
}

void sendMessage(char *mess)
{
  // Add the refresh need to the xMMIQueue.
  strncpy(UserMessScreen,mess,20);
  xQueueSend( xDISPLAYQueue, ( void * ) &UserMessScreen, 0 );
}
