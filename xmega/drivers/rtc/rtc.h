/**
 * \file
 *
 * \brief AVR XMEGA Real Time Counter driver definitions
 *
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
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
#ifndef DRIVERS_RTC_RTC_H
#define DRIVERS_RTC_RTC_H

#include <compiler.h>
#include <conf_rtc.h>

/**
 * \defgroup rtc_group Real Time Counter (RTC)
 *
 * See \ref xmega_rtc_quickstart.
 *
 * This is a driver implementation for the XMEGA RTC.
 *
 * \section rtc_min_alarm_time Minimum allowed alarm time
 *
 * If current time is close to a time unit roll over, there is a risk to miss
 * this when using a value of 0.
 *
 * A safe use of this can be in an alarm callback.
 *
 * @{
 */

/**
 * \def CONFIG_RTC_COMPARE_INT_LEVEL
 * \brief Configuration symbol for interrupt level to use on alarm
 *
 * Possible values:
 * - RTC_COMPINTLVL_LO_gc
 * - RTC_COMPINTLVL_MED_gc
 * - RTC_COMPINTLVL_HI_gc
 */
#ifdef __DOXYGEN__
# define CONFIG_RTC_COMPARE_INT_LEVEL
#endif

/**
 * \def CONFIG_RTC_OVERFLOW_INT_LEVEL
 * \brief Configuration symbol for interrupt level to use on overflow
 *
 * Possible values:
 * - RTC_OVFINTLVL_LO_gc
 * - RTC_OVFINTLVL_MED_gc
 * - RTC_OVFINTLVL_HI_gc
 */
#ifdef __DOXYGEN__
# define CONFIG_RTC_OVERFLOW_INT_LEVEL
#endif

/**
 * \def CONFIG_RTC_PRESCALER
 * \brief Configuration symbol for prescaler to use
 *
 * Possible values:
 * - RTC_PRESCALER_DIV1_gc
 * - RTC_PRESCALER_DIV2_gc
 * - RTC_PRESCALER_DIV8_gc
 * - RTC_PRESCALER_DIV16_gc
 * - RTC_PRESCALER_DIV64_gc
 * - RTC_PRESCALER_DIV256_gc
 * - RTC_PRESCALER_DIV1024_gc
 */
#ifdef __DOXYGEN__
# define CONFIG_RTC_PRESCALER
#endif

/**
 * \brief Callback definition for alarm callback
 *
 * \param time The time of the alarm
 */
typedef void (*rtc_callback_t)(uint32_t time);

void rtc_set_callback(rtc_callback_t callback);
void rtc_set_time(uint32_t time);
uint32_t rtc_get_time(void);
void rtc_set_alarm(uint32_t time);
bool rtc_alarm_has_triggered(void);

#if XMEGA_E

void rtc_load_calibration(uint8_t error_value, uint8_t sign);

/**
 * \brief Enable correction of RTC clock
 *
 * \note The RTC-CALIB register should be loaded with proper error and
 *       sign value before calling this function.
 */
static inline void rtc_enable_correction(void)
{
	RTC.CTRL |= RTC_CORREN_bm;
}

/**
 * \brief Disable correction of RTC clock
 */
static inline void rtc_disable_correction(void)
{
	RTC.CTRL &= ~RTC_CORREN_bm;
}

#endif

/**
 * \brief Set alarm relative to current time
 *
 * \param offset Offset to current time. This is minimum value, so the alarm
 * might happen at up to one time unit later. See also \ref
 * rtc_min_alarm_time
 *
 * \note For devices with the errata "RTC Counter value not correctly read
 *       after sleep", this can be unsafe to do shortly after waking up from sleep.
 * \note Without this errata this function can block for up to 1 RTC clock
 *       source cycle after waking up from sleep.
 */
static inline void rtc_set_alarm_relative(uint32_t offset)
{
        rtc_set_alarm(rtc_get_time() + offset);
}

extern void rtc_init(void);

//! @}

