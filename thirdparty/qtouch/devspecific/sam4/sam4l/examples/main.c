/**
 * \file
 *
 * \brief SAM4L QTouch Example.
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
 * \mainpage SAM4L QTouch Example
 *
 * \section Purpose
 *
 * This example demonstrates how to use QTouch library on SAM4L devices.
 *
 * \section Requirements
 *
 * This example can be used on SAM4L series.
 *
 * \section Description
 *
 * The program configures the necessary modules for using QTouch library. After
 * it started, users can turn on and off the on-board LED by touching the QTouch
 * key.
 *
 * \section Usage
 *
 * -# Build the program and download it into the evaluation board. Please
 *    refer to the
 *    <a href="http://www.atmel.com/dyn/resources/prod_documents/6421B.pdf">
 *    SAM-BA User Guide</a>, the
 *    <a href="http://www.atmel.com/dyn/resources/prod_documents/doc6310.pdf">
 *    GNU-Based Software Development</a>
 *    application note or the
 *    <a href="http://www.iar.com/website1/1.0.1.0/78/1/">
 *    IAR EWARM User and reference guides</a>,
 *    depending on the solutions that users choose.
 * -# On the computer, open and configure a terminal application
 *    (e.g., HyperTerminal on Microsoft Windows) with these settings:
 *   - 115200 bauds
 *   - 8 bits of data
 *   - No parity
 *   - 1 stop bit
 *   - No flow control
 * -# In the terminal window, the following text should appear (values depend
 *    on the board and chip used):
 *    \code
	-- SAM4L QTouch Example xxx --
	-- xxxxxx-xx
	-- Compiled: xxx xx xxxx xx:xx:xx --
\endcode
 * -# the sent text should appear.
 */
#include "asf.h"
#include "conf_example.h"

#define STRING_EOL    "\r"
#define STRING_HEADER "-- SAM4L QTouch Example --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

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

static void ast_per_callback(void)
{
	touch_sensors_update_time();
	ast_clear_interrupt_flag(AST, AST_INTERRUPT_PER);
}

static void example_qtouch_init(void)
{
	struct ast_config ast_conf;
	/* Enable Osc32 */
	osc_enable(OSC_ID_OSC32);
	while(!osc_is_ready(OSC_ID_OSC32));

	/* Initialize AST Controller */
	ast_enable(AST);

	ast_conf.mode = AST_COUNTER_MODE;
	ast_conf.osc_type = AST_OSC_32KHZ;
	ast_conf.psel = AST_PSEL_32KHZ_1HZ;
	ast_conf.counter = 0;
	ast_set_config(AST, &ast_conf);

	ast_clear_interrupt_flag(AST, AST_INTERRUPT_PER);
	ast_write_periodic0_value(AST, EXAMPLE_UPDATE_PERIOD);

	ast_set_callback(AST, AST_INTERRUPT_PER, ast_per_callback,
		AST_PER_IRQn, 0);

	/* CATB & Related clocks - for QTouch Sensors */
	sysclk_enable_peripheral_clock(CATB);
	sysclk_enable_peripheral_clock(PDCA);

	/* Initialize QTouch Library. */
	touch_sensors_init();
}

/**
 *  \brief SAM4L QTouch example entry point.
 *  \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	bool pressed = false;

	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	/* Initialize the UART console */
	configure_console();

	/* Output example information */
	puts(STRING_HEADER);

	/* Initialize Qtouch */
	example_qtouch_init();

	while (1) {
		touch_sensors_measure();
		if (GET_QT_SENSOR_STATE(EXAMPLE_KEY_ID) && !pressed) {
			pressed = true;
			LED_On(LED0);
		} else {
			if (!GET_QT_SENSOR_STATE(EXAMPLE_KEY_ID) && pressed) {
				LED_Off(LED0);
				pressed = false;
			}
		}
	}

}
