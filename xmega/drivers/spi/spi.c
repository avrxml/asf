/*****************************************************************************
 *
 * \file
 *
 * \brief SPI software driver functions.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include "spi.h"

/*! \brief Calculates the SPI baudrate divider.
 *
 * \param baudrate The expected baudrate on the SPI.
 * \param clkper_hz  SPI module input clock frequency (Peripheral clock, Hz).
 * \param spi The SPI module address
 *
 * \return Status of operation.
 *   \retval >=0  Success.
 *   \retval  <0  Error.
 */
int8_t spi_xmega_set_baud_div(SPI_t *spi, uint32_t baudrate, uint32_t clkper_hz)
{
	uint32_t divisor;
	uint8_t divisor_8bit;
	uint8_t ctrl;

	// Sanity check, requested baudrate is lower than system clock
	Assert(clkper_hz > baudrate);

	/*
	 * Get wanted divisor rounded up so we don't get speed higher than
	 * requested baudrate.
	 */
	divisor = (clkper_hz + baudrate - 1) / baudrate;

	if (divisor > 128) {
		/*
		 * Highest possible divisor is 128 so fail since we can't get
		 * low enough baudrate.
		 */
		return -1;
	}

	/*
	 * We now know that the divisor is 128 or lower so move it into a 8-bit
	 * variable to make sure the following comparison is more optimized.
	 */
	divisor_8bit = divisor;

	/*
	 * For divisor values between the possible ones round up to the closest
	 * higher one to avoid higher baudrate than requested.
	 */
	if (divisor_8bit > 64) {
		ctrl = SPI_PRESCALER_DIV128_gc;
	}
	else if (divisor_8bit > 32) {
		ctrl = SPI_PRESCALER_DIV64_gc;
	}
	else if (divisor_8bit > 16) {
		ctrl = SPI_CLK2X_bm | SPI_PRESCALER_DIV64_gc;
	}
	else if (divisor_8bit > 8) {
		ctrl = SPI_PRESCALER_DIV16_gc;
	}
	else if (divisor_8bit > 4) {
		ctrl = SPI_CLK2X_bm | SPI_PRESCALER_DIV16_gc;
	}
	else if (divisor_8bit > 2) {
		ctrl = SPI_PRESCALER_DIV4_gc;
	}
	else {
		ctrl = SPI_CLK2X_bm | SPI_PRESCALER_DIV4_gc;
	}

	// Update register and make sure to clear out any leftover bits
	spi->CTRL = (spi->CTRL & ~(SPI_CLK2X_bm | SPI_PRESCALER_gm)) | ctrl;

	return 1;
}
