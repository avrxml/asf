/**
 * \file
 *
 * \brief USART example 2 application for AVR UC3 USART driver.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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

/**
 * \mainpage
 *
 * \section intro Introduction
 *
 * This demo application is a simple example demonstrating the basic use of the
 * AVR32 UC3 USART driver on a range of Atmel hardware.
 *
 * <b>Example's operating mode:</b>
 * -# A message is displayed on the PC terminal ("Hello, this is AT32UC3 saying
 *    hello! (press enter)")
 * -# You may then type any character other than CR(Carriage Return) and it will
 *    be echoed back to the PC terminal.
 * -# If you type a CR, "'CRLF'Goodbye." is echoed back to the PC terminal and
 *    the application ends.
 *
 * \section files Main Files
 * - usart.c: USART driver;
 * - usart.h: USART driver header file;
 * - usart_example2.c: USART example application.
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded
 * Workbench for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with a USART module can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - UC3-L0 Xplained Board
 * - UC3-A3 Xplained Board
 *
 * - PC terminal settings:
 *   - 57600 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.microchip.com/design-centers/32-bit/">Atmel AVR UC3</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include "board.h"
#include "sysclk.h"
#include "usart.h"
#include "conf_example_usart.h"

#if !defined(EXAMPLE_USART) || !defined(EXAMPLE_USART_BAUD)
# error Example USART configuration is missing or bad, check conf_example_usart.h
#endif

/**
 * \brief USART configuration options
 *
 * This struct contains the configuration options for the USART module in use
 * by the example application.
 */
const static usart_options_t usart_opt = {
	//! Baudrate is set in the conf_example_usart.h file.
	.baudrate    = EXAMPLE_USART_BAUD,
	.channelmode = USART_NORMAL_CHMODE,
	.charlength  = 8,
	.paritytype  = USART_NO_PARITY,
	.stopbits    = USART_1_STOPBIT,
};

/**
 * \brief Application main loop.
 */
int main(void)
{
	board_init();
	/**
	 * \note the call to sysclk_init() will disable all non-vital
	 * peripheral clocks, except for the peripheral clocks explicitly
	 * enabled in conf_clock.h.
	 */
	sysclk_init();

	/**
	 * Enable the clock to the selected example USART peripheral module.
	 * The clocks to all non-essential peripherals are disabled in the
	 * conf_clock.h file, so we need to re-enable our USART clock here.
	 */
	sysclk_enable_peripheral_clock(EXAMPLE_USART);

	/**
	 * Write the example USART module configuration to the hardware
	 * registers. The example USART is selected in the conf_example_usart.h
	 * file.
	 */
	usart_init_rs232(EXAMPLE_USART, &usart_opt,
			sysclk_get_peripheral_bus_hz(EXAMPLE_USART));

	/**
	 * Write a welcome message on the USART and busy-wait for the listener
	 * to press the enter key.
	 */
	usart_write_line(EXAMPLE_USART, "Hello, this is the AVR UC3 MCU saying"
			" hello! (press enter)\r\n");

	/**
	 * Busy-wait for a carriage return sent from the listener. Any received
	 * characters are echoed back.
	 */
	do { } while (usart_get_echo_line(EXAMPLE_USART) == USART_FAILURE);

	/**
	 * Send a goodbye message before entering a forever non-return loop.
	 */
	usart_write_line(EXAMPLE_USART, "Goodbye.\r\n");

	for (;;) {
		/* Intentionally loop forever. */
	}
}
