/**
 * \file
 *
 * \brief WDT driver for WDT module from revision 4.0.0 for AVR32 UC3.
 *
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
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
