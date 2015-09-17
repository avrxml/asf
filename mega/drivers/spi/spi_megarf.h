/**
 * \file
 *
 * \brief SPI driver definition for megaRF.
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

#ifndef _SPI_MEGARF_H_
#define _SPI_MEGARF_H_

#include "compiler.h"
#include "status_codes.h"
#include "ioport.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup group_megarf_drivers_spi SPI - Serial Peripheral Interface
 *
 * Driver for the Serial Peripheral Interface (SPI).
 * Provides functions for configuring and using the SPI.
 *
 * \{
 */

/* SPI - Serial Peripheral Interface */
/* SPCR  bit masks and bit positions */
#define SPI_SPIE_bm  0x80  /* Interrupt Enable bit mask. */
#define SPI_SPIE_bp  7  /* Interrupt Enable bit position. */

#define SPI_ENABLE_bm  0x40  /* Enable Module bit mask. */
#define SPI_ENABLE_bp  6  /* Enable Module bit position. */

#define SPI_DORD_bm  0x20  /* Data Order Setting bit mask. */
#define SPI_DORD_bp  5  /* Data Order Setting bit position. */

#define SPI_MASTER_bm  0x10  /* Master Operation Enable bit mask. */
#define SPI_MASTER_bp  4  /* Master Operation Enable bit position. */

#define SPI_MODE_gm  0x0C  /* SPI Mode group mask. */
#define SPI_MODE_gp  2  /* SPI Mode group position. */
#define SPI_CPOL_bm  0x08  /* SPI Clock polarity bit 1 mask. */
#define SPI_CPOL_bp  3  /* SPI Clock polarity bit 1 position. */
#define SPI_CPHA_bm  0x04  /* SPI Clock phase bit 1 mask. */
#define SPI_CPHA_bp  2  /* SPI Clock phase bit 1 position. */

#define SPI_PRESCALER_gm  0x03  /* Prescaler group mask. */
#define SPI_PRESCALER_gp  0  /* Prescaler group position. */
#define SPI_PRESCALER0_bm  (1 << 0)  /* Prescaler bit 0 mask. */
#define SPI_PRESCALER0_bp  0  /* Prescaler bit 0 position. */
#define SPI_PRESCALER1_bm  (1 << 1)  /* Prescaler bit 1 mask. */
#define SPI_PRESCALER1_bp  1  /* Prescaler bit 1 position. */

/* SPSR  bit masks and bit positions */
#define SPI_IF_bm  0x80  /* Interrupt Flag bit mask. */
#define SPI_IF_bp  7  /* Interrupt Flag bit position. */

#define SPI_WRCOL_bm  0x40  /* Write Collision bit mask. */
#define SPI_WRCOL_bp  6  /* Write Collision bit position. */

#define SPI_CLK2X_bm  0x01  /* Enable Double Speed bit mask. */
#define SPI_CLK2X_bp  0  /* Enable Double Speed bit position. */

/* Mask for double baud rate for spi_sck_clock_rate_t */
#define DOUBLE_SPEED_MASK    0x04

/* Mask for baud rate for spi_sck_clock_rate_t */
#define BAUD_RATE_MASK    0x03

/* SPI mode 0 */
#define SPI_MODE0       0
/* SPI mode 1 */
#define SPI_MODE1       (SPI_CPHA)
/* SPI mode 2 */
#define SPI_MODE2       (SPI_CPOL)
/* SPI mode 3 */
#define SPI_MODE3       (SPI_CPOL | SPI_CPHA)

/* Prescaler setting */
typedef enum SPI_PRESCALER_enum {
	SPI_PRESCALER_DIV4_gc = (0x00 << 0), /* System Clock / 4 */
	SPI_PRESCALER_DIV16_gc = (0x01 << 0), /* System Clock / 16 */
	SPI_PRESCALER_DIV64_gc = (0x02 << 0), /* System Clock / 64 */
	SPI_PRESCALER_DIV128_gc = (0x03 << 0), /* System Clock / 128 */
} SPI_PRESCALER_t;

/**
 * \brief Interrupt event callback function type
 *
 * The interrupt handler can be configured to do a function callback,
 * the callback function must match the api_int_callback_t type.
 *
 */
typedef void (*spi_int_callback_t)(void);

/**
 * \brief Calculates the SPI baudrate divider.
 *
 * \param baudrate The expected baudrate on the SPI.
 * \param clkper_hz  SPI module input clock frequency (Peripheral clock, Hz).
 * \param spi The SPI module address
 *
 * \return Status of operation.
 *   \retval >=0  Success.
 *   \retval  <0  Error.
 */
int8_t spi_set_baud_div(volatile void *spi, uint32_t baudrate,
		uint32_t clkper_hz);

/**
 * \brief Set SPI interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \param callback Reference to a callback function
 */
void spi_set_interrupt_callback(spi_int_callback_t callback);

/**
 * \brief Enables the SPI.
 *
 * \param spi Base address of the SPI instance.
 */
static inline void spi_enable(volatile void *spi)
{
	SPCR |= SPI_ENABLE_bm;
}

