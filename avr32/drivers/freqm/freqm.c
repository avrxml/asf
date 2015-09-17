/**
 * \file
 *
 * \brief AVR UC3 Frequency Meter (FREQM) driver
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

