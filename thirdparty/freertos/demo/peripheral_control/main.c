/**
 *
 * \file
 *
 * \brief Example that demonstrates FreeRTOS peripheral control functions
 * and FreeRTOS+CLI
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
 */

/**
 * \mainpage FreeRTOS peripheral control functions and FreeRTOS+CLI Example
 * \section intro Introduction
 * ************* Introduction **************************************************
 *
 * The examples defined in this project demonstrate:
 *
 *	+ The blocking FreeRTOS UART API being used with FreeRTOS+CLI to create a
 *	  command console.
 *
 *	+ The fully asynchronous FreeRTOS USART API being used to send a string to
 *	  an RS232 echo server, and receive the reply.  A loopback connector can be
 *    used in place of an echo server.
 *
 *	+ The blocking FreeRTOS USART API being used with FreeRTOS+CLI to create a
 *	  command console.
 *
 *	+ The standard ASF USB CDC driver being used with FreeRTOS+CLI to create a
 *    command console.
 *
 *  + The FreeRTOS TWI API being used to write to, and read from, an I2C EEPROM.
 *    The example can be configured to use either the fully asynchronous
 *    FreeRTOS API, or the blocking FreeRTOS API.
 *
 *  + The FreeRTOS SPI API being used to write to, and read from, an SPI flash.
 *    The example can be configured to use either the fully asynchronous
 *    FreeRTOS API, or the blocking FreeRTOS API.
 *
 * A FreeRTOS software timer is used to give visual feedback of the system
 * status.  The software timer toggles LED mainSOFTWARE_TIMER_LED every 200ms to
 * show the system is running, and turns LED mainERROR_LED on if any errors have
 * been latched.
 *
 * This file also provides default implementations of the FreeRTOS idle, tick
 * malloc failed, and stack overflow hook functions.  See
 * http://www.freertos.org/a00016.html.
 *
 * Readers are recommended to also read the application note that accompanies
 * the FreeRTOS ASF API.
 *
 * \section loopback USART Echo/Loopback Example
 * ************* USART Echo/Loopback Example ***********************************
 *
 * - Functionality -
 *
 * Two tasks are created;  A transmit (or Tx) task, and a receive (or Rx) task.
 * The transmit task periodically sends one of a set of strings to the USART
 * peripheral.  The receive task expects to receive each transmitted string.
 * The receive task latches an error is the characters it receives do not
 * exactly match the characters it expects to receive.
 *
 *
 * - Software Configuration -
 *
 * The USART loopback example is created if the confINCLUDE_USART_ECHO_TASKS
 * constant is defined.  confINCLUDE_USART_ECHO_TASKS can be defined in
 * conf_example.h.
 *
 * The USART loopback example and the USART CLI example cannot be used at the
 * same time as both examples access the same USART port.
 *
 *
 * - Hardware Setup -
 *
 * The example needs every character that is transmitted on the USART port
 * to be received on the same USART port.  This can be achieved using an RS232
 * echo server (see http://www.serialporttool.com/CommEcho.htm for an example),
 * or by simply linking pin 2 on the RS232 port to pin 3 on the same RS232 port.
 *
 * By default, the RS232 communication is configured to use 115200 baud, 8 data
 * bits, no parity, one stop bit, and no flow control.
 *
 * \section usart_cli USART Command Console using FreeRTOS+CLI
 * ************* USART Command Console using FreeRTOS+CLI **********************
 *
 * - Functionality -
 *
 * A task is created that receives command line input on a USART, then sends the
 * results of executing the command to the same USART peripheral.
 *
 * A number of example command implementations are provided.  These include:
 *
 *	+ "echo-parameters".  This command accepts a variable number of parameters.
 *    Each parameter entered is echoed back individually.
 *
 *	+ "echo-three-parameters".  This command is similar to "echo-parameters",
 *    but will only accept exactly three parameters.
 *
 *  + "task-stats".  This command displays a table that includes a line for each
 *    executing task.  The table displays information including the task state,
 *    and the task's stack high water mark.  (the closer the stack high water
 *    mark value is to zero the closer the task has come to overflowing its
 *    stack).
 *
 *	+ "run-time-stats".  This command also displays a table that also includes
 *    a line for each executing task.  This time the table displays the amount
 *    of time each task has spent in the "Running" state (actually executing).
 *    Both absolute and percentage times are shown.  See
 *    http://www.freertos.org/rtos-run-time-stats.html
 *
 *	+ "create-task".  This command takes a single numerical parameter.  It
 *     creates a new task and passes the command line parameter into the task as
 *     the task parameter.  The task that is created displays the parameter value
 *     on the command console.  The "task-stats" command can be used both before
 *     and after executing the "create-task" command to see the newly created
 *	   task appear in the table of running tasks.
 *
 *	+ "delete-task".  This command deletes the task that was created by the
 *    "create-task" command.
 *
 *
 * - Software Configuration -
 *
 * The USART command console example is created if the confINCLUDE_USART_CLI
 * constant is define.  confINCLUDE_USART_CLI can be defined in conf_example.h.
 *
 * The USART loopback example and the USART CLI example cannot be used at the
 * same time as both examples access the same USART port.
 *
 *
 * - Hardware Setup -
 *
 * The RS232 USART port needs to be connected to a dumb terminal, such as
 * TeraTerm.  For the cleanest output, the terminal should be set not to echo
 * characters, transmit line feeds (LF), and receive carriage returns (CR).
 *
 * By default, the RS232 communication is configured to use 115200 baud, 8 data
 * bits, no parity, one stop bit, and no flow control.
 *
 * \section uart_cli UART Command Console using FreeRTOS+CLI
 * ************* UART Command Console using FreeRTOS+CLI **********************
 *
 * - Functionality -
 *
 * The functionality is exactly as that described for the USART command console
 * above.  Only the communication interface is different.  The UART peripheral
 * is used as a com port.
 *
 * - Software Configuration -
 *
 * The UART command console example is created if the confINCLUDE_UART_CLI
 * constant is define.  confINCLUDE_UART_CLI can be defined in conf_example.h.
 *
 * - Hardware Setup -
 *
 * The RS232 UART port needs to be connected to a dumb terminal, such as
 * TeraTerm.  For the cleanest output, the terminal should be set not to echo
 * characters, transmit line feeds (LF), and receive carriage returns (CR).
 *
 * By default, the RS232 communication is configured to use 115200 baud, 8 data
 * bits, no parity, one stop bit, and no flow control.
 *
 * \section usb_cdc_cli USB/CDC Command Console using FreeRTOS+CLI
 * ************* USB/CDC Command Console using FreeRTOS+CLI ********************
 *
 * - Functionality -
 *
 * The functionality is exactly as that described for the USART command console
 * above.  Only the communication interface is different.  The standard ASF USB
 * CDC driver is used as a virtual com port.
 *
 * - Software Configuration -
 *
 * The USB/CDC command console example is created if the confINCLUDE_CDC_CLI
 * constant is defined.  confINCLUDE_CDC_CLI can be defined in conf_example.h.
 *
 * You may be asked to install a USB driver the first time the host computer is
 * plugged into the target.  The file atmel_devices_cdc.inf is provided with
 * this demo for this purpose.  Select the provided atmel_devices_cdc.inf file
 * if the install new hardware wizard executes on the host computer when the USB
 * cable is inserted.
 *
 * - Hardware Setup -
 *
 * The virtual com port was tested with the following settings:  115200 baud, 8
 * data bits, no parity, one stop bit, and no flow control.  The settings might
 * not be critical.
 *
 * \section twi_eeprom_rw TWI EEPROM Read and Write
 * ************* TWI EEPROM Read and Write ***********************************
 *
 * - Functionality -
 *
 * A task is created that writes a bit pattern to an I2C EEPROM, then reads back
 * from the EEPROM to ensure the data read back matches the data previously
 * written.  An error is latched if the data read back does not match the data
 * that was written.
 *
 * The example can be configured to use either the blocking or the fully
 * asynchronous FreeRTOS functions.
 *
 * - Software Configuration -
 *
 * The TWI EEPROM example is created if the confINCLUDE_TWI_EEPROM_TASK
 * constant is define.  confINCLUDE_TWI_EEPROM_TASK can be defined in
 * conf_example.h.
 *
 * Set mainDEMONSTRATE_ASYNCHRONOUS_API to 1 to use the fully asynchronous
 * FreeRTOS API, or 0 to use the blocking FreeRTOS API.  Other tasks will
 * execute while the task is blocked.  mainDEMONSTRATE_ASYNCHRONOUS_API is
 * defined in this file.
 *
 *
 * - Hardware Setup -
 *
 * The example requires that a TWI/I2C EEPROM be present on the evaluation kit.
 * As the example uses the EEPROM built onto the evaluation kit, no hardware
 * configuration is required.
 *
 * \section spi_flash_rw SPI flash Read and Write
 * ************* SPI flash Read and Write ***********************************
 *
 * - Functionality -
 *
 * A task is created that writes a bit pattern to an SPI flash, then reads back
 * from the flash to ensure the data read back matches the data previously
 * written.  An error is latched if the data read back does not match the data
 * that was written.
 *
 * The example can be configured to use either the blocking or the fully
 * asynchronous FreeRTOS functions.
 *
 * - Software Configuration -
 *
 * The SPI flash example is created if the confINCLUDE_SPI_FLASH_TASK
 * constant is define.  confINCLUDE_SPI_FLASH_TASK can be defined in
 * conf_example.h.
 *
 * Set mainDEMONSTRATE_ASYNCHRONOUS_API to 1 to use the fully asynchronous
 * FreeRTOS API, or 0 to use the blocking FreeRTOS API.  Other tasks will
 * execute while the task is blocked.  mainDEMONSTRATE_ASYNCHRONOUS_API is
 * defined in this file.
 *
 *
 * - Hardware Setup -
 *
 * The example requires that an SPI flash be present on the evaluation kit.
 * As the example uses the flash built onto the evaluation kit (SAM3N-EK only),
 * no hardware configuration is required.
 */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/* Demo includes. */
