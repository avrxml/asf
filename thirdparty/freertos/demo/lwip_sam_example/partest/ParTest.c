/*****************************************************************************
*
* \file
*
* \brief FreeRTOS Led Driver example for SAM.
*
 * Copyright (c) 2012-2018 Microchip Technology Inc. and its subsidiaries.
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
