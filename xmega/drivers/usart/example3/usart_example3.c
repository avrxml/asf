/**
 * \file
 *
 * \brief AVR XMEGA USART example 3
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
 * This example demonstrates how to use XMEGA E USART module in one wire
 * communication mode.
 *
 * \section files Main Files
 * - usart_example3.c: the example application.
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
 *     USARTC0 one wire on pin PC2 must be connected
 *     to USARTD0 one wire on pin PD2.
 *     The PC4 must be connected to RS232 TXD pin on STK600
 *     to spy the one wire communication
 *   - XMEGA E5 Xplained evaluation kit
 *     USARTC0 one wire on pin PC2 (J1-PIN3) must be connected
 *     to USARTD0 one wire on pin PD2 (J4-PIN3).
 *     The PC4 (J1-PIN5) must be connected to TXD pin (PD7, J4-PIN6).
 *     of board controller to spy the one wire communication
 * UART configuration is 115200 baudrate, no parity, data 8 bit.
 *
 * \section exampledescription Description of the example
 * The example executes a transfer between two UARTs in one wire mode.
 * The first UART sends "ping" message and the second responses "pong".
 * Note: Using the XCL driver, the UART line PC2 is connected internaly to PC4
 * pin to allow the connection of a UART spy.
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

volatile bool message1_received = false;
volatile bool message2_received = false;

void send_onewire_message(USART_t *usart, uint8_t *msg, uint8_t len);

ISR(USART1_RX_Vect)
{
	if (usart_getchar(USART1_SERIAL_EXAMPLE) == '>') {
		message2_received = true;
	}

	usart_clear_tx_complete(USART1_SERIAL_EXAMPLE);
}

ISR(USART2_RX_Vect)
{
	if (usart_getchar(USART2_SERIAL_EXAMPLE) == '>') {
		message1_received = true;
	}

	usart_clear_tx_complete(USART2_SERIAL_EXAMPLE);
}

/**
 * \brief Send a sequence of bytes to USART one wire
 *
 * \param usart  Base address of the USART instance.
 * \param msg    Data buffer to read
 * \param len    Length of data
 *
 */
void send_onewire_message(USART_t *usart, uint8_t *msg, uint8_t len)
{
	uint8_t i;

	for (i = 0; i < len; i++) {
		usart_putchar_onewire(usart, msg[i]);
	}
}

/*! \brief Main function.
 */
int main(void)
{
	uint8_t ping_msg[] = "\n\rPing? ->";
	uint8_t pong_msg[] = "\n\r        Pong! ->";

	/* Initialize the board.
	 * The board-specific conf_board.h file contains the configuration of
	 * the board initialization.
	 */
	board_init();
	sysclk_init();
	irq_initialize_vectors();
	cpu_irq_enable();

	/* USART options. */
	static usart_xmegae_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = USART_SERIAL_EXAMPLE_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT,
		.start_frame_detection = false,
		.one_wire = true,
		.pec_length = 0,
		.encoding_type = USART_DECTYPE_DATA_gc,
		.encoding_stream = USART_LUTACT_OFF_gc,
	};

	/* Initialize usart driver in RS232 mode */
	usart_xmegae_init_rs232(USART1_SERIAL_EXAMPLE, &USART_SERIAL_OPTIONS);
	usart_xmegae_init_rs232(USART2_SERIAL_EXAMPLE, &USART_SERIAL_OPTIONS);

	usart_set_rx_interrupt_level(USART1_SERIAL_EXAMPLE, USART_INT_LVL_LO);
	usart_set_rx_interrupt_level(USART2_SERIAL_EXAMPLE, USART_INT_LVL_LO);

	/* To initiate the first transfer "Ping" */
	message2_received = true;

	/* Configure XCL so it copies the PC2 pin to PC4 (it allows to wire PC4
	 * to RS232 TXD pin on STK600 to spy the one wire communication */
	xcl_enable(XCL_ASYNCHRONOUS);
	xcl_port(PC);
	xcl_lut_type(LUT_2LUT2IN);
	xcl_lut_in0(LUT_IN_PINL);
	xcl_lut_in1(LUT_IN_XCL);
	xcl_lut0_output(LUT0_OUT_PIN4);
	xcl_lut_config_delay(DLY11, LUT_DLY_DISABLE, LUT_DLY_DISABLE);
	xcl_lut0_truth(IN0);

	/* Incoming character is process under interrupt
	 * main loop simply enters sleep mode */
	while (true) {
		if (message2_received) {
			/* When "pong" is received, send "ping" */
			gpio_toggle_pin(LED0_GPIO);
			message2_received = false;
			send_onewire_message(USART1_SERIAL_EXAMPLE, ping_msg,
					sizeof(ping_msg));
		}

		if (message1_received) {
			/* When "ping" is received, send "pong" */
			gpio_toggle_pin(LED0_GPIO);
			message1_received = false;
			send_onewire_message(USART2_SERIAL_EXAMPLE, pong_msg,
					sizeof(pong_msg));
		}
	}
}