#include "partest.h"
#include "demo-tasks.h"
#include "conf_example.h"

/* ASF includes. */
#include "sysclk.h"

/* Defines the LED toggled to provide visual feedback that the system is
 * running.  The rate is defined in milliseconds, then converted to RTOS ticks
 * by the portTICK_RATE_MS constant. */
#define mainSOFTWARE_TIMER_LED                  (0)
#define mainSOFTWARE_TIMER_RATE                 (200 / portTICK_RATE_MS)

/* Defines the LED that is turned on if an error is detected. */
#define mainERROR_LED                           (1)

/* A block time of 0 ticks simply means "don't block". */
#define mainDONT_BLOCK                          (0)

/* The priorities at which various tasks will get created. */
#define mainUART_CLI_TASK_PRIORITY              (tskIDLE_PRIORITY + 1)
#define mainUSART_CLI_TASK_PRIORITY             (tskIDLE_PRIORITY + 1)
#define mainCDC_CLI_TASK_PRIORITY               (tskIDLE_PRIORITY + 1)
#define mainUSART_ECHO_TASK_PRIORITY            (tskIDLE_PRIORITY)
#define mainSPI_FLASH_TASK_PRIORITY             (tskIDLE_PRIORITY)
#define mainTWI_EEPROM_TASK_PRIORITY            (tskIDLE_PRIORITY)

