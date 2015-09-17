/**
 * \file
 *
 * \brief AVR MEGARF WatchDog Timer driver Definition.
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
#ifndef _WDT_MEGARF_H_
#define _WDT_MEGARF_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <compiler.h>

/**
 * \defgroup megarf_wdt_group Watchdog Timer (WDT)
 *
 * See \ref megarf_wdt_quickstart
 *
 * This is a driver for configuring, enabling, disabling and use of the on-chip
 * WDT.
 *
 * \section dependencies Dependencies
 * This driver depends on the following modules:
 * - \ref sysclk_group for peripheral clock control.
 *
 * @{
 */

/**
 * \brief Interrupt event callback function type
 *
 * The interrupt handler can be configured to do a function callback,
 * the callback function must match the wdt_callback_t type.
 *
 */
typedef void (*wdt_callback_t)(void);

/* WDTCSR  bit masks and bit positions */
#define WDIF_bm  0x80           /* WDT timeout interrupt flag bit mask. */
#define WDIF_bp  7              /* WDT timeout interrupt flag bit position. */

#define WDIE_bm  0x40           /* WDT timeout interrupt enable bit mask. */
#define WDIE_bp  6              /* WDT timeout interrupt enable bit position. */

#define WDP3_bm  0x20           /* WDT prescaler 3 Bit mask. */
#define WDP3_bp  5              /*  WDT prescaler 3 bit position. */

#define WDCE_bm   0x10          /* WDT change enable bit mask. */
#define WDCE_bp   4             /* WDT change enable bit position. */

#define WDE_bm  0x08            /* WDT enable bit mask. */
#define WDE_bp  3               /* WDT enable bit position. */

#define WDP2_bm  0x04           /* WDT prescaler 2 Bit mask. */
#define WDP2_bp  2              /*  WDT prescaler 2 bit position. */

#define WDP1_bm  0x02           /* WDT prescaler 1 Bit mask. */
#define WDP1_bp  1              /*  WDT prescaler 1 bit position. */

#define WDP0_bm  0x01           /* WDT prescaler 0 Bit mask. */
#define WDP0_bp  0              /*  WDT prescaler 0 bit position. */

/* WDRF bit mask and bit position in MCUSR */
#define WDRF_bm  0x08           /* WDT Reset flag Bit mask. */
#define WDRF_bp  3              /*  WDT Reset flag bit position. */

/**
 * \internal
 * \def wdt_timeout_period
 * \brief Watchdog timeout period setting
 */
enum wdt_timeout_period {
	/* ! Timeout period = 2K cycles or 16ms @ 3.3V */
	WDT_TIMEOUT_PERIOD_2KCLK = (0x00),
	/* ! Timeout period = 4K cycles or 32ms @ 3.3V */
	WDT_TIMEOUT_PERIOD_4KCLK = (0x01),
	/* ! Timeout period = 8K cycles or 64ms @ 3.3V */
	WDT_TIMEOUT_PERIOD_8KCLK = (0x02),
	/* ! Timeout period = 16K cycles or 125ms @ 3.3V */
	WDT_TIMEOUT_PERIOD_16KCLK = (0x03),
	/* ! Timeout period = 32K cycles or 250ms @ 3.3V */
	WDT_TIMEOUT_PERIOD_32KCLK = (0x04),
	/* ! Timeout period = 64K cycles or 500ms @ 3.3V */
	WDT_TIMEOUT_PERIOD_64KCLK = (0x05),
	/* ! Timeout period = 128K cycles or 1s @ 3.3V */
	WDT_TIMEOUT_PERIOD_128KCLK = (0x06),
	/* ! Timeout period = 256K cycles or 2s @ 3.3V */
	WDT_TIMEOUT_PERIOD_256KCLK = (0x07),
	/* ! Timeout period = 512K cycles or 4s @ 3.3V */
	WDT_TIMEOUT_PERIOD_512KCLK = (0x08),
	/* ! Timeout period = 1024K cycles or 8s @ 3.3V */
	WDT_TIMEOUT_PERIOD_1024KCLK = (0x09),
};

/**
 * \internal
 * \def wdt_mode_select
 * \brief Watchdog mode selection settings
 */
enum wdt_mode_select {
	INTERRUPT_MODE,
	SYSTEM_RESET_MODE,
	INTERRUPT_SYSTEM_RESET_MODE,
};

/**
 * \internal
 * \def MASK_PRESCALE_WPD3
 * \brief Mask for timeout period setting above WDT_TIMEOUT_PERIOD_256KCLK
 */
#define MASK_PRESCALE_WPD3 0x08

