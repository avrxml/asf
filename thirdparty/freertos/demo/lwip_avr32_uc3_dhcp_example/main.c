/*****************************************************************************
 *
 * \file
 *
 * \brief FreeRTOS and lwIP example for AVR32 UC3.
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
 * <A href="http://www.microchip.com/uc3/">Atmel AVR UC3</A>.\n
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