/* The stack sizes allocated to the various tasks. */
#define mainUART_CLI_TASK_STACK_SIZE    (configMINIMAL_STACK_SIZE * 2)
#define mainUSART_CLI_TASK_STACK_SIZE   (configMINIMAL_STACK_SIZE * 2)
#define mainCDC_CLI_TASK_STACK_SIZE     (configMINIMAL_STACK_SIZE * 2)
#define mainUSART_ECHO_TASK_STACK_SIZE  (configMINIMAL_STACK_SIZE)
#define mainSPI_FLASH_TASK_STACK_SIZE   (configMINIMAL_STACK_SIZE * 2)
#define mainTWI_EEPROM_TASK_STACK_SIZE  (configMINIMAL_STACK_SIZE * 2)

/* When mainDEMONSTRATE_ASYNCHRONOUS_API is set to 1, the SPI and TWI
 * EEPROM/flash examples will use the fully asynchronous FreeRTOS API.
 * When mainDEMONSTRATE_ASYNCHRONOUS_API is set to 0, the SPI and TWI
 * EEPROM/flash examples will use the blocking FreeRTOS API. */
#define mainDEMONSTRATE_ASYNCHRONOUS_API        (0)

/*-----------------------------------------------------------*/

/*
 * Sets up the hardware ready to run this example.
 */
static void prvSetupHardware(void);

/*
 * The callback function used by the software timer.  See the comments at the
 * top of this file.
 */
static void prvLEDTimerCallback(void *pvParameters);

/*
 * FreeRTOS hook (or callback) functions that are defined in this file.
 */
void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(xTaskHandle pxTask,
		signed char *pcTaskName);
void vApplicationTickHook(void);

/*-----------------------------------------------------------*/

