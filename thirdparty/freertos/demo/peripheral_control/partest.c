/*
    FreeRTOS V7.1.1 - Copyright (C) 2012 Real Time Engineers Ltd.


    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?                                      *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************


    http://www.FreeRTOS.org - Documentation, training, latest information,
    license and contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool.

    Real Time Engineers ltd license FreeRTOS to High Integrity Systems, who sell
    the code with commercial support, indemnification, and middleware, under
    the OpenRTOS brand: http://www.OpenRTOS.com.  High Integrity Systems also
    provide a safety engineered and independently SIL3 certified version under
    the SafeRTOS brand: http://www.SafeRTOS.com.
*/

/*-----------------------------------------------------------
 * Simple IO routines to control the LEDs.
 *-----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo includes. */
#include "partest.h"
#include "conf_example.h"

/* Library includes. */
#include <board.h>
#include <ioport.h>

/* The number of LEDs available to the user on the evaluation kit. */
#ifndef partestNUM_LEDS
#define partestNUM_LEDS                 (3UL)
#endif

/* The index of the pins to which the LEDs are connected.  This file is used
with many different evaluation kits, so the order may not match the order on
the evaluation kit hardware. */
static const uint32_t ulLED[] = {LED0_GPIO, LED1_GPIO, LED2_GPIO};

/* Relevant active states. */
static const signed portBASE_TYPE xActiveStates[]
	= {LED0_ACTIVE_LEVEL, LED1_ACTIVE_LEVEL, LED2_ACTIVE_LEVEL};

/*-----------------------------------------------------------*/

void vParTestInitialise(void)
{
	unsigned long ul;

	for (ul = 0; ul < partestNUM_LEDS; ul++) {
		/* Configure the LED, before ensuring it starts in the off
		state. */
		ioport_set_pin_dir(ulLED[ ul ], IOPORT_DIR_OUTPUT);
        ioport_set_pin_level(ulLED[ ul ], IOPORT_PIN_LEVEL_HIGH);
		vParTestSetLED(ul, xActiveStates[ ul ]);
	}
}

/*-----------------------------------------------------------*/

void vParTestSetLED(unsigned portBASE_TYPE uxLED, signed portBASE_TYPE xValue)
{
	if (uxLED < partestNUM_LEDS) {
		if (xValue != pdFALSE) {
			/* Turn the LED on. */
			taskENTER_CRITICAL();
			{
				if (xActiveStates[ uxLED ] == 0) {
					ioport_set_pin_level(ulLED[ uxLED ], IOPORT_PIN_LEVEL_LOW);
				} else {
					ioport_set_pin_level(ulLED[ uxLED ], IOPORT_PIN_LEVEL_HIGH);
				}
			}
			taskEXIT_CRITICAL();
		} else {
			/* Turn the LED off. */
			taskENTER_CRITICAL();
			{
				if (xActiveStates[ uxLED ] == 0) {
					ioport_set_pin_level(ulLED[ uxLED ], IOPORT_PIN_LEVEL_HIGH);
				} else {
					ioport_set_pin_level(ulLED[ uxLED ], IOPORT_PIN_LEVEL_LOW);
				}
			}
			taskEXIT_CRITICAL();
		}
	}
}

/*-----------------------------------------------------------*/

void vParTestToggleLED(unsigned portBASE_TYPE uxLED)
{
	if (uxLED < partestNUM_LEDS) {
		taskENTER_CRITICAL();
		{
			ioport_toggle_pin_level(ulLED[ uxLED ]);
		}
		taskEXIT_CRITICAL();
	}
}
