/**
 * \file
 *
 * \brief SAM Asynchronous Timer (AST) example 1.
 *
 * Copyright (C) 2012-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include "ast_example1.h"

/**
 *  \brief Configure serial console.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
#ifdef CONF_UART_CHAR_LENGTH
		.charlength = CONF_UART_CHAR_LENGTH,
#endif /* CONF_UART_CHAR_LENGTH */
		.paritytype = CONF_UART_PARITY,
#ifdef CONF_UART_STOP_BITS
		.stopbits = CONF_UART_STOP_BITS,
#endif /* CONF_UART_STOP_BITS */
	};

	// Configure console.
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * \brief main function : do init and loop
 */
int main(void)
{

	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	/* Initialize the console uart */
	configure_console();

	/* Output example information */
	printf("-- AST Example 1 in Calendar Mode --\r\n");
	printf("-- %s\n\r", BOARD_NAME);
	printf("-- Compiled: %s %s --\n\r", __DATE__, __TIME__);

	printf("Config AST with 32 KHz oscillator.\r\n");

	/* Enable osc32 oscillator*/
	if (!osc_is_ready(OSC_ID_OSC32)) {
		osc_enable(OSC_ID_OSC32);
		osc_wait_ready(OSC_ID_OSC32);
	}

	struct ast_calendar calendar;
	calendar.FIELD.sec = 0;
	calendar.FIELD.min = 15;
	calendar.FIELD.hour = 12;
	calendar.FIELD.day = 20;
	calendar.FIELD.month = 9;
	calendar.FIELD.year = 14;

	/* Enable the AST */
	ast_enable(AST);

	struct ast_config ast_conf;
	ast_conf.mode = AST_CALENDAR_MODE;
	ast_conf.osc_type = AST_OSC_32KHZ;
	ast_conf.psel = AST_PSEL_32KHZ_1HZ;
	ast_conf.calendar = calendar;

	/* Initialize the AST */
	if (!ast_set_config(AST, &ast_conf)) {
		printf("Error initializing the AST\r\n");
		while (1) {
		}
	}

	while (1) {
		/* slow down operations */
		delay_s(1);

		ioport_toggle_pin_level(LED0_GPIO);

		/* Output the calendar */
		calendar = ast_read_calendar_value(AST);
		printf("\r");
		printf("Calendar: Year:%02u Month:%02u Day:%02u, %02uh%02um%02us ",
				(unsigned int)calendar.FIELD.year,
				(unsigned int)calendar.FIELD.month,
				(unsigned int)calendar.FIELD.day,
				(unsigned int)calendar.FIELD.hour,
				(unsigned int)calendar.FIELD.min,
				(unsigned int)calendar.FIELD.sec);
	}
}
