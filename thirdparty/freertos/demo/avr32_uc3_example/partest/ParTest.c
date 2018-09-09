/*****************************************************************************
 *
 * \file
 *
 * \brief FreeRTOS LEDs Management for AVR32 UC3.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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



#include <avr32/io.h>
#include "FreeRTOS.h"
#include "task.h"
#include "partest.h"


/*-----------------------------------------------------------
 * Simple parallel port IO routines.
 *-----------------------------------------------------------*/

#define partstALL_OUTPUTS_OFF     ( ( unsigned portCHAR ) 0x00 )
#define partstMAX_OUTPUT_LED      ( ( unsigned portCHAR ) LED_COUNT )

static volatile unsigned portCHAR ucCurrentOutputValue = partstALL_OUTPUTS_OFF; /*lint !e956 File scope parameters okay here. */

/*-----------------------------------------------------------*/

void vParTestInitialise( void )
{
	LED_Display( partstALL_OUTPUTS_OFF ); /* Start with all LEDs off. */
}
/*-----------------------------------------------------------*/

void vParTestSetLED( unsigned portBASE_TYPE uxLED, signed portBASE_TYPE xValue )
{
unsigned portCHAR ucBit;

	if( uxLED >= partstMAX_OUTPUT_LED )
	{
		return;
	}

	ucBit = ( ( unsigned portCHAR ) 1 ) << uxLED;

	vTaskSuspendAll();
	{
		if( xValue == pdTRUE )
		{
			ucCurrentOutputValue |= ucBit;
		}
		else
		{
			ucCurrentOutputValue &= ~ucBit;
		}

		LED_Display(ucCurrentOutputValue);
	}
	xTaskResumeAll();
}
/*-----------------------------------------------------------*/

void vParTestToggleLED( unsigned portBASE_TYPE uxLED )
{
unsigned portCHAR ucBit;

	  if( uxLED >= partstMAX_OUTPUT_LED )
	  {
	return;
	}

	ucBit = ( ( unsigned portCHAR ) 1 ) << uxLED;

	vTaskSuspendAll();
	{
		ucCurrentOutputValue ^= ucBit;
		LED_Display(ucCurrentOutputValue);
	}
	xTaskResumeAll();
}
