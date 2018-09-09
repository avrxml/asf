/**
 * \file
 *
 * \brief AVR XMEGA USART example 2
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
 * <A href="http://www.microchip.com/design-centers/8-bit">Atmel AVR</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
