/**
 * \file
 *
 * \brief WDT driver for AVR32 UC3.
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
