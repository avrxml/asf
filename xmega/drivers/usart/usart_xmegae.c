/**
 * \file
 *
 * \brief USART driver extension for AVR XMEGA E.
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

#include "usart_xmegae.h"

/*
 * Fix XMEGA header files
 * USART.CTRLC  bit masks and bit positions
 */
#ifndef USART_UCPHA_bm
#  define USART_UCPHA_bm 0x02
#endif
#ifndef USART_DORD_bm
#  define USART_DORD_bm 0x04
#endif

/**
 * \brief Initialize USART in RS232 mode and similar for XMEGA E.
 *
 * This function initializes the USART module in RS232 mode using the
 * usart_rs232_options_t configuration structure and CPU frequency.
 * Compared to usart_init_rs232(), this function gives acces to XMEGA E sepcific
 * feature as one wire mode, start frame detection, data encoding/decoding and
 * variable frame lenght.
 *
 * \param usart The USART module.
 * \param opt The RS232 configuration option.
 *
 * \retval true if the initialization was successfull
 * \retval false if the initialization failed (error in baud rate calculation)
 */
bool usart_xmegae_init_rs232(USART_t *usart,
		const usart_xmegae_rs232_options_t *opt)
{
	bool result;

	sysclk_enable_peripheral_clock(usart);

	usart_set_mode(usart, USART_CMODE_ASYNCHRONOUS_gc);
	usart_format_set(usart, opt->charlength, opt->paritytype,
			opt->stopbits);
	result = usart_set_baudrate(usart, opt->baudrate, sysclk_get_per_hz());

	/* configure encoding/decoding if needed */
	if (opt->encoding_stream != USART_LUTACT_OFF_gc) {
		usart_xmegae_encoding(usart, opt->encoding_type,
				opt->encoding_stream);
	}

	usart_sfden(usart, opt->start_frame_detection);
	usart_onewire(usart, opt->one_wire);

	/* Test if XCL PEC should be used and configure it */
	if (opt->pec_length) {
		xcl_enable(XCL_SYNCHRONOUS);
#ifdef USARTC0
		if ((uint16_t)usart == (uint16_t)&USARTC0) {
			xcl_port(PC);
		}
#endif
#ifdef USARTD0
		if ((uint16_t)usart == (uint16_t)&USARTD0) {
			xcl_port(PD);
		}
#endif
		if (opt->pec_length > 16) {
			xcl_tc_type(PEC01);
		} else {
			xcl_tc_type(BTC0PEC2);
		}

		xcl_pec_set_lenght(opt->pec_length - 1);
		usart_xmegae_pec_action(usart, opt->pec_action);
	}

	if (opt->one_wire == false) {
		usart_tx_enable(usart);
	}

	usart_rx_enable(usart);

	return result;
}

/**
 * \brief Send a data with the USART module in one_wire mode
 *
 * This function outputs a data using the USART module.
 *
 * \param usart The USART module.
 * \param c The data to send.
 *
 * \return STATUS_OK
 */
