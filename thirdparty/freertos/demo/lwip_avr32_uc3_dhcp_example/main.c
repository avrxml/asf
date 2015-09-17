/*****************************************************************************
 *
 * \file
 *
 * \brief FreeRTOS and lwIP example for AVR32 UC3.
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


 /*! \mainpage
 * \section intro Introduction
 * This documents data structures, functions, variables, defines, enums, and
 * typedefs in the software for the lwIP basic two-in-one web server and TFTP server demo (with DHCP) example.
 *
 * The given example is an example using freeRTOS, the current lwIP stack and MACB driver.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/uc3/">Atmel AVR UC3</A>.\n
 */

/* Environment include files. */
#include <stdlib.h>
#include <string.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo file headers. */
#include "partest.h"
#include "ethernet.h"
#include "netif/etharp.h"
#include "flash.h"
#include "displayTask.h"
#include "sysclk.h"

/* Priority definitions for most of the tasks in the demo application. */
#define mainLED_TASK_PRIORITY     ( tskIDLE_PRIORITY + 1 )
#define mainETH_TASK_PRIORITY     ( tskIDLE_PRIORITY + 1 )
#define display_TASK_PRIORITY     ( tskIDLE_PRIORITY + 1 )

/* Baud rate used by the serial port tasks. */
#define mainCOM_BAUD_RATE      ( ( unsigned portLONG ) 57600 )
#define mainCOM_BUFFER_LEN     ( ( unsigned portLONG ) 70 )

/* An address in the internal Flash used to count resets.  This is used to check that
the demo application is not unexpectedly resetting. */
#define mainRESET_COUNT_ADDRESS     ( ( void * ) 0xC0000000 )

/**
 * \fn     main
 * \brief  start the software here
 *         1) Configure system clocks.
 *         2) Setup the GPIO in output for the board's LED.
 *         3) Start Display task.
 *         4) Start the LED flash tasks just to provide visual feedback that the
 *         demo is executing.
 *         5) Start the ethernet tasks.
 *         6) Start FreeRTOS.
 * \return 0, which should never occur.
 *
 */
int main( void )
{
	// 1) Configure system clocks.
	sysclk_init();

	// 2) Setup the GPIO in output for the board's LED.
	vParTestInitialise();

	// 3) Start Display task.
	vDisplay_Start(display_TASK_PRIORITY);

	// 4) Start the LED flash tasks just to provide visual feedback that the
	// demo is executing.
	vStartLEDFlashTasks( mainLED_TASK_PRIORITY );

	// 5) Start the ethernet tasks.
	vStartEthernetTaskLauncher( configMAX_PRIORITIES );

	// 6) Start FreeRTOS.
	vTaskStartScheduler();


	/* Will only reach here if there was insufficient memory to create the idle task. */

	return 0;
}
/*-----------------------------------------------------------*/
