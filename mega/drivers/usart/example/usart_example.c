/**
 * \file
 *
 * \brief AVR MEGARF USART example
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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

/*! \mainpage
 * \section intro Introduction
 * This example demonstrates how to use MEGARF USART module.
 *
 * \section files Main Files
 * - usart_example.c: the example application.
 * - conf_board.h: board configuration
 * - conf_usart_example.h: configuration of the example
 *
 * \section usart_apiinfo drivers/usart API
 * The USART driver API can be found \ref megarf_usart_group "here".
 *
 * \section deviceinfo Device Info
 * All AVR MEGARF devices can be used. This example has been tested
 * with the following setup:
 *   - STK600 with ATmega128RFA1 on STK600-RC128X_RFX. (USARTA1 should be
 * connected to the RS232 spare)
 *
 * \section exampledescription Description of the example
 * The example waits for a received character on the configured USART and
 * echoes the character back to the same USART.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/avr">Atmel AVR</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <conf_usart_example.h>
#include <asf.h>

/*! \brief Main function.
 */
int main(void)
{
	uint8_t tx_buf[] = "\n\rHello AVR world ! : ";
	uint8_t tx_length = sizeof(tx_buf);
	uint8_t received_byte;
	uint8_t i;

	/* Initialize the board.
	 * The board-specific conf_board.h file contains the configuration of
	 * the board initialization.
	 */
	board_init();
	sysclk_init();

	/* USART options. */
	static usart_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = USART_SERIAL_EXAMPLE_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT
	};

	/* Initialize usart driver in RS232 mode */
	usart_init_rs232(USART_SERIAL_EXAMPLE, &USART_SERIAL_OPTIONS);

	/* Send "message header" */
	for (i = 0; i < tx_length; i++) {
		usart_putchar(USART_SERIAL_EXAMPLE, tx_buf[i]);
	}
	/* Get and echo a character forever, specific '\r' processing. */
	while (true) {
		received_byte = usart_getchar(USART_SERIAL_EXAMPLE);
		if (received_byte == '\r') {
			for (i = 0; i < tx_length; i++) {
				usart_putchar(USART_SERIAL_EXAMPLE, tx_buf[i]);
			}
		} else {
			usart_putchar(USART_SERIAL_EXAMPLE, received_byte);
		}
	}
}
