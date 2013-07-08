/**
 * \file
 *
 * \brief WDT driver for WDT module from revision 4.0.0 for AVR32 UC3.
 *
 * Copyright (c) 2010-2012 Atmel Corporation. All rights reserved.
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
#ifndef _WDT_H_
#define _WDT_H_

/**
 * \defgroup group_avr32_drivers_wdt WDT - Watchdog Timer
 *
 * Driver for the Watchdog Timer. Provides functions for selecting the mode, setting the time-out period, and enabling/disabling the Watchdog Timer.
 *
 * \{
 */

// These defines are missing from or wrong in the toolchain header file ip_xxx.h or part.h
#if !defined(AVR32_SCIF_OSC32_FREQUENCY)
#define AVR32_SCIF_OSC32_FREQUENCY         32768
#else
#warning "Duplicate define(s) to remove from the ASF"
#endif
// These defines are missing from or wrong in the toolchain header file ip_xxx.h or part.h
#if (defined AVR32_WDT_401_H_INCLUDED ) || (defined AVR32_WDT_410_H_INCLUDED )
#define AVR32_WDT_KEY_VALUE                            0x00000055
#endif

//! WDT Clock Source Select RCOSC
#define WDT_CLOCK_SOURCE_SELECT_RCSYS           0x0

//! WDT Clock Source Select 32K Oscillator
#define WDT_CLOCK_SOURCE_SELECT_OSC32K          0x1

//! WDT Basic Mode
#define WDT_BASIC_MODE                          0x0

//! WDT Window Mode
#define WDT_WINDOW_MODE                         0x1

//! WDT startup options.
typedef struct
{
	//! Prescaler value for the timeout.
	uint64_t   us_timeout_period;
#if AVR32_WDT_H_VERSION >= 400
	//! Prescaler value for the timeban.
	uint64_t   us_timeban_period;
	//! Clock Source Select.
	uint8_t   cssel;
	//! Flash Calibration Done.
	uint8_t   fcd;
	//! Store final Value.
	uint8_t   sfv;
	//! Mode selection for the WDT
	uint8_t   mode;
	//! Disable WDT after reset
	uint8_t   dar;
#endif
} wdt_opt_t;

/**
 * \brief Gets the time-out period of the WatchDog Timer in microseconds.
 *
 * \param opt Structure for settings of WDT.
 *
 * \retval <0   The WatchDog Timer is disabled.
 * \retval >=0  Active time-out period of the WatchDog Timer in microseconds.
 */
int64_t wdt_get_us_timeout_period(wdt_opt_t *opt);

// Only available for WDT versions above 400
#if AVR32_WDT_H_VERSION >= 400
/**
 * \brief Gets the timeban period of the WatchDog Timer in microseconds.
 *
 * \param opt Structure for settings of WDT.
 *
 * \retval <0   The WatchDog Timer is disabled.
 * \retval >=0  Active timeban period of the WatchDog Timer in microseconds.
 */
int64_t wdt_get_us_timeban_period(wdt_opt_t *opt);
#endif

/**
 * \brief Disables the WatchDog Timer.
 */
void wdt_disable(void);

/**
 * \brief Enables the WatchDog Timer with the \a us_timeout_period time-out
 *         period saturated to the supported range and rounded up to the nearest
 *         supported greater time-out period.
 *
 * \param opt Structure for settings of WDT.
 *
 * \return Actually configured time-out period in microseconds.
 */
uint64_t wdt_enable(wdt_opt_t *opt);

/**
 * \brief Re-enables the WatchDog Timer with the last time-out period
 *        configured.
 */
void wdt_reenable(void);

/**
 * \brief Clears the WatchDog Timer.
 */
void wdt_clear(void);

/**
 * \brief Resets the MCU with the WatchDog Timer as fast as possible.
 */
void wdt_reset_mcu(void);

/**
 * \}
 */

#endif  // _WDT_H_
