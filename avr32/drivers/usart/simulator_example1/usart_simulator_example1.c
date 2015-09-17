/**
 * \file
 *
 * \brief USART Example application for AVR UC3 USART driver.
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
 * \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the USART software driver.\n It also comes
 * bundled with an example.
 * This is a basic example for use with AVR Studio 5 Simulator. \n
 * <b>Example's operating mode: </b>
 * -# Add test_out and test_in to the Watch window
 * -# Place breakpoint in usart_getchar() function
 * -# The test data sent will be echoed back to the Receiver
 *
 * \section files Main Files
 * - usart.c: GPIO driver
 * - usart.h: GPIO driver header file
 * - conf_example.h: USART configuration file
 * - conf_clock.h: Clock Configuration for example
 * - conf_board.h: Simulator Configuration for example
 * - usart_simulator_example1.c: USART Example application
 *
 * \section compilinfo Compilation Information
 * This software is written for GCC for 32-bit AVR
 * Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All 32-bit AVR device with an USART module supported by the simulator in AVR
 * Studio 5 can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - AVR Simulator in AVR Studio 5.0
 * - CPU clock: 115 KHz
 * - USART1 in Local Loopback mode
 * - USART settings:
 *   - 9600 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/uc3">Atmel AVR UC3</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <stdint.h>

#include "usart.h"
#include "board.h"
#include "sysclk.h"
#include "gpio.h"
#include "conf_board.h"
#include "conf_clock.h"
#include "conf_example.h"

//! \name Example configuration
//!@{
/**
 * \def CONFIG_SYSCLK_SOURCE
 * \brief Clock source to use
 */
/**
 * \def EXAMPLE_USART
 * \brief Base address of USART module to use.
 */
/**
 * \def EXAMPLE_USART_BAUD
 * \brief Baud rate to use with USART module.
 */
/**
 * \def EXAMPLE_USART_RX_PIN
 * \brief GPIO Pin to use as USART RX.
 */
/**
 * \def EXAMPLE_USART_RX_FUNCTION
 * \brief Alternate function of the GPIO pin to use (Here USART RX).
 */
/**
 * \def EXAMPLE_USART_TX_PIN
 * \brief GPIO Pin to use as USART TX.
 */
/**
 * \def EXAMPLE_USART_TX_FUNCTION
 * \brief Alternate function of the GPIO pin to use (Here USART TX).
 */
//!@}

// Check whether USART is configured
#if !defined(EXAMPLE_USART) || !defined(EXAMPLE_USART_BAUD)
# error Example USART configuration is missing or bad, check conf_example_usart.h
#endif

/**
 * \brief USART configuration options
 *
 * This struct contains the configuration options for the USART module in use
 * by the example application.
 */
//@{
//! GPIO mapping for USART1 module
static const gpio_map_t usart_gpio_map = {
	{EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
	{EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION}
};

//! USART options for enabling local loopback mode.
static const usart_options_t usart_options = {
	.baudrate     = EXAMPLE_USART_BAUD,
	.charlength   = 8,
	.paritytype   = USART_NO_PARITY,
	.stopbits     = USART_1_STOPBIT,
	.channelmode  = USART_LOCAL_LOOPBACK
};
//@}

/**
 * \brief Application main loop.
 *
 * The main function does the following:
 * - Configures the USART module
 * - Performs loopback transfer of data
 * - Enters an infinite loop on completion of example
 *
 * \note Use no compiler optimization to be able to watch test_in in the watch
 * window.
 */
int main(void)
{
	uint8_t test_out[5] = {0xAA, 0x55, 0x77, 0xFF, 0xCC};
	uint8_t test_in[5];
	uint8_t i;

	/**
	 * \note Call to sysclk_init() will disable all non-vital
	 * peripheral clocks, except for the peripheral clocks explicitly
	 * enabled in conf_clock.h.
	 */
	sysclk_init();
	sysclk_enable_peripheral_clock(EXAMPLE_USART);

	// Assign GPIO to USART.
	gpio_enable_module(usart_gpio_map,
			sizeof(usart_gpio_map) / sizeof(usart_gpio_map[0]));

	usart_init_rs232(EXAMPLE_USART, &usart_options,
			sysclk_get_pba_hz());

	/*
	 * The sent data is immediately available since the USART module is in
	 * loopback mode.
	 *
	 * Add the test_out & test_in variables to the watch window. The
	 * received data can be verified by inspecting the test_in variable
	 * after every transmission by placing a breakpoint in the
	 * usart_putchar() function.
	 */
	for (i = 0; i < sizeof(test_out); i++) {
		usart_putchar(EXAMPLE_USART, test_out[i]);
		test_in[i] = usart_getchar(EXAMPLE_USART);
	}

	// Intentionally loop endlessly after completion of example.
	while (true) {
		cpu_relax();
	}
}
