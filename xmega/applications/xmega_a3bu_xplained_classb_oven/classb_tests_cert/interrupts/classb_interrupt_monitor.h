/**
 * \file
 *
 * \brief Settings and definitions for the interrupt monitor.
 *
 * \par Application note:
 *      AVR1610: Guide to IEC60730 Class B compliance with XMEGA
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef CLASSB_INTERRUPT_MONITOR_H_
#define CLASSB_INTERRUPT_MONITOR_H_

#include <asf.h>
#include "classb_rtc_common.h"
#include "error_handler.h"

/**
 * \defgroup int_mon Interrupt Monitor
 *
 * \brief A test for the frequency of interrupts.
 *
 * Each time an interrupt is executed a counter is increased. The interrupt
 * monitor is executed periodically to check that the counters are within a
 * configurable range.
 * The Real Time Counter is used to run \ref classb_intmon_callback()
 * periodically. The Interrupt monitor is enabled after calling
 * \ref classb_rtc_setup(). Note that \ref CLASSB_INT_MON should be defined.
 * See \ref rtc_driver for more details.
 *
 * In order to monitor an interrupt, the following steps should be followed:
 * -# Register the interrupt in \ref classb_int_identifiers.
 * -# The main application registers the interrupt by calling
 * \ref classb_intmon_reg_int(). This gives the monitor information on the
 * interrupt, for example the expected frequency.
 * -# The interrupts that have to be monitored should call
 * \ref classb_intmon_increase() on each execution.
 * -# The main application requests that the monitor starts checking the
 * interrupt. This is done by changing the interrupt state to \c M_ENABLE with
 * \ref classb_intmon_set_state(). The next time the interrupt monitor runs the
 * state of the interrupt will be set to \c ON.
 * -# If at some point an interrupt should stop being monitored, the main
 * application can change the state to \c M_DISABLE. The interrupt monitor will
 * change the state to \c OFF the next time it is executed.
 *
 * The fact that the main application has to request to start monitoring an
 * interrupt (and that it is the monitor sets the \c ON state) ensures that the
 * interrupt counter is synchronized with the interrupt monitor, i.e. the
 * interrupt counter starts being increased exactly after a period of the
 * interrupt monitor.
 *
 * Note that the interrupt counter is only increased if the interrupt is \c ON.
 * When an interrupt is \c OFF, the counter should be zero and otherwise the
 * error flag will be set. Further, enabling an interrupt that is \c ON or
 * disabling an interrupt that is \c OFF will call the error handler if
 * \ref CLASSB_STRICT_INTMON is defined.
 *
 * The error handler related to this test is CLASSB_ERROR_HANDLER_INTERRUPT().
 *
 * @{
 */

/**
 * \defgroup int_mon_conf Settings
 *
 * \brief Settings for the interrupt monitor
 *
 * @{
 */

/**
 * \brief Enumeration of interrupt identifiers.
 *
 * This enumeration holds the identifiers for the interrupts that should be
 * monitored. These identifiers are used in the interrupt when calling functions
 * related to the interrupt monitor: \ref classb_intmon_reg_int(),
 * \ref classb_intmon_increase() and \ref classb_intmon_set_state(). Interrupt
 * identifiers are included before \ref N_INTERRUPTS, so that it will hold the
 * total number of registered interrupts.
 */
enum classb_int_identifiers {
	TEMP_SANITY_TEST,
	PER_CLASSB_TESTS,
	/** Number of registered interrupts */
	N_INTERRUPTS
};

/**
 * \brief Behavior for re-enabling or re-disabling monitoring of interrupts.
 *
 * If this is defined, enabling an interrupt that is in \c ON state or disabling
 * an interrupt that is in \c OFF state will call the error handler.
 */
#ifdef __DOXYGEN__
#  define CLASSB_STRICT_INTMON
#else
/** #define CLASSB_STRICT_INTMON */
#endif

/** @} */

/**
 * \defgroup int_mon_def Interrupt data interface
 *
 * \brief Definition of data structures used by the monitor.
 *
 * @{
 */

/** Enumeration of interrupt states. */
enum classb_int_states {
	/** Interrupt is being monitored (set by interrupt monitor). */
	ON,
	/** Interrupt is not being monitored (set by interrupt monitor). */
	OFF,
	/** Interrupt should start being monitored (set by user application). */
	M_ENABLE,
	/** Interrupt should stop being monitored (set by user application). */
	M_DISABLE
};

/**
 * \brief Data structure for the interrupts that are monitored.
 *
 * The main application has to register the interrupt by calling
 * \ref classb_intmon_reg_int(). It is that function that sets the values of the
 * structure.
 */
struct classb_intmon_interrupt {
	/**
	 * \brief Expected number of interrupts in the monitor period.
	 *
	 * The monitor period is defined by \ref CLASSB_RTC_INT_PERIOD and
	 * \ref CLASSB_RTC_FREQ.
	 */
	uint16_t expected;
	/**
	 * \brief Interrupt counter.
	 *
	 * This holds the number of interrupt occurrences in the current monitor
	 * period.
	 */
	uint16_t count;
	/** Limit for deviation in the counter. */
	uint16_t limit;
	/** State of the interrupt. */
	enum classb_int_states state;
};

/** @} */

/**
 * \defgroup int_mon_func Functions
 *
 * \brief Functions related to the interrupt monitor
 *
 * @{
 */

void classb_intmon_set_state(enum classb_int_identifiers identifier,
		enum classb_int_states state);
void classb_intmon_reg_int(enum classb_int_identifiers identifier,
		uint16_t interrupt_counter, uint8_t tolerance_percent);
void classb_intmon_increase(enum classb_int_identifiers identifier);
void classb_intmon_callback(void);

/** @} */

/** @} */

#endif /* CLASSB_INTERRUPT_MONITOR_H_ */
