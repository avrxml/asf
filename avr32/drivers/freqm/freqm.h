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

#ifndef _FREQM_H_
#define _FREQM_H_

#include <avr32/io.h>
#include "compiler.h"
#include "status_codes.h"
#include "sysclk.h"
/**
 * \defgroup group_avr32_drivers_freqm Frequency Meter (FREQM)
 *
 * This is a driver for configuration, enabling, disabling and use of the
 * on-chip UC3 Frequency Meter (FREQM) controller.
 *
 * @{
 */

//! Time out value (number of attempts).
#define FREQM_NUM_OF_ATTEMPTS                  10000

/**
 * \brief FREQM Enable
 * Enable clock for the FREQM bus interface,depended on Power
 * Manager
 */
static inline void freqm_enable(void)
{
	// Enable clock for the FREQM bus interface
	sysclk_enable_peripheral_clock((const volatile void *)AVR32_FREQM_ADDRESS);
}

static inline bool freqm_is_busy(void);

/**
 * \brief FREQM disable
 * disable clock for the FREQM bus interface,depended on Power
 * Manager
 *
 * \note Check if freqm is busy when disable it to avoid freezing
 */
static inline void freqm_disable(void)
{
	uint32_t timeout = FREQM_NUM_OF_ATTEMPTS;
	//check if freqm is ongoing
	while(freqm_is_busy()) {
		if (!timeout--) {
			return;
		}
	}
	// Disable clock for the FREQM bus interface
	sysclk_disable_peripheral_clock((const volatile void *)AVR32_FREQM_ADDRESS);
}

/**
 * \brief Start a measurement
 *
 */
static inline void freqm_start(void)
{
	volatile avr32_freqm_t *freqm = &AVR32_FREQM;
	freqm->ctrl |= AVR32_FREQM_START_MASK;
}

status_code_t freqm_write_config(uint32_t ref_clk, uint32_t msr_clk,
		uint8_t duration);

status_code_t freqm_get_result(uint32_t *result, uint32_t ref_clk_hz);

/**
 * \brief Set clock source to be measured
 *
 * \param msr_clk Clock source number
 */
static inline void freqm_set_clock_source(uint32_t msr_clk)
{
	volatile avr32_freqm_t *freqm = &AVR32_FREQM;
	freqm->mode &= (~AVR32_FREQM_CLKSEL_MASK);
	// Set the clock to be measured
	freqm->mode |= ((msr_clk << AVR32_FREQM_CLKSEL_OFFSET)
			& AVR32_FREQM_CLKSEL_MASK);
}

/**
 * \brief Check if reference clock is busy
 */
static inline bool freqm_is_refclk_busy(void)
{
	volatile avr32_freqm_t *freqm = &AVR32_FREQM;
	return ((freqm->status & AVR32_FREQM_RCLKBUSY_MASK) == 1);
}

/**
 * \brief Check if freqm measuring is ongoing
 */
static inline bool freqm_is_busy(void)
{
	volatile avr32_freqm_t *freqm = &AVR32_FREQM;
	return ((freqm->status & AVR32_FREQM_BUSY_MASK) == 1);
}

/**
 * \brief Measurement done interrupt enable
 */
static inline void freqm_enable_measurement_done_int(void)
{
	volatile avr32_freqm_t *freqm = &AVR32_FREQM;
	irqflags_t flags;
	flags = cpu_irq_save();
	freqm->ier |= AVR32_FREQM_IER_DONE_MASK;
	cpu_irq_restore(flags);
}

/**
 * \brief Reference clock ready interrupt enable
 */
static inline void freqm_enable_refclk_ready_int(void)
{
	volatile avr32_freqm_t *freqm = &AVR32_FREQM;
	irqflags_t flags;
	flags = cpu_irq_save();
	freqm->ier |= AVR32_FREQM_IER_RCLKRDY_MASK;
	cpu_irq_restore(flags);
}

/**
 * \brief Measurement done interrupt disable
 */
static inline void freqm_disable_measurement_done_int(void)
{
	volatile avr32_freqm_t *freqm = &AVR32_FREQM;
	freqm->idr |= AVR32_FREQM_IDR_DONE_MASK;
}

/**
 * \brief Reference clock ready interrupt disable
 */
static inline void freqm_disable_refclk_ready_int(void)
{
	volatile avr32_freqm_t *freqm = &AVR32_FREQM;
	freqm->idr |= AVR32_FREQM_IDR_RCLKRDY_MASK;
}

/**
 * \brief Measurement done interrupt clear
 */
static inline void freqm_clear_measurement_done_int(void)
{
	volatile avr32_freqm_t *freqm = &AVR32_FREQM;
	freqm->icr |= AVR32_FREQM_ICR_DONE_MASK;
}

/**
 * \brief Reference clock ready interrupt clear
 */
static inline void freqm_clear_refclk_ready_int(void)
{
	volatile avr32_freqm_t *freqm = &AVR32_FREQM;
	freqm->icr |= AVR32_FREQM_ICR_RCLKRDY_MASK;
}

/**
 * \brief check if measurement done interrupt happened
 *
 * \retval true Freqm is done interrupt
 * \retval false Freqm is not done interrupt
 */
static inline bool freqm_is_measurement_done_int(void)
{
	volatile avr32_freqm_t *freqm = &AVR32_FREQM;
	return  ((freqm->isr & AVR32_FREQM_ISR_DONE_MASK) == 1);
}

/**
 * \brief check if refclk ready interrupt happened
 *
 * \retval true Freqm is reference ready interrupt
 * \retval false Freqm is not reference ready interrupt
 */
static inline bool freqm_is_refclk_ready_int(void)
{
	volatile avr32_freqm_t *freqm = &AVR32_FREQM;
	return  ((freqm->isr & AVR32_FREQM_ISR_RCLKRDY_MASK) == 1);
}

//! @}

#endif  // _FREQM_H_