enum status_code usart_putchar_onewire(USART_t *usart, uint8_t c)
{
	port_pin_t tx_pin;
	while (usart_data_register_is_empty(usart) == false) {
	}

#ifdef USARTC0
	if ((uint16_t)usart == (uint16_t)&USARTC0) {
		tx_pin = IOPORT_CREATE_PIN(PORTC, 2);
		ioport_configure_port_pin(ioport_pin_to_port(tx_pin),
				ioport_pin_to_mask(tx_pin),
				IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	}
#endif
#ifdef USARTD0
	if ((uint16_t)usart == (uint16_t)&USARTD0) {
		tx_pin = IOPORT_CREATE_PIN(PORTD, 2);
		ioport_configure_port_pin(ioport_pin_to_port(tx_pin),
				ioport_pin_to_mask(tx_pin),
				IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	}
#endif
	usart_tx_enable(usart);
	usart_rx_disable(usart);
	(usart)->DATA = c;
	while (!usart_tx_is_complete(usart)) {
	}
	usart_clear_tx_complete(usart);
	usart_rx_enable(usart);
	usart_tx_disable(usart);
	ioport_configure_port_pin(ioport_pin_to_port(tx_pin),
			ioport_pin_to_mask(tx_pin),
			IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	return STATUS_OK;
}

/**
 * \brief Initialize USART in SPI master mode for XMEGA E.
 *
 * This function initializes the USART module in SPI master mode using the
 * usart_xmegae_spi_options_t configuration structure and CPU frequency.
 * Compared to usart_init_spi(), this function gives acces to XMEGA E sepcific
 * feature as data encoding/decoding and variable frame lenght.
 *
 * \param usart The USART module.
 * \param opt The SPI configuration option.
 */
void usart_xmegae_init_spi(USART_t *usart,
		const usart_xmegae_spi_options_t *opt)
{
	ioport_pin_t sck_pin;
	bool invert_sck;

	sysclk_enable_peripheral_clock(usart);

	usart_rx_disable(usart);

	/* configure Clock polarity using INVEN bit of the correct SCK I/O port **/
	invert_sck = (opt->spimode == 2) || (opt->spimode == 3);
	UNUSED(invert_sck);

#ifdef USARTC0
	if ((uint16_t)usart == (uint16_t)&USARTC0) {
		sck_pin = IOPORT_CREATE_PIN(PORTC, 1);
	}
#endif
#ifdef USARTC1
	if ((uint16_t)usart == (uint16_t)&USARTC1) {
		sck_pin = IOPORT_CREATE_PIN(PORTC, 5);
	}
#endif
#ifdef USARTD0
	if ((uint16_t)usart == (uint16_t)&USARTD0) {
		sck_pin = IOPORT_CREATE_PIN(PORTD, 1);
	}
#endif
#ifdef USARTD1
	if ((uint16_t)usart == (uint16_t)&USARTD1) {
		sck_pin = IOPORT_CREATE_PIN(PORTD, 5);
	}
#endif
#ifdef USARTE0
	if ((uint16_t)usart == (uint16_t)&USARTE0) {
		sck_pin = IOPORT_CREATE_PIN(PORTE, 1);
	}
#endif
#ifdef USARTE1
	if ((uint16_t)usart == (uint16_t)&USARTE1) {
		sck_pin = IOPORT_CREATE_PIN(PORTE, 5);
	}
#endif
#ifdef USARTF0
	if ((uint16_t)usart == (uint16_t)&USARTF0) {
		sck_pin = IOPORT_CREATE_PIN(PORTF, 1);
	}
#endif
#ifdef USARTF1
	if ((uint16_t)usart == (uint16_t)&USARTF1) {
		sck_pin = IOPORT_CREATE_PIN(PORTF, 5);
	}
#endif

	/* Configure the USART output pin */
	ioport_set_pin_dir(sck_pin, IOPORT_DIR_OUTPUT);
	ioport_set_pin_mode(sck_pin,
			IOPORT_MODE_TOTEM | (invert_sck? IOPORT_MODE_INVERT_PIN : 0));
	ioport_set_pin_level(sck_pin, IOPORT_PIN_LEVEL_HIGH);

	usart_set_mode(usart, USART_CMODE_MSPI_gc);

	if (opt->spimode == 1 || opt->spimode == 3) {
		usart->CTRLC |= USART_UCPHA_bm;
	} else {
		usart->CTRLC &= ~USART_UCPHA_bm;
	}
	if (opt->data_order) {
		(usart)->CTRLC |= USART_DORD_bm;
	} else {
		(usart)->CTRLC &= ~USART_DORD_bm;
	}

	usart_spi_set_baudrate(usart, opt->baudrate, sysclk_get_per_hz());

	/* configure encoding/decoding if needed */
	if (opt->encoding_stream != USART_LUTACT_OFF_gc) {
		usart_xmegae_encoding(usart, opt->encoding_type,
				opt->encoding_stream);
	}

	/* Test if XCL PEC should be used and configure it */
	if (opt->pec_length) {
		xcl_enable(XCL_SYNCHRONOUS);
#ifdef USARTC0
		if ((uint16_t)usart == (uint16_t)&USARTC0) {
			xcl_port(PC);
		}
#endif
#ifdef USARTD0
		if ((uint16_t)usart == (uint16_t)&USARTD0) {
			xcl_port(PD);
		}
#endif
		if (opt->pec_length > 16) {
			xcl_tc_type(PEC01);
		} else {
			xcl_tc_type(BTC0PEC2);
		}

		xcl_pec_set_lenght(opt->pec_length - 1);
		usart_xmegae_pec_action(usart, opt->pec_action);
	}

	usart_tx_enable(usart);
	usart_rx_enable(usart);
}
