/**
 * \file
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "bootloader.h"

#ifdef DBG_USE_USART

#define DBG_USART_BASE ((Usart*)ATPASTE2(DBG_USART_NAME,DBG_USART_NB))
#define DBG_USART_ID   (ATPASTE3(ID_,DBG_USART_NAME,DBG_USART_NB))

#ifndef DBG_USART_PARITY
#  define DBG_USART_PARITY     US_MR_PAR_NO
#endif
#ifndef DBG_USART_CHR_LENGTH
#  define DBG_USART_CHR_LENGTH US_MR_CHRL_8_BIT
#endif
#ifndef DBG_USART_STOP_BITS
#  define DBG_USART_STOP_BITS  US_MR_NBSTOP_1_BIT
#endif
#ifndef DBG_USART_MODE
#  define DBG_USART_MODE       US_MR_CHMODE_NORMAL
#endif

static void _dbg_usart_pins_init(void)
{
#ifdef DBG_USART_PIN_EN
	ioport_set_pin_dir(DBG_USART_PIN_EN, IOPORT_DIR_OUTPUT);
	ioport_enable_pin(DBG_USART_PIN_EN);
	ioport_set_pin_level(DBG_USART_PIN_EN, DBG_USART_PIN_EN_LEVEL);
#endif
#if defined(DBG_USART_PIN_TX)
	ioport_set_pin_mode(DBG_USART_PIN_TX, DBG_USART_PIN_TX_MUX);
	ioport_disable_pin(DBG_USART_PIN_TX);
#endif
#if defined(DBG_USART_PIN_RX)
	ioport_set_pin_mode(DBG_USART_PIN_RX, DBG_USART_PIN_TX_MUX);
	ioport_disable_pin(DBG_USART_PIN_RX);
#endif
}

/**
 * Initialize USART serial as debug communication port
 */
void dbg_usart_init(void)
{
	/* USART pins init */
	_dbg_usart_pins_init();
	/* Configure USART */
    const usart_serial_options_t options = {
        .baudrate   = DBG_USART_BAUDRATE,
		.charlength = DBG_USART_CHR_LENGTH,
		.paritytype = DBG_USART_PARITY,
		.stopbits   = DBG_USART_STOP_BITS
    };
	stdio_serial_init(DBG_USART_BASE, &options);
}

/**
 * Close USART serial debug communication port
 */
void dbg_usart_cleanup(void)
{
	usart_disable_tx(DBG_USART_BASE);
	usart_disable_rx(DBG_USART_BASE);
	usart_reset(DBG_USART_BASE);
	pmc_disable_periph_clk(DBG_USART_ID);
}

/**
 * Check if there is console input
 * \return true if there is input
 */
bool dbg_usart_rx_ready(void)
{
	return usart_serial_is_rx_ready(DBG_USART_BASE);
}

/**
 * Get a byte from console
 * \return byte read
 */
uint8_t dbg_usart_getchar(void)
{
	uint8_t byte;
	usart_serial_getchar(DBG_USART_BASE, &byte);
	return byte;
}

/**
 * Put a byte to console
 * \param c The byte to put
 */
void dbg_usart_putchar(uint8_t c)
{
	usart_serial_putchar(DBG_USART_BASE, c);
}

#endif /* #ifdef DBG_USE_USART */

