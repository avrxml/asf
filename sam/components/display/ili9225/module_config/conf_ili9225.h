/**
 * \file
 *
 * \brief ILI9225 configuration.
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

//! Configuration of the ILI9225 LCD display driver

#ifndef CONF_ILI9225_H_INCLUDED
#define CONF_ILI9225_H_INCLUDED

#include "board.h"

#if !defined(BOARD_ILI9225_SPI)      || \
	!defined(BOARD_ILI9225_SPI_IRQN) || \
	!defined(BOARD_ILI9225_SPI_NPCS) || \
	!defined(BOARD_ILI9225_RS_GPIO)  || \
	!defined(BOARD_ILI9225_RSTN_GPIO)

	#warning The ILI9225 PIN configuration does not exist in the board definition file. Using default settings.

	#define BOARD_ILI9225_SPI         0 /* SPI instance, which can be SPI, SPI0 or SPI1, depends on which SPI channel is used by ILI9225 */
	#define BOARD_ILI9225_SPI_IRQN    0 /* SPI interrupt number, which can be SPI_IRQn, SPI0_IRQn or SPI1_IRQn, depends on which SPI channel is used by ILI9225 */
	#define BOARD_ILI9225_SPI_NPCS    2 /* SPI chip select number, depends on which SPI CS pin is used */
	#define BOARD_ILI9225_RS_GPIO     PIO_PA28_IDX /* PIN index, depends on which pin is connnected with RS of ILI9225 */
	#define BOARD_ILI9225_RSTN_GPIO   PIO_PA29_IDX /* PIN index, depends on which pin is connnected with nRESET of ILI9225 */
#endif

#endif /* CONF_ILI9225_H_INCLUDED */
