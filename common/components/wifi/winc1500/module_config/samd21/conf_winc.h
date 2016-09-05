/**
 *
 * \file
 *
 * \brief WINC1500 configuration.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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

#ifndef CONF_WINC_H_INCLUDED
#define CONF_WINC_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"

#if BOARD == SAMW25_XPLAINED_PRO

/* This configuration files comes with empty settings! */
/* Default settings for SAMW25 Xplained Pro with WINC on EXT1 are */
/* available in comments for reference. */
#warning Please modify conf_winc.h for WINC module settings!

/*
   ---------------------------------
   ---------- PIN settings ---------
   ---------------------------------
*/

#define CONF_WINC_PIN_RESET				0 /* PIN_PA27 */
#define CONF_WINC_PIN_CHIP_ENABLE		0 /* PIN_PA28 */
#define CONF_WINC_PIN_WAKE				0 /* PIN_PB08 */

/*
   ---------------------------------
   ---------- SPI settings ---------
   ---------------------------------
*/

#define CONF_WINC_USE_SPI				(1)

/** SPI pin and instance settings. */
#define CONF_WINC_SPI_MODULE			0 /* SERCOM2 */
#define CONF_WINC_SPI_SERCOM_MUX		0 /* SPI_SIGNAL_MUX_SETTING_D */
#define CONF_WINC_SPI_PINMUX_PAD0		0 /* PINMUX_PA12C_SERCOM2_PAD0 */ /* out */
#define CONF_WINC_SPI_PINMUX_PAD1		0 /* PINMUX_PA13C_SERCOM2_PAD1 */ /* sck  */
#define CONF_WINC_SPI_PINMUX_PAD2		0 /* PINMUX_UNUSED */ /* cs driven from software */
#define CONF_WINC_SPI_PINMUX_PAD3		0 /* PINMUX_PA15C_SERCOM2_PAD3 */ /* in  */
#define CONF_WINC_SPI_CS_PIN			0 /* PIN_PA14 */

/** SPI interrupt pin. */
#define CONF_WINC_SPI_INT_PIN			0 /* PIN_PB09A_EIC_EXTINT9 */
#define CONF_WINC_SPI_INT_MUX			0 /* MUX_PB09A_EIC_EXTINT9 */
#define CONF_WINC_SPI_INT_EIC			0 /* (9) */

/** SPI clock. */
#define CONF_WINC_SPI_CLOCK				(12000000)

/*
   ---------------------------------
   --------- Debug Options ---------
   ---------------------------------
*/

#define CONF_WINC_DEBUG					(1)
#define CONF_WINC_PRINTF				printf



#else /* BOARD == SAMD21_XPLAINED_PRO */

/* This configuration files comes with empty settings! */
/* Default settings for SAMD21 Xplained Pro with WINC on EXT1 are */
/* available in comments for reference. */
#warning Please modify conf_winc.h for WINC module settings!

/*
   ---------------------------------
   ---------- PIN settings ---------
   ---------------------------------
*/

#define CONF_WINC_PIN_RESET				0 /* PIN_PB06 */
#define CONF_WINC_PIN_CHIP_ENABLE		0 /* PIN_PB05 */
#define CONF_WINC_PIN_WAKE				0 /* PIN_PB07 */

/*
   ---------------------------------
   ---------- SPI settings ---------
   ---------------------------------
*/

#define CONF_WINC_USE_SPI				(1)

/** SPI pin and instance settings. */
#define CONF_WINC_SPI_MODULE			0 /* SERCOM1 */
#define CONF_WINC_SPI_SERCOM_MUX		0 /* SPI_SIGNAL_MUX_SETTING_E */
#define CONF_WINC_SPI_PINMUX_PAD0		0 /* PINMUX_PA16C_SERCOM1_PAD0 */ /* in */
#define CONF_WINC_SPI_PINMUX_PAD1		0 /* PINMUX_PA16C_SERCOM1_PAD1 */ /* cs driven from software */
#define CONF_WINC_SPI_PINMUX_PAD2		0 /* PINMUX_PA16C_SERCOM1_PAD2 */ /* out */
#define CONF_WINC_SPI_PINMUX_PAD3		0 /* PINMUX_PA16C_SERCOM1_PAD3 */ /* sck */
#define CONF_WINC_SPI_CS_PIN			0 /* PIN_PA17 */

/** SPI interrupt pin. */
#define CONF_WINC_SPI_INT_PIN			0 /* PIN_PA20A_EIC_EXTINT4 */
#define CONF_WINC_SPI_INT_MUX			0 /* PINMUX_PA20A_EIC_EXTINT4 */
#define CONF_WINC_SPI_INT_EIC			0 /* (4) */

/** SPI clock. */
#define CONF_WINC_SPI_CLOCK				(12000000)

/*
   ---------------------------------
   --------- Debug Options ---------
   ---------------------------------
*/

#define CONF_WINC_DEBUG					(1)
#define CONF_WINC_PRINTF				printf

#endif

#ifdef __cplusplus
}
#endif

#endif /* CONF_WINC_H_INCLUDED */
