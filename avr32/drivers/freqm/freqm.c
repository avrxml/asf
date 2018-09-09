/**
 * \file
 *
 * \brief AVR UC3 Frequency Meter (FREQM) driver
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
#include <avr32/io.h>
#include "freqm.h"

/**
 * \brief FREQM Configuration
 *
 * \param ref_clk Select the reference clock
 * \param msr_clk Select the clock to be measured
 * \param duration Set the duration of a measurement
 *
 * \retval ERR_TIMEOUT Time out error
 * \retval STATUS_OK Successful operation
 */
status_code_t freqm_write_config(uint32_t ref_clk, uint32_t msr_clk,
		uint8_t duration)
{
	uint32_t timeout = FREQM_NUM_OF_ATTEMPTS;
	volatile avr32_freqm_t *freqm = &AVR32_FREQM;

	// The reference clock periods can't be zero
	Assert(duration != 0);
	// Disable the former reference clock
	freqm->mode &= (~AVR32_FREQM_REFCEN_MASK);
	// Wait until the RCLKBUSY bit reads as zero
	while (freqm->status & AVR32_FREQM_RCLKBUSY_MASK) {
		if (!timeout--) {
			return ERR_TIMEOUT;
		}
	}
	freqm->mode &= (~AVR32_FREQM_REFSEL_MASK);
	// Set reference clock
	freqm->mode |= (ref_clk & AVR32_FREQM_REFSEL_MASK);
	// Enable reference clock
	freqm->mode |= AVR32_FREQM_REFCEN_MASK;

	timeout = FREQM_NUM_OF_ATTEMPTS;
	// Wait until the RCLKBUSY bit reads as zero
	while (freqm->status & AVR32_FREQM_RCLKBUSY_MASK) {
		if (!timeout--) {
			return ERR_TIMEOUT;
		}
	}

	freqm->mode &= (~AVR32_FREQM_CLKSEL_MASK);
	// Set the clock to be measured
	freqm->mode |= ((msr_clk << AVR32_FREQM_CLKSEL_OFFSET)
			& AVR32_FREQM_CLKSEL_MASK);
	freqm->mode &= (~AVR32_FREQM_REFNUM_MASK);
	// Set reference clock period
	freqm->mode |= (duration << AVR32_FREQM_REFNUM_OFFSET);
	return STATUS_OK;
}

/**
 * \brief Get result from measurement
 *
 * \param result Measurement result
 * \param ref_clk_hz reference clock frequency (Hz)
 *
 * \retval ERR_TIMEOUT Time out error
 * \retval STATUS_OK Successful operation
 */
status_code_t freqm_get_result(uint32_t *result, uint32_t ref_clk_hz)
{
	uint32_t refnum;
	float value;
	volatile avr32_freqm_t *freqm = &AVR32_FREQM;
	uint32_t timeout = FREQM_NUM_OF_ATTEMPTS;
	// Wait until the measurement is done
	while (freqm->status & (1 << AVR32_FREQM_BUSY)) {
		if (!timeout--) {
			return ERR_TIMEOUT;
		}
	}
	// Get the reference clock period
	refnum = (freqm->mode & AVR32_FREQM_REFNUM_MASK)
		>> AVR32_FREQM_REFNUM_OFFSET;
	if(refnum == 0)
		return ERR_TIMEOUT;
	// Get the measurement result
	value = freqm->value;
	// Calculate the result
	*result = (uint32_t)((value/refnum) * ref_clk_hz);
	return STATUS_OK;
}

