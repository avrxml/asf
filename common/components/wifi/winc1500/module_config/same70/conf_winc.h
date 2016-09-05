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

/** Default settings for SAME70 Xplained. */

/*
   ---------------------------------
   ---------- PIN settings ---------
   ---------------------------------
*/

#define CONF_WINC_PIN_RESET				IOPORT_CREATE_PIN(PIOB, 3)
#define CONF_WINC_PIN_CHIP_ENABLE		IOPORT_CREATE_PIN(PIOC, 17)
#define CONF_WINC_PIN_WAKE				IOPORT_CREATE_PIN(PIOB, 2)

/*
   ---------------------------------
   ---------- SPI settings ---------
   ---------------------------------
*/

#define CONF_WINC_USE_SPI				(1)

/** SPI pin and instance settings. */
#define CONF_WINC_SPI					SPI0
#define CONF_WINC_SPI_ID				ID_SPI0
#define CONF_WINC_SPI_MISO_GPIO			PIO_PD20_IDX
#define CONF_WINC_SPI_MISO_FLAGS		IOPORT_MODE_MUX_B
#define CONF_WINC_SPI_MOSI_GPIO			PIO_PD21_IDX
#define CONF_WINC_SPI_MOSI_FLAGS		IOPORT_MODE_MUX_B
#define CONF_WINC_SPI_CLK_GPIO			PIO_PD22_IDX
#define CONF_WINC_SPI_CLK_FLAGS			IOPORT_MODE_MUX_B
#define CONF_WINC_SPI_CS_GPIO			PIO_PD25_IDX
#define CONF_WINC_SPI_CS_FLAGS			IOPORT_MODE_MUX_B
#define CONF_WINC_SPI_NPCS				(1)

/** SPI delay before SPCK and between consecutive transfer. */
#define CONF_WINC_SPI_DLYBS				(0)
#define CONF_WINC_SPI_DLYBCT 			(0)

/** SPI interrupt pin. */
#define CONF_WINC_SPI_INT_PIN			IOPORT_CREATE_PIN(PIOD, 28)
#define CONF_WINC_SPI_INT_PIO			PIOD
#define CONF_WINC_SPI_INT_PIO_ID		ID_PIOD
#define CONF_WINC_SPI_INT_MASK			PIO_PD28
#define CONF_WINC_SPI_INT_PRIORITY		(0)

/** Clock polarity & phase. */
#define CONF_WINC_SPI_POL				(0)
#define CONF_WINC_SPI_PHA				(1)

/** SPI clock. */
#define CONF_WINC_SPI_CLOCK				(48000000)

/*
   ---------------------------------
   --------- Debug Options ---------
   ---------------------------------
*/

#define CONF_WINC_DEBUG					(1)
#define CONF_WINC_PRINTF				printf

#ifdef __cplusplus
}
#endif

#endif /* CONF_WINC_H_INCLUDED */
