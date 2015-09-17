/**
 * \file
 *
 * \brief SAM Peripheral DMA Controller Example.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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
#include <stdio_serial.h>
#include <conf_board.h>
#include <conf_pdc_uart_example.h>

#define BUFFER_SIZE  5

#define STRING_EOL    "\r"
#define STRING_HEADER "-- PDC_UART Example --\r\n" \
"-- "BOARD_NAME" --\r\n" \
"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

/* Pdc transfer buffer */
uint8_t g_uc_pdc_buffer[BUFFER_SIZE];
/* PDC data packet for transfer */
pdc_packet_t g_pdc_uart_packet;
/* Pointer to UART PDC register base */
Pdc *g_p_uart_pdc;

/**
 * \brief Interrupt handler for UART interrupt.
 */

//! [int_handler]
void console_uart_irq_handler(void)
{
	/* Get UART status and check if PDC receive buffer is full */
	if ((uart_get_status(CONSOLE_UART) & UART_SR_RXBUFF) == UART_SR_RXBUFF) {
		/* Configure PDC for data transfer (RX and TX) */
		pdc_rx_init(g_p_uart_pdc, &g_pdc_uart_packet, NULL);
		pdc_tx_init(g_p_uart_pdc, &g_pdc_uart_packet, NULL);
	}
}
//! [int_handler]

/**
 * \brief Configure UART console.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};
	
	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * \brief Application entry point for pdc_uart example.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	/* Initialize the SAM system */

	//! [board_setup]
	sysclk_init();
	board_init();
	//! [board_setup]

	/* Initialize the UART console */
	configure_console();

	/* Output example information */
	puts(STRING_HEADER);

	//! [pdc_config]
	/* Get pointer to UART PDC register base */
	g_p_uart_pdc = uart_get_pdc_base(CONSOLE_UART);

	/* Initialize PDC data packet for transfer */
	g_pdc_uart_packet.ul_addr = (uint32_t) g_uc_pdc_buffer;
	g_pdc_uart_packet.ul_size = BUFFER_SIZE;

	/* Configure PDC for data receive */
	pdc_rx_init(g_p_uart_pdc, &g_pdc_uart_packet, NULL);

	/* Enable PDC transfers */
	pdc_enable_transfer(g_p_uart_pdc, PERIPH_PTCR_RXTEN | PERIPH_PTCR_TXTEN);
	//! [pdc_config]

	/* Enable UART IRQ */
	//! [uart_irq]
	uart_enable_interrupt(CONSOLE_UART, UART_IER_RXBUFF);
	//! [uart_irq]

	/* Enable UART interrupt */
	//! [uart_nvic_irq]
	NVIC_EnableIRQ(CONSOLE_UART_IRQn);
	//! [uart_nvic_irq]

	//! [busy_waiting]
	while (1) {
	}
	//! [busy_waiting]
}
