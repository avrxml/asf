/**
 *
 * \file
 *
 * \brief Weather concurrent and BT demo.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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

/** \mainpage
 * \section intro Introduction
 * This example demonstrates the use of the WILC3000 with the SAM Xplained Pro
 * board using WiFi concurrent mode STA/AP.
 * STA mode is used to connect to a home router with internet connection to
 * retrieve weather information. 
 * AP mode is used to allow any WiFi capable devices to connect to the WILC3000
 * and then access an embedded webserver to read weather information as well as
 * host MCU status (up to 7 connected stations at the same time).<br>
 *
 * To access the HTTP server connect with your laptop to WiFi network "WILC3000", then
 * open a browser at http://192.168.5.1<br>
 *
 * It uses the following hardware:
 * - the SAM Xplained Pro.
 * - the WILC3000 on EXT1 (WINC3400 can be used as well in bypass mode).
 *
 * \section files Main Files
 * - main.c : Initialize the FreeRTOS scheduler.
 * - sta.c : Enable STA and AP mode for WILC3000. Retrieve weather information via STA mode.
 * - ap.c : Enable HTTP server via AP mode.
 *
 * \section usage Usage
 * -# Configure below defines in sta.h to specify AP to connect to.
 * \code
 *    #define STA_WLAN_SSID         "DEMO_AP"
 *    #define STA_WLAN_AUTH         M2M_WIFI_SEC_WPA_PSK
 *    #define STA_WLAN_PSK          "12345678"
 * \endcode
 * -# Build the program and download it into the board.
 * -# On the computer, open and configure a terminal application as the follows.
 * \code
 *    Baud Rate : 115200
 *    Data : 8bit
 *    Parity bit : none
 *    Stop bit : 1bit
 *    Flow control : none
 * \endcode
 * -# Start the application.
 * -# In the terminal window, the following text should appear:
 * \code
 *    -- Weather concurrent an BT demo --
 *    -- SAMXXX_XPLAINED_PRO --
 *    -- Compiled: Oct 19 2015 14:39:47 --
 *    
 *    wifi_cb: M2M_WIFI_CONNECTED
 *    wifi_cb: STA M2M_WIFI_REQ_DHCP_CONF
 *    wifi_cb: STA IPv4 addr: xxx.xxx.xxx.xxx
 *    wifi_cb: STA IPv6 addr: xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx
 *    wifi_cb: AP M2M_WIFI_CONNECTED xx-xx-xx-xx-xx-xx
 * \endcode
 *
 * \section compinfo Compilation Information
 * This software was written for the GNU GCC compiler using Atmel Studio 6.2
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com">Atmel</A>.\n
 */ 

#include "asf.h"
#include "osprintf.h"
#include "driver/include/m2m_wifi.h"
#include "sta.h"
#include "ap.h"

#define TASK_STA_STACK_SIZE		(4096 / sizeof(portSTACK_TYPE))
#define TASK_STA_PRIORITY			(tskIDLE_PRIORITY + 1)
#define TASK_AP_STACK_SIZE		(1024 / sizeof(portSTACK_TYPE))
#define TASK_AP_PRIORITY			(tskIDLE_PRIORITY + 1)

#define STRING_EOL    "\r\n"
#define STRING_HEADER "-- Weather concurrent and BT demo --"STRING_EOL \
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
	
	/* Create main task. */
	xTaskCreate(sta_task, (const void *)"WiFiSTA", TASK_STA_STACK_SIZE, 0,
			TASK_STA_PRIORITY, 0);
			
	/* Create main task. */
	xTaskCreate(ap_task, (const void *)"WiFiAP", TASK_AP_STACK_SIZE, 0,
			TASK_AP_PRIORITY, 0);

	vTaskStartScheduler();
	
	while (1) {
	}

	return 0;
}
