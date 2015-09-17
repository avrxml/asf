/**
 * \file
 *
 * \brief AVR XMEGA USART example 2
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
 * This example demonstrates how to use XMEGA E USART module with Start Frame
 * Detection.
 *
 * \section files Main Files
 * - usart_example2.c: the example application.
 * - conf_board.h: board configuration
 * - conf_usart_example.h: configuration of the example
 *
 * \section usart_apiinfo drivers/usart API XMEGA E specific
 * The USART XMEGA E specific driver extension can be found
 * \ref usart_xmegae_group "here".
 *
 * \section deviceinfo Device Info
 * All AVR XMEGA devices can be used. This example has been tested
 * with the following setup:
 *   - STK600 with ATxmega32E5 on STK600-RC032X
 *     USARTC0 on PC2 and PC3 pins should be connected to the RXD and TXD
 *     pins of STK600 RS232 spare port.
 *   - XMEGA E5 Xplained evaluation kit
 *     USARTD0 on PORTD pin 6 and 7 is used (Connected to board controller)
 * UART configuration is 115200 baudrate, no parity, data 8 bit.
 *
 * \section exampledescription Description of the example
 * The example waits for a received character on the configured USART and
 * echoes the character back to the same USART. When waiting for a character,
 * the cpu core enters standby mode and is wake up using the start frame
 * detection on USART.
 * Using the internal 8MHz Oscilator of ATxmega32E5, the wakeup character is not
 * lost.
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

ISR(USART_RX_Vect)
{
	usart_putchar(USART_SERIAL_EXAMPLE,
			usart_getchar(USART_SERIAL_EXAMPLE));
	while (!usart_tx_is_complete(USART_SERIAL_EXAMPLE)) {
	}
	usart_clear_tx_complete(USART_SERIAL_EXAMPLE);
}

/*! \brief Main function.
 */
int main(void)
{
	uint8_t tx_buf[] = "\n\rHello AVR world ! : ";
	uint8_t i;

	/* Initialize the board.
	 * The board-specific conf_board.h file contains the configuration of
	 * the board initialization.
	 */
	board_init();
	sysclk_init();
	pmic_init();
	cpu_irq_enable();
	sleepmgr_init();
	sleepmgr_lock_mode(SLEEPMGR_STDBY);

	/* USART options. */
	static usart_xmegae_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = USART_SERIAL_EXAMPLE_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT,
		.start_frame_detection = true,
		.one_wire = false,
		.pec_length = 0,
		.encoding_type = USART_DECTYPE_DATA_gc,
		.encoding_stream = USART_LUTACT_OFF_gc,
	};

	/* Initialize usart driver in RS232 mode */
	usart_xmegae_init_rs232(USART_SERIAL_EXAMPLE, &USART_SERIAL_OPTIONS);
	usart_set_rx_interrupt_level(USART_SERIAL_EXAMPLE, USART_INT_LVL_LO);

	/* Send "message header" */
	for (i = 0; i < sizeof(tx_buf); i++) {
		usart_putchar(USART_SERIAL_EXAMPLE, tx_buf[i]);
		while (!usart_tx_is_complete(USART_SERIAL_EXAMPLE)) {
		}
		usart_clear_tx_complete(USART_SERIAL_EXAMPLE);
	}

	/* Incoming character is process under interrupt
	 * main loop simply enters sleep mode */
	while (true) {
		sleepmgr_enter_sleep();
	}
}
