/**
 * \file
 *
 * \brief External Interrupt Controller Example for SAM4L.
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
 * \mainpage External Interrupt Controller Example
 *
 * \section Purpose
 *
 * The eic_example demonstrates how to use EIC driver toggle LED.
 *
 * \section Requirements
 *
 * This example can be used on SAM4L boards.
 *
 * \section Description
 *
 * The SAM4L controller waits for button press to generate external interrupt
 * and will toggle LED.
 *
 * \section Usage
 *
 * - Build the program and download it to the board
 * -# On the computer, open, and configure a terminal application
 *    (e.g., HyperTerminal on Microsoft&reg; Windows&reg;) with these settings:
 *   - 115200 bauds
 *   - 8 bits of data
 *   - No parity
 *   - 1 stop bit
 *   - No flow control
 * -# In the terminal window, the following text should appear (values depend
 *    on the board and chip used):
 *    \code
 *     -- EIC Example xxx --
 *     -- xxxxxx-xx
 *     -- Compiled: xxx xx xxxx xx:xx:xx --
 *    \endcode
 * -# the sent text should appear.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "asf.h"
#include "conf_eic_example.h"

#define STRING_EOL    "\r"
#define STRING_HEADER "-- EIC Example --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

volatile uint8_t bToggle = 0;

/**
 * \brief Interrupt handler for EIC interrupt.
 */
static void set_toggle_flag(void)
{
	/* Check if EIC push button line interrupt line is pending. */
	if (eic_line_interrupt_is_pending(EIC, GPIO_PUSH_BUTTON_EIC_LINE)) {
		eic_line_clear_interrupt(EIC, GPIO_PUSH_BUTTON_EIC_LINE);
		bToggle = 1;
	}
}

/**
 * \brief Configure UART console.
 */
static void configure_console(void)
{
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
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

//! [eic_example_code]
/**
 * \brief Interrupt handler for EIC interrupt.
 */
//! [set_eic_callback]
static void eic_callback(void)
{
	/* Check if EIC push button line interrupt line is pending. */
	if (eic_line_interrupt_is_pending(EIC, GPIO_PUSH_BUTTON_EIC_LINE)) {
		eic_line_clear_interrupt(EIC, GPIO_PUSH_BUTTON_EIC_LINE);
		bToggle = 1;
	}
}
//! [set_eic_callback]

/**
 * \brief EIC Setup
 */
//! [eic_setup]
static void eic_setup(void)
{
//! [enable_eic_module]
	eic_enable(EIC);
//! [enable_eic_module]

	struct eic_line_config eic_line_conf;
	eic_line_conf.eic_mode = EIC_MODE_EDGE_TRIGGERED;
	eic_line_conf.eic_edge = EIC_EDGE_FALLING_EDGE;
	eic_line_conf.eic_level = EIC_LEVEL_LOW_LEVEL;
	eic_line_conf.eic_filter = EIC_FILTER_DISABLED;
	eic_line_conf.eic_async = EIC_ASYNCH_MODE;

//! [configure_eic_mode]
	eic_line_set_config(EIC, GPIO_PUSH_BUTTON_EIC_LINE,	&eic_line_conf);
//! [configure_eic_mode]
	
//! [set_eic_callback_1]
	eic_line_set_callback(EIC, GPIO_PUSH_BUTTON_EIC_LINE, eic_callback,
	GPIO_PUSH_BUTTON_EIC_IRQ, 1);
//! [set_eic_callback_1]
	
//! [enable_eic_line]
	eic_line_enable(EIC, GPIO_PUSH_BUTTON_EIC_LINE);
//! [enable_eic_line]
}
//! [eic_setup]
//! [eic_example_code]

/**
 * \brief Application entry point for eic example.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();

	/* Initialize the UART console. */
	configure_console();

	/* Output example information. */
	puts(STRING_HEADER);

	eic_setup();

	puts("--Push the button to toggle the LED--\r\n\r");

	while (1) {
		if (bToggle == 1) {
			puts("--Toggle the LED--\r");
			ioport_toggle_port_level(EXAMPLE_LED_PORT,
					EXAMPLE_LED_MASK);
			bToggle = 0;
		}
	}
}