/**
 * \internal
 * \def PRESCALE_MASK
 * \brief Mask for timeout period setting register
 */
#define PRESCALE_MASK (WDP3_bm | WDP2_bm | WDP1_bm | WDP0_bm)

/**
 * \internal
 * \def wdt_reset
 * \brief This macro resets (clears/refreshes) the Watchdog Timer.
 */
#if defined(__GNUC__)
#define wdt_reset() __asm__ __volatile__ ("wdr");
#elif defined(__ICCAVR__)
#define wdt_reset() __watchdog_reset();
#else
#error Unsupported compiler.
#endif

/**
 * *\brief Clear Watchdog reset flag.
 *
 *  This function clears the WDT flag.
 *
 ***\param  none
 */
static inline void wdt_reset_flag_clear(void)
{
	/* Clear WDRF flag in MCUSR */
	MCUSR &= ~WDRF_bm;
}

/**
 * *\brief Gets the status of Watchdog reset flag.
 *
 *  This function checks the WDT reset flag is set
 *
 *  \retval  false  WDT reset flag is not set
 *           true   reset flag is set
 */
static inline bool wdt_reset_flag_is_set(void)
{
	return ((MCUSR & WDRF_bm) == WDRF_bm);
}

/**
 * \brief Clear Watchdog timeout interrupt flag.
 *
 *  This function clears the WDT interrupt flag.
 *
 *  \param  none
 */
static inline void wdt_timeout_interrupt_flag_clear(void)
{
	/* Clear WDIF flag in WDTCSR */
	WDTCSR |= WDIF_bm;
}

/**
 * \brief Gets the status of Watchdog timeout interrupt flag.
 *
 * This function checks whether the Watchdog timeout interrupt flag
 * is set
 *
 * \return  false  WDT timeout interrupt flag is not set
 *           true   WDT timeout interrupt flag is set
 */
static inline bool wdt_timeout_interrupt_flag_is_set(void)
{
	return ((WDTCSR & WDIF_bm) == WDIF_bm);
}

/**
 * \brief checks whether the Watchdog is enabled for system reset
 * mode
 *
 * \return  false  WDT in system reset mode is disabled
 *           true   WDT in system reset mode is enabled
 */
static inline bool wdt_system_reset_mode_enabled(void)
{
	return (((WDTCSR & WDE_bm) == WDE_bm) && ((WDTCSR & WDIE_bm) == 0x00));
}

/**
 * \brief checks whether the Watchdog is enabled for interrupt mode
 *
 * \return  false  WDT in interrupt mode is disabled
 *           true   WDT in interrupt mode is enabled
 */
static inline bool wdt_interrupt_mode_enabled(void)
{
	return (((WDTCSR & WDIE_bm) == WDIE_bm) && ((WDTCSR & WDE_bm) == 0x00));
}

/**
 * \brief checks whether the Watchdog is enabled for system reset and
 * interrupt mode
 *
 * \return  false  WDT in system reset and interrupt mode is disabled
 *           true   WDT in system reset and interrupt mode is enabled
 */
static inline bool wdt_reset_interrupt_mode_enabled(void)
{
	return (((WDTCSR & WDIE_bm) == WDIE_bm) &&
	       ((WDTCSR & WDE_bm) == WDE_bm));
}

/**
 * *\brief Get Watchdog timeout period.
 *
 *  This function gets the coded field of the WDT timeout period.
 *
 ***\return WDT timeout coded period
 */
static inline enum wdt_timeout_period wdt_get_timeout_period(void)
{
	volatile uint8_t period;

	period = WDTCSR & PRESCALE_MASK;

	if (period == 0x20) {
		return WDT_TIMEOUT_PERIOD_512KCLK;
	} else if (period == 0x21) {
		return WDT_TIMEOUT_PERIOD_1024KCLK;
	} else {
		return (enum wdt_timeout_period)period;
	}
}

/**
 * \brief Disable Watchdog.
 *
 *  This function disables the WDT without changing period settings.
 *  This function is written in asm where ever the time is critical
 */
void wdt_disable(void);

/**
 * \brief Set Watchdog timeout period.
 *
 *  This function sets the coded field of the WDT timeout period.
 *  This function is written in asm where ever the time is critical
 *
 *  \param  to_period  WDT timeout period
 */
void wdt_set_timeout_period(enum wdt_timeout_period to_period);

/**
 * \brief Enable WDT at different modes
 *
 * This function allows WDT to enable. Using this function we can
 * select either Interrupt Mode or System reset mode or Interrupt
 * and System reset mode.
 * This function is written in asm where ever the time is critical
 *
 * \param mode WDT timer mode selection
 */
