/*****************************************************************************
 *
 * \file
 *
 * \brief Unit test configuration
 *
 ******************************************************************************/

/**
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
