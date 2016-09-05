/**
 *
 * \file
 *
 * \brief IoT Temperature Sensor Demo.
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
 * This IoT temperature sensor demo is configured to work with the SAM D21
 * Xplained Pro using the WINC1500 Wi-Fi module. It will basically report
 * temperature sensor information to provided Android application via your
 * home network. The Android application can be located in the android_app
 * folder.
 * It uses the following hardware:
 * - the WINC1500 module on EXT1.
 * - the IO1 sensors on EXT2 (optional).
 *
 * \section files Main Files
 * - main.c : initialize the SAMD21 system and run the main demo routine
 * - demo.c : handle the wifi and sensor logic
 * - demo.h : sensor demo header file
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC compiler using Atmel Studio 6.2
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com">Atmel</A>.\n
 */

#include "asf.h"
#include <string.h>
#include "stdio_serial.h"
#include "conf_uart_serial.h"
#include "demo.h"

#define STRING_EOL    "\r\n"
#define STRING_HEADER "-- WINC1500 IoT temperature sensor demo --"STRING_EOL \
		"-- "BOARD_NAME" --"STRING_EOL \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

#if SAMD21
/** UART module for debug. */
static struct usart_module cdc_uart_module;
#endif

/** SysTick counter to avoid busy wait delay. */
volatile uint32_t ms_ticks = 0;

/**
 * \brief Configure UART console.
 */
static void configure_console(void)
{
#if SAMD21
	struct usart_config usart_conf;

	usart_get_config_defaults(&usart_conf);
	usart_conf.mux_setting = CONF_STDIO_MUX_SETTING;
	usart_conf.pinmux_pad0 = CONF_STDIO_PINMUX_PAD0;
	usart_conf.pinmux_pad1 = CONF_STDIO_PINMUX_PAD1;
	usart_conf.pinmux_pad2 = CONF_STDIO_PINMUX_PAD2;
	usart_conf.pinmux_pad3 = CONF_STDIO_PINMUX_PAD3;
	usart_conf.baudrate    = CONF_STDIO_BAUDRATE;

	stdio_serial_init(&cdc_uart_module, CONF_STDIO_USART_MODULE, &usart_conf);
	usart_enable(&cdc_uart_module);
#elif SAME70
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
#ifdef CONF_UART_CHAR_LENGTH
		.charlength = CONF_UART_CHAR_LENGTH,
#endif
		.paritytype = CONF_UART_PARITY,
#ifdef CONF_UART_STOP_BITS
		.stopbits = CONF_UART_STOP_BITS,
#endif
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
#endif
}

/**
 * \brief SysTick handler used to measure precise delay. 
 */
void SysTick_Handler(void)
{
	ms_ticks++;
}

/**
 * \brief Main application function. 
 *
 * Start the sensor task then start the scheduler.
 *
 * \return program return value.
 */
int main(void)
{
#if SAMD21
	system_init();
#elif SAME70
	sysclk_init();
	board_init();
#endif

	/* Initialize the UART console. */
	configure_console();

	/* Initialize the delay driver. */
	delay_init();
			
	/* Enable SysTick interrupt for non busy wait delay. */
#if SAMD21
	if (SysTick_Config(system_cpu_clock_get_hz()/1000)) {
		puts("main: SysTick configuration error!");
		while (1);
	}
#elif SAME70
	if (SysTick_Config(sysclk_get_main_hz()/1000)) {
		puts("main: SysTick configuration error!");
		while (1);
	}
#endif


	/* Output example information */
	puts(STRING_HEADER);

	/* Start the demo task. */
	demo_start();
	
	return 0;
}
