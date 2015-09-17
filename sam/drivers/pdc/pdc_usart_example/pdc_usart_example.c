/**
 * \file
 *
 * \brief Peripheral DMA Controller Example for SAM.
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
 * \mainpage Peripheral DMA Controller Example
 *
 * \section Purpose
 *
 * The pdc_usart example demonstrates how to use PDC driver to receive/send 
 * data from/to the USART.
 *
 * \section Requirements
 *
 * This example can be used on samg55j19_samg_xplained_pro.
 *
 * \section Description
 *
 * The SAM controller waits for BUFFER_SIZE data to receive from the USART.
 * As soon as the expected amount of data is received, the whole buffer is 
 * sent back to the terminal.
 *
 * \section Usage
 *
 * -# Build the program and download it into the evaluation board.
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
	-- PDC Usart Example xxx --
	-- xxxxxx-xx
	-- Compiled: xxx xx xxxx xx:xx:xx --
\endcode
 * -# the sent text should appear.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "asf.h"
#include "stdio_serial.h"
#include "conf_board.h"
#include "conf_pdc_usart_example.h"

#define BUFFER_SIZE  5

#define STRING_EOL    "\r"
#define STRING_HEADER "-- PDC_USART Example --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

/* Pdc transfer buffer */
uint8_t g_uc_pdc_buffer[BUFFER_SIZE];
/* PDC data packet for transfer */
pdc_packet_t g_pdc_usart_packet;
/* Pointer to USART PDC register base */
Pdc *g_p_usart_pdc;

/**
 * \brief Interrupt handler for USART interrupt.
 */
void console_usart_irq_handler(void)
{
	/* Get USART status and check if PDC receive buffer is full */
	if ((usart_get_status(CONSOLE_UART) & US_CSR_RXBUFF) == US_CSR_RXBUFF) {
		/* Configure PDC for data transfer (RX and TX) */
		pdc_rx_init(g_p_usart_pdc, &g_pdc_usart_packet, NULL);
		pdc_tx_init(g_p_usart_pdc, &g_pdc_usart_packet, NULL);
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
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * \brief Application entry point for pdc_usart example.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	/* Initialize the SAM system */
	sysclk_init();
	board_init();    

	/* Initialize the UART console */
	configure_console();

	/* Output example information */
	puts(STRING_HEADER);

	/* Get pointer to USART PDC register base */
	g_p_usart_pdc = usart_get_pdc_base(CONSOLE_UART);

	/* Initialize PDC data packet for transfer */
	g_pdc_usart_packet.ul_addr = (uint32_t) g_uc_pdc_buffer;
	g_pdc_usart_packet.ul_size = BUFFER_SIZE;

	/* Configure PDC for data receive */
	pdc_rx_init(g_p_usart_pdc, &g_pdc_usart_packet, NULL);

	/* Enable PDC transfers */
	pdc_enable_transfer(g_p_usart_pdc, PERIPH_PTCR_RXTEN | PERIPH_PTCR_TXTEN);

	/* Enable USART IRQ */
	usart_enable_interrupt(CONSOLE_UART, US_IER_RXBUFF);

	/* Enable USART interrupt */
	NVIC_EnableIRQ(CONSOLE_USART_IRQn);

	while (1) {
	}
}