/**
 * \page xmega_rtc_quickstart Quick start guide for RTC driver
 *
 * This is the quick start guide for the \ref rtc_group "RTC driver", with
 * step-by-step instructions on how to configure and use the drivers in a
 * selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 *
 * \section rtc_basic_use_case Basic use case
 * In this basic use case, the RTC is configured for:
 * - Clock source: 1 kHz from internal 32 kHz ULP
 * - Prescaling: RTC clock/1024
 *
 * \section rtc_basic_use_case_setup Setup steps
 *
 * \subsection rtc_basic_use_case_setup_code Example code
 * Content of conf_rtc.h:
 * \code
	#define CONFIG_RTC_PRESCALER       RTC_PRESCALER_DIV1024_gc
\endcode
 * Needed in conf_clock.h:
 * \code
	#define CONFIG_RTC_SOURCE          SYSCLK_RTCSRC_ULP
\endcode
 * Add to the initialization code:
 * \code
	sysclk_init();
	rtc_init();
\endcode
 *
 * \subsection rtc_basic_use_case_setup_flow Workflow
 * -# Ensure that conf_rtc.h is present for the driver.
 *   - \note This configuration file is used by the driver and
 * should not be included by the user.
 * -# Initialize system clock:
 *   - \code sysclk_init(); \endcode
 *   - \note Make sure the define CONFIG_RTC_SOURCE is defined in conf_clock.h
 * -# Call RTC driver's own init function to start up the RTC and start
 * counting from zero:
 *   - \code rtc_init(); \endcode
 *
 * \section rtc_basic_use_case_usage Usage steps
 *
 * \subsection rtc_basic_use_case_usage_code Example code
 * Add to, e.g., main loop in application C-file:
 * \code
	rtc_get_time();
\endcode
 *
 * \subsection rtc_basic_use_case_usage_flow Workflow
 * -# Get current time of the RTC:
 *   - \code rtc_get_time(); \endcode
 *
 * \section rtc_use_cases Advanced use cases
 * For more advanced use of the RTC driver, see the following use cases:
 * - \subpage rtc_use_case_1 :
 */

/**
 * \page rtc_use_case_1 Use case #1
 *
 * In this use case, the RTC is configured for:
 * - Clock source: 1 kHz from internal 32 kHz ULP
 * - Prescaling: RTC clock/1024
 *
 * This use case shows how to set an alarm for the RTC.
 *
 * \section rtc_use_case_1_setup Setup steps
 *
 * \subsection rtc_basic_use_case_setup_prereq Prerequisites
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * -# PMIC for interrupt handling.
 * -# Sleep Manager.
 * -# A \ref rtc_callback_t "callback" function, called alarm, that
 * reschedules the alarm must be provided by the user:
 * \code
	static void alarm(uint32_t time)
	{
	    rtc_set_alarm(time);
	}
\endcode
 *
 * \subsection rtc_use_case_1_setup_code Example code
 * Content of conf_rtc.h:
 * \code
	#define CONFIG_RTC_PRESCALER       RTC_PRESCALER_DIV1024_gc
\endcode
 * Needed in conf_clock.h:
 * \code
	#define CONFIG_RTC_SOURCE          SYSCLK_RTCSRC_ULP
\endcode
 * Add to application initialization:
 * \code
	pmic_init();
	sysclk_init();
	sleepmgr_init();
	rtc_init();
	rtc_set_callback(alarm);
	cpu_irq_enable();
\endcode
 *
 * \subsection rtc_use_case_1_setup_flow Workflow
 * -# Ensure that conf_rtc.h is present for the driver.
 *   - \note This configuration file is used by the driver and
 * should not be included by the user.
 * -# Call the init function of the PMIC driver to enable all interrupt levels:
 *   - \code pmic_init(); \endcode
 * -# Initialize system clock:
 *   - \code sysclk_init(); \endcode
 *   - \note Make sure the define CONFIG_RTC_SOURCE is defined in conf_clock.h
 * -# Call the init function of the sleep manager driver to be able to sleep
 * waiting for alarm:
 *   - \code sleepmgr_init(); \endcode
 * -# Call RTC driver's own init function to start up the RTC and start
 * counting from zero:
 *   - \code rtc_init(); \endcode
 * -# Set callback function to call on alarm:
 *   - \code rtc_set_callback(alarm); \endcode
 *   - \note The callback function alarm must be defined by the user.
 * -# Enable interrupts globally:
 *   - \code cpu_irq_enable(); \endcode
 *
 * \section rtc_use_case_1_usage Usage steps
 *
 * \subsection rtc_use_case_1_usage_code Example code
 * \code
	rtc_set_alarm_relative(0);
	while (true) {
	    sleepmgr_enter_sleep();
	}
\endcode
 *
 * \subsection rtc_use_case_1_usage_flow Workflow
 * -# Set the alarm to trigger on next time unit roll over:
 *   - \code rtc_set_alarm_relative(0); \endcode
 * -# Sleep between each triggered alarm:
 *   - \code
	while (true) {
	    sleepmgr_enter_sleep();
	}
\endcode
 */

#endif /* RTC_H */
