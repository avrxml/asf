/**
 * \file
 *
 * \brief Example application for ATMEL PRIME v1.3 Base Node
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
 */

/**
 *  \mainpage ATMEL PRIME v1.3 Base Node Example
 *
 *  \section Purpose
 *
 *  This application shows how to use the PRIME stack in a Base Node on PLC
 * boards.
 *
 *  \section Requirements
 *
 *  This package should be used with SAM4CP16BMB board, on which there is
 * dedicated PLC hardware.
 *
 *  \section Description
 *
 *  This application will configure the PRIME stack and its serial interface to
 * use PHY, MAC and IEC_432
 *  layers as Base Node. It will also configure the serial interface to
 * communicate with ATMEL PLC tools.
 *
 *  \section Usage
 *
 *  -# Build the program and download it into the evaluation board. Please
 *     refer to the
 *     <a href="http://www.atmel.com/dyn/resources/prod_documents/6421B.pdf">
 *     SAM-BA User Guide</a> or the
 *     <a href="http://www.atmel.com/dyn/resources/prod_documents/doc6310.pdf">
 *     GNU-Based Software Development</a>
 *     application note depending on the solutions that users choose.
 *     \endcode
 *  -# The application will start PRIME standard as Base Node mode.
 *	-# The application is configured to serialize several protocols with
 * these settings:
 *    - Uart0 Serial port
 *	  - 115200 bauds
 *    - TX buffer: 1024 bytes, Rx buffer: 1024 bytes
 *    - Serialized protocols in use:
 *              - Prime Management
 *              - Prime MLME
 *              - Prime PLME
 *              - Prime IEC-432
 *
 */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Atmel boards includes. */
#include "board.h"
#include "conf_board.h"

/* Atmel library includes. */
#include "asf.h"

/* Example config */
#include "conf_example.h"

#define STRING_EOL    "\r"
#define STRING_HEADER "-- ATMEL Prime Base Node Serialized Application --\r\n" \
	"-- "BOARD_NAME " --\r\n" \
	"-- Compiled: "__DATE__ " "__TIME__ " --"STRING_EOL

/* FreeRTOS utils */
void vApplicationIdleHook( void );
void vApplicationMallocFailedHook( void );
void vApplicationStackOverflowHook( xTaskHandle pxTask,
		signed char *pcTaskName );
void vApplicationTickHook( void );

/*-----------------------------------------------------------*/
void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	 * configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is
	 * a hook function that will get called if a call to pvPortMalloc() fails.
     * pvPortMalloc() is called internally by the kernel whenever a task,
	 * queue,timer or semaphore is created.  It is also called by various parts of
	 * the demo application.  If heap_1.c or heap_2.c are used, then the size of
	 * the heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE
	 * in FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be
	 * used to query the size of free heap space that remains (although it does
	 * not provide information on how the remaining heap might be fragmented).
	 * */
	taskDISABLE_INTERRUPTS();
	for (;;) {
		while (1) {
		}
	}
}

/*-----------------------------------------------------------*/
void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is
	 * set to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the
	 * idle task.  It is essential that code added to this hook function never
	 * attempts to block in any way (for example, call xQueueReceive() with a block
	 * time specified, or call vTaskDelay()).  If the application makes use of
	 * the vTaskDelete() API function (as this demo application does) then it is
	 * also important that vApplicationIdleHook() is permitted to return to its
	 * calling function, because it is the responsibility of the idle task to clean
	 * up memory allocated by the kernel to any task that has since been
	 * deleted. */
}

/*-----------------------------------------------------------*/
void vApplicationStackOverflowHook( xTaskHandle pxTask,
		signed char *pcTaskName )
{
	(void)pcTaskName;
	(void)pxTask;

	/* Run time stack overflow checking is performed if
	 * configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	 * function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for (;;) {
		while (1) {
		}
	}
}

/*-----------------------------------------------------------*/
void vApplicationTickHook( void )
{
	/* This function will be called by each tick interrupt if
	* configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can
	* be added here, but the tick hook is called from an interrupt context, so
	* code must not attempt to block, and only the interrupt safe FreeRTOS
	* API functions can be used (those that end in FromISR()). */
}

/**
 * \brief Configure the hardware.
 */
static void prvSetupHardware(void)
{
#ifdef CONF_BOARD_LCD_EN
	status_code_t status;
#endif

	/* ASF function to setup clocking. */
	sysclk_init();

	/* Ensure all priority bits are assigned as preemption priority bits. */
	NVIC_SetPriorityGrouping(__NVIC_PRIO_BITS);

	/* Atmel library function to setup for the evaluation kit being used. */
	board_init();

	/* PLC HAL service initialization */
	hal_init();
	hal_start();

#ifdef CONF_BOARD_LCD_EN
	/* Initialize the C42364A LCD glass component. */
	status = c42364a_init();
	if (status != STATUS_OK) {
		puts("-- LCD Initialization fails! --\r\n");
		while (1) {
		}
	}

	c42364a_set_contrast(15);
	c42364a_clear_all();
	c42364a_show_icon(C42364A_ICON_ATMEL);
	c42364a_show_icon(C42364A_ICON_WLESS);
	c42364a_show_text((const uint8_t *)"BASE  ");
#endif
}

/**
 * \brief Display scheduler activity led.
 */
static void prvProcessMonitorTasks(xTimerHandle pxTimer)
{
	UNUSED(pxTimer);

#if (BOARD == SAM4CMP_DB || BOARD == SAM4CMS_DB)
	LED_Toggle(LED4);
#else
	LED_Toggle(LED0);
#endif
}

/**
 *  Configure UART console.
 */
/* [main_console_configure] */
static void configure_dbg_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONF_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * \brief Main code entry point.
 */
int main( void )
{
	xTimerHandle xMonitorTimer;

	/* Prepare the hardware */
	prvSetupHardware();

	/* Init Prime Stack OSS */
	vPrimeStackInitTask();

	/* Configure console */
	configure_dbg_console();
	puts(STRING_HEADER);

	/* Create timer to monitor tasks execution */
	xMonitorTimer = xTimerCreate(
			(const signed char *const)"Monitor timer", /* Text name for
	                                                    * debugging. **/
			MONITOR_TIMER_RATE, /* The timer period. */
			pdTRUE, /* This is an auto-reload timer, so xAutoReload
	                         * is set to pdTRUE.*/
			NULL, /* The timer does not use its ID, so the ID is
	                       * just set to NULL. */
			prvProcessMonitorTasks /* Function called each time the
	                                        * timer expires. */
			);
	configASSERT(xMonitorTimer);
	xTimerStart(xMonitorTimer, MONITOR_BLOCK_TIME);

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following
	 * line will never be reached. If the following line does execute, then
	 * there was insufficient FreeRTOS heap memory available for the idle
	 * and/or
	 * timer tasks to be created. See the memory management section on the
	 * FreeRTOS web site for more details. */
	for (;;) {
	}
}