/**
 * \brief Disables the SPI.
 *
 * Ensures that nothing is transferred while setting up buffers.
 *
 * \param spi Base address of the SPI instance.
 *
 * \warning This may cause data loss if used on a slave SPI.
 */
static inline void spi_disable(volatile void *spi)
{
	SPCR &= ~SPI_ENABLE_bm;
}

/**
 * \brief Enables the SPI in interrupt mode.
 *
 * \param spi Base address of the SPI instance.
 */
static inline void spi_enable_interrupt(volatile void *spi)
{
	SPCR |= SPI_SPIE_bm;
}

/**
 * \brief Disables the SPI's interrupt.
 *
 * \param spi Base address of the SPI instance.
 */
static inline void spi_disable_interrupt(volatile void *spi)
{
	SPCR &= ~SPI_SPIE_bm;
}

/**
 * \brief Activate SPI master mode of a SPI peripheral
 *
 * \param spi Base address of the SPI instance.
 */
static inline void spi_enable_master_mode(volatile void *spi)
{
	SPCR |= SPI_MASTER_bm;
}

/**
 * \brief Activate SPI slave mode of a SPI peripheral
 *
 * \param spi Base address of the SPI instance.
 */
static inline void spi_enable_slave_mode(volatile void *spi)
{
	SPCR &= ~SPI_MASTER_bm;
}

/**
 * \brief Set SPI clock mode
 *
 * \param spi Base address of the SPI instance.
 * \param mode Value of the mode to be set.
 */
static inline void spi_set_clock_mode(volatile void *spi, uint8_t mode)
{
	SPCR = (SPCR & ~SPI_MODE_gm) | ((mode << SPI_MODE_gp) & SPI_MODE_gm);
}

/**
 * \brief Tests if the SPI is enabled.
 *
 * \param spi Base address of the SPI instance.
 *
 * \return \c 1 if the SPI is enabled, otherwise \c 0.
 */
static inline bool spi_is_enabled(volatile void *spi)
{
	return SPCR & SPI_ENABLE_bm ? true : false;
}

/**
 * \brief Put one data byte to SPI peripheral.
 *
 * \param spi Base address of the SPI instance.
 * \param data The data byte to be loaded
 *
 */
static inline void spi_put(volatile void *spi, uint8_t data)
{
	SPDR = data;
}

/**
 * \brief Get one data byte from SPI peripheral.
 *
 * \param spi Base address of the SPI instance.
 * \return The data byte
 *
 */
static inline uint8_t spi_get(volatile void *spi)
{
	return SPDR;
}

/**
 * \brief Tests if the SPI transfer is complete.
 *
 * \param spi Base address of the SPI instance.
 *
 * \return \c 1 if the SPI transfer is complete, otherwise \c 0.
 */
static inline bool spi_is_tx_ok(volatile void *spi)
{
	return SPSR & SPI_IF_bm ? true : false;
}

/**
 * \brief Tests if the Write collision occurs during SPI transfer.
 *
 * \param spi Base address of the SPI instance.
 *
 * \return \c 1 if the SPI Write collision occurred, otherwise \c 0.
 */
static inline bool spi_is_wc_flag_set(volatile void *spi)
{
	return SPSR & SPI_WRCOL_bm ? true : false;
}

/**
 * \brief Enable SPI Double Speed.
 *
 * \param spi Base address of the SPI instance.
 */
static inline void spi_double_speed_enable(volatile void *spi)
{
	SPSR |= SPI_CLK2X_bm;
}

/**
 * \brief Disable SPI Double Speed.
 *
 * \param spi Base address of the SPI instance.
 */
static inline void spi_double_speed_disable(volatile void *spi)
{
	SPSR &= ~SPI_CLK2X_bm;
}

/**
 * \brief Set SPI data order to LSB first.
 *
 * \param spi Base address of the SPI instance.
 */
static inline void spi_set_dord_lsb(volatile void *spi)
{
	SPCR |= SPI_DORD_bm;
}

/**
 * \brief Set SPI data order to MSB first.
 *
 * \param spi Base address of the SPI instance.
 */
static inline void spi_set_dord_msb(volatile void *spi)
{
	SPSR &= ~SPI_DORD_bm;
}

/**
 * \brief Set clock default state.
 *
 * \param spi Base address of the SPI instance.
 * \param polarity Set clock polarity as logical one(high)/zero(low).
 */
static inline void spi_set_clock_polarity(volatile void *spi, bool polarity)
{
	if (polarity) {
		SPCR |= SPI_CPOL_bm;
	} else {
		SPSR &= ~SPI_CPOL_bm;
	}
}

/**
 * \brief Set Data Capture Phase.
 *
 * \param spi Base address of the SPI instance.
 * \param polarity Set clock phase as logical one(high)/zero(low).
 */
static inline void spi_set_clock_phase(volatile void *spi, bool polarity)
{
	if (polarity) {
		SPCR |= SPI_CPHA_bm;
	} else {
		SPSR &= ~SPI_CPHA_bm;
	}
}

/* ! @} */
#ifdef __cplusplus
}
#endif
#endif  /* _SPI_MEGARF_H_ */
