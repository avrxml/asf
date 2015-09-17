/**
 * \file
 *
 * \brief WDT driver for AVR32 UC3.
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
#include "compiler.h"
#include "wdt.h"

#define MIN_US_TIMEOUT_PERIOD   (((1ULL <<  1                             ) * 1000000 + AVR32_PM_RCOSC_FREQUENCY / 2) / AVR32_PM_RCOSC_FREQUENCY)
#define MAX_US_TIMEOUT_PERIOD   (((1ULL << (1 << AVR32_WDT_CTRL_PSEL_SIZE)) * 1000000 + AVR32_PM_RCOSC_FREQUENCY / 2) / AVR32_PM_RCOSC_FREQUENCY)

/**
 * \brief Sets the WatchDog Timer Control register to the \a ctrl value thanks
 *        to the WatchDog Timer key.
 *
 * \param ctrl  Value to set the WatchDog Timer Control register to.
 *
 * \note The KEY bit-field of \a ctrl is assumed to be zero.
 */
static void wdt_set_ctrl(uint32_t ctrl)
{
	AVR32_WDT.ctrl = ctrl | (AVR32_WDT_KEY_VALUE << AVR32_WDT_CTRL_KEY_OFFSET);
	AVR32_WDT.ctrl = ctrl | ((uint32_t) (~AVR32_WDT_KEY_VALUE << AVR32_WDT_CTRL_KEY_OFFSET) & AVR32_WDT_CTRL_KEY_MASK);
}

int64_t wdt_get_us_timeout_period(wdt_opt_t *opt)
{
	// Read CTRL.PSEL and translate it into us.
	return (AVR32_WDT.ctrl & AVR32_WDT_CTRL_EN_MASK) ?
			((1ULL << (((AVR32_WDT.ctrl & AVR32_WDT_CTRL_PSEL_MASK) >> AVR32_WDT_CTRL_PSEL_OFFSET) + 1)) *
			1000000 + AVR32_PM_RCOSC_FREQUENCY / 2) / AVR32_PM_RCOSC_FREQUENCY : -1ULL;
}

void wdt_disable(void)
{
	wdt_set_ctrl(AVR32_WDT.ctrl & ~AVR32_WDT_CTRL_EN_MASK);
}

uint64_t wdt_enable(wdt_opt_t *opt)
{
	// Set the CTRL.EN bit and translate the us timeout to fit in CTRL.PSEL using
	// the formula Twdt = 2pow(PSEL+1) / fRCosc
	wdt_set_ctrl(AVR32_WDT_CTRL_EN_MASK |
			((32 - clz(((((Min(Max(opt->us_timeout_period, MIN_US_TIMEOUT_PERIOD), MAX_US_TIMEOUT_PERIOD) *
			AVR32_PM_RCOSC_FREQUENCY + 500000) / 1000000) << 1) - 1) >> 1) - 1) << AVR32_WDT_CTRL_PSEL_OFFSET));
	// Return the actual wdt period in us.
	return wdt_get_us_timeout_period(opt);
}

void wdt_reenable(void)
{
	wdt_set_ctrl(AVR32_WDT.ctrl | AVR32_WDT_CTRL_EN_MASK);
}

void wdt_clear(void)
{
	AVR32_WDT.clr = 0;
}

void wdt_reset_mcu(void)
{
	cpu_irq_disable();
	// Enable the WDT with a 0s period (fastest way to get a Watchdog reset).
	wdt_opt_t opt = {
			.us_timeout_period = 0
	};
	wdt_enable(&opt);
	while (1);
}
