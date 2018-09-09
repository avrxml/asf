/*****************************************************************************
 *
 * \file
 *
 * \brief Display task
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
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