void wdt_enable(enum wdt_mode_select mode);

/**
 * \brief Set WDT timer overflow interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \param callback Reference to a callback function
 */
void wdt_set_interrupt_callback(wdt_callback_t callback);

/**
 * *\brief Reset MCU via Watchdog.
 *
 *  This function generates an hardware microcontroller reset using the WDT.
 *
 *  The function loads enables the WDT in system reset mode.
 */
void wdt_reset_mcu(void);

/** @} */

#ifdef __cplusplus
}
#endif

/**
 * \page megarf_wdt_quickstart Quick start guide for WDT driver
 *
 * This is the quick start guide for the \ref megarf_wdt_group, with
 * step-by-step instructions on how to configure and use the driver in a
 * selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 *
 * \section megarf_wdt_basic_use_case Basic use case
 * \section megarf_wdt_use_cases WDT use cases
 * - \ref megarf_wdt_basic_use_case
 * - \subpage megarf_wdt_use_case_1
 *
 * \section megarf_wdt_basic_use_case Basic use case - Reset WDT in system
 * reset mode
 * In this use case, the WDT is configured for:
 * - System rest mode
 * - Timeout period of 16 ms
 *
 * The use case enables the WDT, and resets it after 10 ms to prevent system
 * reset after time out period of 16 ms.
 *
 * \section megarf_wdt_basic_use_case_setup Setup steps
 *
 * \subsection megarf_wdt_basic_use_case_setup_prereq Prerequisites
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * -# \ref group_common_services_delay "Busy-Wait Delay Routines"
 *
 * \subsection megarf_wdt_basic_use_case_setup_code Example code
 * Add to application initialization:
 * \code
	wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_2KCLK);
	wdt_enable(SYSTEM_RESET_MODE);
\endcode
 *
 * \subsection megarf_wdt_basic_use_case_setup_flow Workflow
 * -# Set timeout period to 2K cycles or 16 ms:
 *   - \code wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_2KCLK); \endcode
 * -# Enable WDT:
 *   - \code wdt_enable(SYSTEM_RESET_MODE); \endcode
 * \section megarf_wdt_basic_use_case_usage Usage steps
 *
 * \subsection megarf_wdt_basic_use_case_usage_code Example code
 * Add to, e.g., main loop in application C-file:
 * \code
	delay_ms(10);
	wdt_reset();
\endcode
 *
 * \subsection megarf_wdt_basic_use_case_usage_flow Workflow
 * -# Wait for 10 ms:
 *   - \code delay_ms(10); \endcode
 * -# Reset the WDT before the timeout period is over to prevent system reset:
 *   - \code wdt_reset(); \endcode
 */

/**
 * \page megarf_wdt_use_case_1 Reset WDT in interrupt mode
 *
 * In this use case, the WDT is configured for:
 * - interrupt mode
 * - Timeout period of 32 ms
 *
 * The use case enables the WDT in interrupt mode, and resets it after 25 ms to
 * prevent wdt interrupt to get triggered after period of 32 ms.
 *
 * \section megarf_wdt_use_case_1_setup Setup steps
 *
 * \subsection megarf_wdt_use_case_1_setup_prereq Prerequisites
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * -# \ref group_common_services_delay "Busy-Wait Delay Routines"
 *
 * \subsection megarf_wdt_use_case_1_setup_code Example code
 * Add to application initialization:
 * \code
	wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_4KCLK);
	wdt_set_interrupt_callback(wdt_timer_callback);
	wdt_enable(INTERRUPT_MODE);
\endcode
 *
 * \subsection megarf_wdt_use_case_1_setup_flow Workflow
 * -# Set timeout period to 4K cycles or 32 ms:
 *   - \code wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_4KCLK); \endcode
 * -# Set callback funtion:
 *   - \code wdt_set_interrupt_callback(wdt_timer_callback); \endcode
 * -# Enable WDT in interrupt mode:
 *   - \code wdt_enable(INTERRUPT_MODE); \endcode
 *
 * \section megarf_wdt_use_case_1_usage Usage steps
 *
 * \subsection megarf_wdt_use_case_1_usage_code Example code
 * Add to, e.g., main loop in application C-file:
 * \code
	delay_ms(25);
	wdt_reset();
\endcode
 *
 * \subsection megarf_wdt_use_case_1_usage_flow Workflow
 * -# Wait for 25 ms to not reset the WDT :
 *   - \code delay_ms(25); \endcode
 * -# Reset the WDT before the timeout period is over to prevent system reset:
 *   - \code wdt_reset(); \endcode
 */
#endif /* _WDT_MEGARF_H_ */
