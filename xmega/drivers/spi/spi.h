/*****************************************************************************
 *
 * \file
 *
 * \brief SPI driver for AVR.
 *
 * This file defines a useful set of functions for the SPI interface on AVR
 * devices.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _SPI_H_
#define _SPI_H_

/**
 * \defgroup group_xmega_drivers_spi SPI - Serial Peripheral Interface
 *
 * Driver for the Serial Peripheral Interface (SPI).
 * Provides functions for configuring and using the SPI.
 *
 * \{
 */

#include "compiler.h"
#include "status_codes.h"
#include "ioport.h"

/*! \brief Calculates the SPI baudrate divider.
 *
 * \param baudrate The expected baudrate on the SPI.
 * \param clkper_hz  SPI module input clock frequency (Peripheral clock, Hz).
 * \param spi The SPI module address
 *
 * \return Divider or error code.
 *   \retval >=0  Success.
 *   \retval  <0  Error.
 */
int8_t spi_xmega_set_baud_div(SPI_t *spi, uint32_t baudrate, uint32_t clkper_hz);

/*! \brief Enables the SPI.
 *
 * \param spi Base address of the SPI instance.
 */
static inline void spi_enable(SPI_t *spi)
{
	spi->CTRL |= SPI_ENABLE_bm;
}

/*! \brief Disables the SPI.
 *
 * Ensures that nothing is transferred while setting up buffers.
 *
 * \param spi Base address of the SPI instance.
 *
 * \warning This may cause data loss if used on a slave SPI.
 */
static inline void spi_disable(SPI_t *spi)
{
	spi->CTRL &= ~SPI_ENABLE_bm;
}

/*! \brief Tests if the SPI is enabled.
 *
 * \param spi Base address of the SPI instance.
 *
 * \return \c 1 if the SPI is enabled, otherwise \c 0.
 */
static inline bool spi_is_enabled(SPI_t *spi)
{
	return spi->CTRL & SPI_ENABLE_bm ? true : false;
}

/*! \brief Put one data byte to a SPI peripheral.
 *
 * \param spi Base address of the SPI instance.
 * \param data The data byte to be loaded
 *
 */
static inline void spi_put(SPI_t *spi, uint8_t data)
{
	spi->DATA = data;
}

/*! \brief Get one data byte to a SPI peripheral.
 *
 * \param spi Base address of the SPI instance.
 * \return The data byte
 *
 */
static inline uint8_t spi_get(SPI_t *spi)
{
	return spi->DATA;
}

/*! \brief Tests if the SPI contains a received character.
 *
 * \param spi Base address of the SPI instance.
 *
 * \return \c 1 if the SPI Receive Holding Register is full, otherwise \c 0.
 */
static inline bool spi_is_tx_ok(SPI_t *spi)
{
	return spi->STATUS & SPI_IF_bm ? true : false;
}

/*! \brief Activate SPI master mode of a SPI peripheral
 *
 * \param spi Base address of the SPI instance.
 *
 * \warning This may cause data loss if used on a slave SPI.
 */
static inline void spi_enable_master_mode(SPI_t *spi)
{
	spi->CTRL |= SPI_MASTER_bm;
}

/*! \name Part Specific SPI Driver
 */
//! @{
//! @}

/**
 * \}
 */

#endif  // _SPI_H_
