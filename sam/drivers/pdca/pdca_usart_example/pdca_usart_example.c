/**
 * \file
 *
 * \brief Peripheral DMA Controller Example for SAM4L.
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
 * \mainpage Peripheral DMA Controller Example
 *
 * \section Purpose
 *
 * The pdca_usart example demonstrates how to use PDCA driver to send 
 * data to the USART.
 *
 * \section Requirements
 *
 * This example can be used on SAM4L series.
 *
 * \section Description
 *
 * The SAM4L controller sends data in ascii_anim1.h and ascii_anim2.h from USART
 * to the terminal.
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
	-- PDCA_USART Example xxx --
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

#define PDCA_TX_CHANNEL  0

#define STRING_EOL    "\r"
#define STRING_HEADER "-- PDCA_USART Example --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

/* Counter for interrupt test. */
volatile int bool_anim = 1;

/* String for ASCII animation: this is the first part of the ASCII animation. */
const char ascii_anim1[] =
#include "ascii_anim1.h"
;

/*
* String for ASCII animation: this is the second and final part of the ASCII
* animation.
*/
const char ascii_anim2[] =
#include "ascii_anim2.h"
;

/* PDCA channel options */
static const pdca_channel_config_t pdca_tx_configs = {
	.addr   = (void *)ascii_anim1,      /* memory address              */
	.pid    = PDCA_PID_USART_TX,        /* select peripheral           */
	.size   = sizeof(ascii_anim1),      /* transfer counter            */
	.r_addr = 0,                        /* next memory address         */
	.r_size = 0,                        /* next transfer counter       */
	.etrig  = false,                    /* disable the transfer upon event
										 * trigger */
	.ring   = false,                    /* disable ring buffer mode    */
	.transfer_size = PDCA_MR_SIZE_BYTE  /* select size of the transfer */
};

/**
 * \brief Interrupt handler for UART interrupt.
 */
static void pdca_tranfer_done(enum pdca_channel_status status)
{
	/* Get PDCA channel status and check if PDCA transfer complete */
	if (status == PDCA_CH_TRANSFER_COMPLETED) {
		/* Configure PDCA for data transfer */
		if (bool_anim == 1){
			pdca_channel_write_reload(PDCA_TX_CHANNEL, (void *)ascii_anim2,
				sizeof( ascii_anim2 ));
			bool_anim = 2;
		} else {
			pdca_channel_write_reload(PDCA_TX_CHANNEL, (void *)ascii_anim1,
				sizeof( ascii_anim1 ));
			bool_anim = 1;
		}
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

/**
 * \brief Application entry point for pdca_usart example.
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

	/* Enable PDCA module clock */
	pdca_enable(PDCA);

	/* Init PDCA channel with the pdca_options.*/
	pdca_channel_set_config(PDCA_TX_CHANNEL, &pdca_tx_configs);

	/* Enable PDCA channel */
	pdca_channel_enable(PDCA_TX_CHANNEL);

	pdca_channel_set_callback(PDCA_TX_CHANNEL, pdca_tranfer_done, PDCA_0_IRQn,
		1, PDCA_IER_RCZ);

	while (1) {
	}
}
