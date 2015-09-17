/**
 * \file
 *
 * \brief Configuration for the ADS7843 component.
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

//! Configuration of the ADS7843 touch driver

#ifndef CONF_ADS7843_H_INCLUDED
#define CONF_ADS7843_H_INCLUDED

#include "board.h"

#if !defined(BOARD_ADS7843_SPI_NPCS) || \
	!defined(BOARD_ADS7843_IRQ_GPIO) || \
	!defined(BOARD_ADS7843_IRQ_FLAGS) || \
	!defined(BOARD_ADS7843_SPI_BASE)
	
	#warning The ADS7843 SPI configuration does not exist in the board definition file. Using default settings.

	#define BOARD_ADS7843_SPI_BASE    0 /* SPI instance, which can be SPI, SPI0 or SPI1, depends on which SPI channel is used by ADS7843. */
	#define BOARD_ADS7843_SPI_NPCS    0 /* SPI chip select number, depends on which SPI CS pin is used by ADS7843. */
	#define BOARD_ADS7843_IRQ_GPIO    PIO_PA16_IDX  /* PIN index, depends on which pin is connnected with nPENIRQ of ADS7843 */
	#define BOARD_ADS7843_IRQ_FLAGS    PIO_INPUT | PIO_PULLUP /* PIO type and attribute of the pin connected with nPENIRQ, normally should be (PIO_INPUT | PIO_PULLUP) */

#endif

#endif /* CONF_ADS7843_H_INCLUDED */
