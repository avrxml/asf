/**
 * \file
 *
 * \brief USART driver extension for AVR XMEGA E.
 *
 * This file contains XMEGA E specific add on features.
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#ifndef _USART_XMEGAE_H_
#define _USART_XMEGAE_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup usart_group
 * \defgroup usart_xmegae_group USART XMEGA E specific driver extension
 * This group provides functions to access specific XMEGA E USART modules
 * features:
 * - Start frame detection
 * - OneWire mode
 * - Handle variable DataLenght frames with XCL PEC Timers (see \ref
 * xcl_tc_group)
 * - Handle custom transmission encoding/decoding using XCL LUTs (see \ref
 * xcl_gl_group)
 *
 * The main functions to configure the USART with XMEGA E specific features are:
 * - usart_xmegae_init_rs232()
 * - usart_xmegae_init_spi()
 *
 *
 * @{
 */

#include "compiler.h"
#include "pmic.h"
#include "sleepmgr.h"
#include "xcl.h"
#include "ioport.h"
#include "usart.h"
#include "status_codes.h"

/* ! Input parameters when initializing XMEGA E usart module */
typedef struct usart_xmegae_rs232_options {
	/* ! Set baud rate of the USART (unused in slave modes). */
	uint32_t baudrate;

	/* ! Number of bits to transmit as a character (5 to 9). */
	USART_CHSIZE_t charlength;

	/* ! Parity type: USART_PMODE_DISABLED_gc, USART_PMODE_EVEN_gc, */
	/* ! USART_PMODE_ODD_gc. */
	USART_PMODE_t paritytype;

	/* ! Number of stop bits between two characters: */
	/* ! true: 2 stop bits */
	/* ! false: 1 stop bit */
	bool stopbits;

	/* ! Start Frame detection activation */
	bool start_frame_detection;

	/* ! One Wire mode activation */
	bool one_wire;

	/* ! USART to use PEC counter for RX, TX or both */
	USART_PECACT_t pec_action;

	/* ! Number of bits to transmit as a character using pec if null PEC is
	 * unused */
	uint8_t pec_length;

	/* ! Encoding/Decoding fields type in USART frame */
	USART_DECTYPE_t encoding_type;

	/* ! Encoding/Decoding XCL LUT are used for RX, TX, both or none */
	USART_LUTACT_t encoding_stream;
} usart_xmegae_rs232_options_t;

/* ! Input parameters when initializing SPI master mode. */
typedef struct usart_xmegae_spi_options {
	/* ! Set baud rate of the USART in SPI mode. */
	uint32_t baudrate;

	/* ! SPI transmission mode. */
	uint8_t spimode;

	/* ! SPI data order transmission mode (MSB or LSB first. */
	uint8_t data_order;

	/* ! USART to use PEC counter for RX, TX or both */
	USART_PECACT_t pec_action;

	/* ! Number of bits to transmit as a character using pec if null PEC is
	 * unused */
	uint8_t pec_length;

	/* ! Encoding/Decoding fields type in USART frame */
	USART_DECTYPE_t encoding_type;

	/* ! Encoding/Decoding XCL LUT are used for RX, TX, both or none */
	USART_LUTACT_t encoding_stream;
} usart_xmegae_spi_options_t;

/**
 * \brief Enable One wire mode.
 *
 * \param usart Pointer to the USART module.
 * \param onewire True to enable one wire mode.
 */
static inline void usart_onewire(USART_t *usart, bool onewire)
{
	if (onewire) {
		(usart)->CTRLB |= USART_ONEWIRE_bm;
	} else {
		(usart)->CTRLB &= ~USART_ONEWIRE_bm;
	}
}

/**
 * \brief Enable Start Frame detection.
 *
 * \param usart Pointer to the USART module.
 * \param sfden True to enable start frame detection.
 */
static inline void usart_sfden(USART_t *usart, bool sfden)
{
	if (sfden) {
		(usart)->CTRLB |= USART_SFDEN_bm;
	} else {
		(usart)->CTRLB &= ~USART_SFDEN_bm;
	}
}

/**
 * \brief Configure the USART encoding/decoding feature through XCL module
 *
 * \param usart Pointer to the USART module.
 * \param encoding_type Type of encoding.
 * \param encoding_stream The stream to be proceed by encoding/decoding.
 */
static inline void usart_xmegae_encoding(USART_t *usart,
		USART_DECTYPE_t encoding_type,
		USART_LUTACT_t encoding_stream)
{
	usart->CTRLD &= ~USART_DECTYPE_gm;
	usart->CTRLD &= ~USART_LUTACT_gm;
	usart->CTRLD |= (uint8_t)encoding_type | (uint8_t)encoding_stream;
}

/**
 * \brief Configure which XCL timer to be use to handle frame lenght on USART
 *
 * \param usart Pointer to the USART module.
 * \param pec_action The XCL timer to be used.
 */
static inline void usart_xmegae_pec_action(USART_t *usart,
		USART_PECACT_t pec_action)
{
	usart->CTRLD &= ~USART_PECACT_gm;
	usart->CTRLD |= (uint8_t)pec_action;
}

/**
 * \brief Enable Start of frame detection interrupt
 *
 * \param usart Pointer to the USART module.
 */
static inline void usart_xmegae_enable_rxsif(USART_t *usart)
{
	usart->CTRLA |= USART_RXSIE_bm;
}

/**
 * \brief Disable start of frame detection interrupt
 *
 * \param usart Pointer to the USART module.
 */
static inline void usart_xmegae_disable_rxsif(USART_t *usart)
{
	usart->CTRLA &= ~USART_RXSIE_bm;
}

/**
 * \brief Clear start of frame detection interrupt flag
 *
 * \param usart Pointer to the USART module.
 */
static inline void usart_xmegae_clear_rxsif(USART_t *usart)
{
	usart->CTRLA = USART_RXSIF_bm;
}

/**
 * \brief Checks if the start of frame interrupt flag is set.
 *
 * Checks if the start of frame interrupt flag is set..
 *
 * \param usart The USART module.
 */
static inline bool usart_start_frame_is_detected(USART_t *usart)
{
	return (usart)->STATUS & USART_RXSIF_bm;
}

bool usart_xmegae_init_rs232(USART_t *usart,
		const usart_xmegae_rs232_options_t *opt);
void usart_xmegae_init_spi(USART_t *usart,
		const usart_xmegae_spi_options_t *opt);
enum status_code usart_putchar_onewire(USART_t *usart, uint8_t c);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _USART_XMEGAE_H_ */
