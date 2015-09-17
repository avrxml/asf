/*****************************************************************************
*
* \file
*
* \brief FreeRTOS Led Driver example for SAM.
*
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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
#include "FreeRTOS.h"
#include "task.h"
#include "partest.h"
#include "led.h"
#include "conf_board.h"

void vParTestInitialise( void )
{
	/* Start with all LEDs off. */
	LED_Off(LED0);
	LED_Off(LED1);
	LED_Off(LED2);
}

void vParTestSetLED( unsigned portBASE_TYPE uxLED, signed portBASE_TYPE xValue )
{
	vTaskSuspendAll();

	if (xValue == pdTRUE) {
	switch (uxLED) {
		case 0:
			LED_On(LED0);
		break;

		case 1:
			LED_On(LED1);
		break;

		case 2:
			LED_On(LED2);
		break;

		default :
		break;
	}
	} else {
	switch (uxLED) {
		case 0:
			LED_Off(LED0);
		break;

		case 1:
			LED_Off(LED1);
		break;

		case 2:
			LED_Off(LED2);
		break;

		default :
		break;
	}
	}

	xTaskResumeAll();
}

void vParTestToggleLED( unsigned portBASE_TYPE uxLED )
{
	vTaskSuspendAll();

	switch (uxLED) {
		case 0:
			LED_Toggle(LED0);
		break;

		case 1:
			LED_Toggle(LED1);
		break;

		case 2:
			LED_Toggle(LED2);
		break;

		default :
		break;
	}

	xTaskResumeAll();
}
