/**
 *
 * \file
 *
 * \brief WILC1000 TCP Server Example
 *
 * Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
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
 */

/** \mainpage
 * \section intro Introduction
 * This example demonstrates the use of the WILC with the SAM Xplained Pro
 * board to test TCP server.<br>
 *
 * This example implements an wireless simple udp server v1.0.0. using WILC device using Socket related APIs.
 * The WILC device as server is connected to a local Wi-Fi network/router as udp server.
 * Connect the laptop as wireless tcp client to the router by running the tcp client py script for testing,
 * The client sends data to the server on every reset (continuously) and can be monitored on the terminal emulator.
 *
 *
 * It uses the following hardware:
 * - the SAM Xplained Pro.
 * - the WILC on EXT1.
 *
 * \section files Main Files
 * - main.c : Initialize the FreeRTOS scheduler.
 * - simple_tcp_server_example.c : Implements the TCP server logic.
 *
 * \section usage Usage
 * -# Configure below defines in sta.h to specify AP to connect to.
 * \code
 *    #define MAIN_WLAN_SSID         "DEMO_AP"
 *    #define MAIN_WLAN_AUTH         M2M_WIFI_SEC_WPA_PSK
 *    #define MAIN_WLAN_PSK          "12345678"
 * \endcode
 *
 * \Make following changes in the code to compile and run successfully
 * Define LWIP_SOCKET as 1 (lwipopts.h) and LWIP_TIMEVAL_PRIVATE as 0 (sockets.h)
 *
 *
 * -# Build the program and download it into the board.
 * -# On the computer, open and configure a terminal application as the follows.
 * \code
 *    Baud Rate : 115200
 *    Data : 8bit
 *    Parity bit : none
 *    Stop bit : 1bit
 *    Flow control : none
 * \endcode
 *
 * -# Start the application.
 *
 *\run TCP Client script on PC to send data to TCP Server (from simple_tcp_server_example\TestScript)
 *
 * \section Compilation Information
 * This software was written for the GNU GCC compiler using Atmel Studio 6.2
 * Other compilers may or may not work.
 *
 * \section Contact Information
 * For further information, visit
 * <A href="http://www.microchip.com">Microchip</A>.\n
 */

#include "asf.h"
#include "osprintf.h"
#include "driver/include/m2m_wifi.h"
#include "tcp_server.h"
#include "lwipopts.h"

#define TASK_DEMO_STACK_SIZE		(8192 / sizeof(portSTACK_TYPE))
#define TASK_DEMO_PRIORITY			(tskIDLE_PRIORITY + 2)

#define STRING_EOL    "\r\n"
#define STRING_HEADER "-- WILC1000 Simple TCP Server Test  --"STRING_EOL \
		"-- "BOARD_NAME " --"STRING_EOL	\
		"-- Compiled: "__DATE__ " "__TIME__ " --"STRING_EOL

/** SysTick counter to avoid busy wait delay. */
uint32_t ms_ticks = 0;

void fatal(int code, const char *msg);
void fatal(int code, const char *msg)
{
	for (;;) {
	}
}

#if configUSE_MALLOC_FAILED_HOOK
void vApplicationMallocFailedHook(void);
void vApplicationMallocFailedHook(void)
{
	for (;;) {
	}
}
#endif

#if configCHECK_FOR_STACK_OVERFLOW
void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName);
void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName)
{
	for (;;) {
	}
}
#endif

static uint32_t fault_addr;
void hard_fault(uint32_t *args, uint32_t lr);
void hard_fault(uint32_t *args, uint32_t lr)
{
	fault_addr = args[6];
	for (;;) {
	}
}

void HardFault_Handler(void)
{
	__asm(
		"  mov r0, #4          \n"
		"  mov r1, lr          \n"
		"  tst r0, r1          \n"
		"  beq using_msp       \n"
		"  mrs r0, psp         \n"
		"  b call_c            \n"
		"using_msp:            \n"
		"  mrs r0, msp         \n"
		"call_c:               \n"
		"  ldr r2, =hard_fault \n"
		"  bx r2               \n"
	);
}

/**
 * \brief Configure UART console (RTOS API).
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_STDIO_BAUDRATE,
#ifdef CONF_STDIO_CHAR_LENGTH
		.charlength = CONF_STDIO_CHAR_LENGTH,
#endif
		.paritytype = CONF_STDIO_PARITY,
#ifdef CONF_STDIO_STOP_BITS
		.stopbits = CONF_STDIO_STOP_BITS
#endif
	};

	/* Configure UART console. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	osprintf_init(CONF_STDIO_USART_MODULE, &uart_serial_options);
}

/**
 * \brief Main program function.
 */
int main(void)
{
	/* Initialize the board. */
	sysclk_init();
	board_init();

	/* Initialize the UART console. */
	configure_console();
	puts(STRING_HEADER);

#if LWIP_SOCKET /* don't build if not configured for use in lwipopts.h */
	/* Create main task. */
	xTaskCreate(tcp_server_task, (signed char *)"TestTCPServer", TASK_DEMO_STACK_SIZE, 0, TASK_DEMO_PRIORITY, 0);		// modified by SP123
#endif /* LWIP_SOCKET  don't build if not configured for use in lwipopts.h */
	
	vTaskStartScheduler();
	
	while (1) {
	}

	return 0;
}