int main(void)
{
	xTimerHandle xLEDTimer;

	/* Prepare the hardware to run this demo. */
	prvSetupHardware();

	/* Create the timer that toggles an LED to show that the system is running,
	and that the other tasks are behaving as expected. */
	xLEDTimer = xTimerCreate((const signed char * const) "LED timer",/* A text name, purely to help debugging. */
							mainSOFTWARE_TIMER_RATE,	/* The timer period. */
							pdTRUE,						/* This is an auto-reload timer, so xAutoReload is set to pdTRUE. */
							NULL,						/* The timer does not use its ID, so the ID is just set to NULL. */
							prvLEDTimerCallback			/* The function that is called each time the timer expires. */
							);

	/* Sanity check the timer's creation, then start the timer.  The timer
	will not actually start until the FreeRTOS kernel is started. */
	configASSERT(xLEDTimer);
	xTimerStart(xLEDTimer, mainDONT_BLOCK);

	/* Create the example tasks as per the configuration settings.
	See the comments at the top of this file. */
	#if (defined confINCLUDE_UART_CLI)
	{
		create_uart_cli_task(BOARD_UART,
				mainUART_CLI_TASK_STACK_SIZE,
				mainUART_CLI_TASK_PRIORITY);
	}
	#endif /* confINCLUDE_USART_CLI */

	#if (defined confINCLUDE_USART_ECHO_TASKS)
	{
		create_usart_echo_test_tasks(BOARD_USART,
				mainUSART_ECHO_TASK_STACK_SIZE,
				mainUSART_ECHO_TASK_PRIORITY);
	}
	#endif /* confINCLUDE_USART_ECHO_TASKS */

	#if (defined confINCLUDE_USART_CLI)
	{
		create_usart_cli_task(BOARD_USART_CLI,
				mainUSART_CLI_TASK_STACK_SIZE,
				mainUSART_CLI_TASK_PRIORITY);
	}
	#endif /* confINCLUDE_USART_CLI */

	#if (defined confINCLUDE_CDC_CLI)
	{
		create_usb_cdc_cli_task(mainCDC_CLI_TASK_STACK_SIZE,
				mainCDC_CLI_TASK_PRIORITY);
	}
	#endif /* confINCLUDE_CDC_CLI */

	#if (defined confINCLUDE_SPI_FLASH_TASK)
	{
		create_spi_flash_test_task(BOARD_SPI,
				mainSPI_FLASH_TASK_STACK_SIZE,
				mainSPI_FLASH_TASK_PRIORITY,
				mainDEMONSTRATE_ASYNCHRONOUS_API);
	}
	#endif /* confINCLUDE_SPI_FLASH_TASK */

	#if (defined confINCLUDE_TWI_EEPROM_TASK)
	{
		create_twi_eeprom_test_task(BOARD_BASE_TWI_EEPROM,
				mainTWI_EEPROM_TASK_STACK_SIZE,
				mainTWI_EEPROM_TASK_PRIORITY,
				mainDEMONSTRATE_ASYNCHRONOUS_API);
	}
	#endif /* confINCLUDE_TWI_EEPROM_TASK */

	/* Start the RTOS scheduler. */
	vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */
	for (;;) {
	}
}

/*-----------------------------------------------------------*/

static void prvLEDTimerCallback(void *pvParameters)
{
	portBASE_TYPE xStatus = pdPASS;

	/* Just to remove compiler warnings. */
	(void) pvParameters;

	/* Check other tasks. */
	#if (defined confINCLUDE_USART_ECHO_TASKS)
	{
		if (are_usart_echo_tasks_still_running() != pdPASS) {
			xStatus = pdFAIL;
		}
	}
	#endif /* confINCLUDE_USART_ECHO_TASKS */

	#if (defined confINCLUDE_SPI_FLASH_TASK)
	{
		if (did_spi_flash_test_pass() != pdPASS) {
			xStatus = pdFAIL;
		}
	}
	#endif /* confINCLUDE_SPI_FLASH_TASK */

	#if (defined confINCLUDE_TWI_EEPROM_TASK)
	{
		if (did_twi_eeprom_test_pass() != pdPASS) {
			xStatus = pdFAIL;
		}
	}
	#endif /* configINCLUDE_TWI_EEPROM_TASK */

	/* If an error has been detected, turn the error LED on. */
	if (xStatus != pdPASS) {
		vParTestSetLED(mainERROR_LED, pdTRUE);
	}

	/* Toggle an LED to show the system is executing. */
	vParTestToggleLED(mainSOFTWARE_TIMER_LED);
}

/*-----------------------------------------------------------*/

static void prvSetupHardware(void)
{
	/* ASF function to setup clocking. */
	sysclk_init();

	/* Ensure all priority bits are assigned as preemption priority bits. */
	NVIC_SetPriorityGrouping(0);

	/* Atmel library function to setup for the evaluation kit being used. */
	board_init();

	/* Perform any initialisation required by the partest LED IO functions. */
	vParTestInitialise();
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook(void)
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for (;;) {
	}
}

/*-----------------------------------------------------------*/

void vApplicationIdleHook(void)
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}

/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(xTaskHandle pxTask,
		signed char *pcTaskName)
{
	(void) pcTaskName;
	(void) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for (;;) {
	}
}

/*-----------------------------------------------------------*/

void vApplicationTickHook(void)
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()). */
}

/*-----------------------------------------------------------*/

void assert_triggered(const char *file, uint32_t line)
{
	volatile uint32_t block_var = 0, line_in;
	const char *file_in;

	/* These assignments are made to prevent the compiler optimizing the
	values away. */
	file_in = file;
	line_in = line;
	(void) file_in;
	(void) line_in;

	taskENTER_CRITICAL();
	{
		while (block_var == 0) {
			/* Set block_var to a non-zero value in the debugger to
			step out of this function. */
		}
	}
	taskEXIT_CRITICAL();
}
