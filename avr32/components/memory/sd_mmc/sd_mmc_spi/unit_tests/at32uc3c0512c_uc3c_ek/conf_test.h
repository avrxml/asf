/*****************************************************************************
 *
 * \file
 *
 * \brief Unit test configuration
 *
 ******************************************************************************/

/**
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
#ifndef CONF_TEST_H
#define CONF_TEST_H

#include "board.h"
#include "sd_mmc_spi.h"
#include "conf_sd_mmc_spi.h"

//! \note Using USART
#define CONF_TEST_USART      USART
//! \note 57600 baud
#define CONF_TEST_BAUDRATE   57600
//! \note 8-bit character length
#define CONF_TEST_CHARLENGTH 8
//! \note No parity check
#define CONF_TEST_PARITY     USART_NO_PARITY
//! \note No extra stopbit, i.e., use 1
#define CONF_TEST_STOPBITS   USART_1_STOPBIT
//! Number of sectors to be read during the multiple sectors access test
#define CONF_TEST_NB_SECTORS 10

/*! \brief Initializes SD/MMC resources: SPI and SD/MMC.
 */
static void sd_mmc_spi_board_init(void)
{
	// SPI options.
	spi_options_t spi_options = {
		.reg         = SD_MMC_SPI_NPCS,
		.baudrate    = SD_MMC_SPI_MASTER_SPEED,
		.bits        = SD_MMC_SPI_BITS,
		.spck_delay  = 0,
		.trans_delay = 0,
		.stay_act    = 1,
		.spi_mode    = 0,
		.modfdis     = 1
	};

	// Initialize as master.
	spi_initMaster(SD_MMC_SPI, &spi_options);

	// Set SPI selection mode: variable_ps, pcs_decode, delay.
	spi_selectionMode(SD_MMC_SPI, 0, 0, 0);

	// Enable SPI module.
	spi_enable(SD_MMC_SPI);

	// Initialize SD/MMC driver with SPI clock (PBA).
	sd_mmc_spi_init(spi_options, sysclk_get_pba_hz());
}

#endif
