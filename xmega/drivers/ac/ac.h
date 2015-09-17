/**
 * \file
 *
 * \brief Analog comparator (AC) functions
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
#ifndef DRIVERS_AC_AC_H
#define DRIVERS_AC_AC_H

#include <stdint.h>

#include "compiler.h"
#include "parts.h"

#include "status_codes.h"
#include "pmic.h"

/**
 * \defgroup analog_comparator_group Analog Comparator (AC)
 *
 * See \ref xmega_ac_quickstart
 *
 * This is a driver for configuring, enabling, disabling and use of the on-chip
 * analog comparator (AC).
 *
 * \section dependencies Dependencies
 *
 * The analog comparator depends on the following modules:
 *  - \ref pmic_group for interrupt functionality.
 *  - \ref sysclk_group for peripheral clock control.
 *  - \ref sleepmgr_group for setting available sleep mode.
 *
 * \section Quick start guide
 * See \ref xmega_ac_quickstart
 *
 * @{
 */

//! Analog comparator channel configuration struct
struct ac_config {
	//! Comparator control register
	uint8_t         acctrl;
	//! Comparator mux control register
	uint8_t         acmuxctrl;
	//! Control register B register
	uint8_t         ctrlb;
	//! Window mode control register
	uint8_t         winctrl;
};

//! Analog comparator status values
enum ac_status_t {
	//! Compare value is above window or reference value
	AC_STATUS_ABOVE     = AC_WSTATE_ABOVE_gc,
	//! Compare value is below window or reference value
	AC_STATUS_BELOW     = AC_WSTATE_BELOW_gc,
	//! Compare value is inside window
	AC_STATUS_INSIDE    = AC_WSTATE_INSIDE_gc,
};

//! Analog comparator modes
enum ac_mode_t {
	//! Analog comparator in normal single trigger value mode
	AC_MODE_SINGLE,
	//! Analog comparator in window trigger mode
	AC_MODE_WINDOW,
};

//! Analog comparator interrupt levels
enum ac_int_level_t {
	AC_INT_LVL_OFF = 0x00,
	AC_INT_LVL_LO = 0x01,
	AC_INT_LVL_MED = 0x02,
	AC_INT_LVL_HI = 0x03,
};

//! Analog comparator interrupt modes
enum ac_interrupt_mode_t {
	//! Interrupt on both edges
	AC_INT_MODE_BOTH_EDGES      = AC_INTMODE_BOTHEDGES_gc,
	//! Interrupt on falling edge
	AC_INT_MODE_FALLING_EDGE    = AC_INTMODE_FALLING_gc,
	//! Interrupt on rising edge
	AC_INT_MODE_RISING_EDGE     = AC_INTMODE_RISING_gc,
	//! Interrupt when above window
	AC_INT_MODE_ABOVE_WINDOW    = AC_WINTMODE_ABOVE_gc,
	//! Interrupt when inside window
	AC_INT_MODE_INSIDE_WINDOW   = AC_WINTMODE_INSIDE_gc,
	//! Interrupt when below window
	AC_INT_MODE_BELOW_WINDOW    = AC_WINTMODE_BELOW_gc,
	//! Interrupt when outside the window
	AC_INT_MODE_OUTSIDE_WINDOW  = AC_WINTMODE_OUTSIDE_gc,
};

/**
 * \brief Interrupt event callback function type
 *
 * The interrupt handler can be configured to do a function callback upon a
 * compare interrupt, the callback function must match the ac_callback_t type.
 *
 * Example:
 * \code
	void my_ac_callback(AC_t *ac, uint8_t channel, enum ac_status_t status) {
	     // Add desired functionality.
	}
\endcode
 *
 * Then add this callback function to the analog comparator interrupt routine
 * by setting it in the driver.
 *
 * \code
	ac_set_interrupt_callback(ACA, 0, my_ac_callback);
\endcode
 */
typedef void (*ac_callback_t) (AC_t *ac, uint8_t channel,
		enum ac_status_t status);

//! \name Management functions
//@{

void ac_write_config(AC_t *ac, uint8_t channel, struct ac_config *config);
void ac_read_config(AC_t *ac, uint8_t channel, struct ac_config *config);
enum ac_status_t ac_get_status(AC_t *ac, uint8_t channel);
void ac_enable(AC_t *ac, uint8_t channel);
void ac_disable(AC_t *ac, uint8_t channel);

//@}

//! \name Interrupt management functions
//@{

void ac_set_interrupt_callback(AC_t *ac, ac_callback_t callback);

/**
 * \brief Set analog comparator channel interrupt mode
 *
 * This function allows the caller to set which analog comparator channel
 * interrupt mode should cause an interrupt to trigger.
 *
 * \param config Pointer to an \ref ac_config variable
 * \param mode Interrupt mode given by an \ref ac_interrupt_mode_t value
 */
static inline void ac_set_interrupt_mode(struct ac_config *config,
		enum ac_interrupt_mode_t mode)
{
	if (mode & AC_INTMODE_gm) {
		config->acctrl &= ~AC_INTMODE_gm;
		config->acctrl |= mode;
	} else {
		config->winctrl &= ~AC_WINTMODE_gm;
		config->winctrl |= mode;
	}
}

/**
 * \brief Set analog comparator channel interrupt level
 *
 * This function allows the caller to set the analog comparator channel
 * interrupt level. The interrupt levels are defined by the
 * \see enum ac_int_level_t defines.
 *
 * \pre ac_set_mode() must be called before this function.
 *
 * \param config Pointer to an \ref ac_config variable
 * \param level Analog comparator interrupt level
 */
static inline void ac_set_interrupt_level(struct ac_config *config,
		enum ac_int_level_t level)
{
	if (config->winctrl & AC_INTLVL_gm) {
		config->winctrl &= ~AC_WINTLVL_gm;
		config->winctrl |= level;
	} else {
		config->acctrl &= ~AC_INTLVL_gm;
		config->acctrl |= (level << AC_INTLVL_gp);
	}
}

//@}

//! \name Configuration helper functions
//@{

/**
 * \brief Set analog comparator mode
 *
 * This function helps the caller setting the analog comparator mode for a
 * channel configuration. For AVR XMEGA this can be normal where it will
 * trigger at a defined value, or window mode, where two analog comparators are
 * combined to make two limits and three operating areas. The modes are defined
 * in the \ref ac_mode_t type.
 *
 * \note This function must be called before any other analog comparator
 * channel or window configuration. This is due to some of the set functions
 * depends on which mode the channels should be configured for.
 *
 * \param config Pointer to an \ref ac_config variable
 * \param mode Analog comparator mode given by a \ref ac_mode_t value
 */
static inline void ac_set_mode(struct ac_config *config, enum ac_mode_t mode)
{
	if (mode == AC_MODE_SINGLE) {
		config->winctrl = 0;
	} else {
		config->winctrl |= AC_WEN_bm;
	}
}

/**
 * \brief Set analog comparator reference for positive line
 *
 * This function helps the caller setting the analog comparator reference
 * source for the positive line, for a given channel configuration. The
 * references are defined in the toolchain header files in the form of
 * \a AC_MUXPOS_*_gc, where * represents the various available references.
 *
 * \note In window mode the positive reference is the input signal.
 *
 * \see Chip-specific datasheet or header file for available mux configuration.
 *
 * \param config Pointer to an \ref ac_config variable
 * \param reference Analog comparator positive reference
 */
static inline void ac_set_positive_reference(struct ac_config *config,
		AC_MUXPOS_t reference)
{
	config->acmuxctrl &= ~AC_MUXPOS_gm;
	config->acmuxctrl |= reference;
}

/**
 * \brief Set analog comparator reference for negative line
 *
 * This function helps the caller setting the analog comparator reference
 * source for the negative line, for a given channel configuration. The
 * references are defined in the toolchain header files in the form of
 * \a AC_MUXNEG_*_gc, where * represents the various available references.
 *
 *
 * \note In window mode the negative reference is the compare levels, analog
 * comparator channel 0 hold the upper limit of the window, while channel 1
 * hold the lower limit of the window.
 *
 * \see Chip-specific datasheet or header file for available mux configuration.
 *
 * \param config Pointer to an \ref ac_config variable
 * \param reference Analog comparator negative reference
 */
static inline void ac_set_negative_reference(struct ac_config *config,
		AC_MUXNEG_t reference)
{
	config->acmuxctrl &= ~AC_MUXNEG_gm;
	config->acmuxctrl |= reference;
}

/**
 * \brief Set analog comparator hysteresis
 *
 * This function helps the caller setting the analog comparator hysteresis for
 * a channel configuration. The hysteresis are defined in the toolchain
 * header files in the form of \a AC_HYSMODE_*_gc, where * represents \a NONE,
 * \a SMALL and \a LARGE.
 *
 * \param config Pointer to an \ref ac_config variable
 * \param hysteresis Analog comparator hysteresis config
 */
static inline void ac_set_hysteresis(struct ac_config *config,
		AC_HYSMODE_t hysteresis)
{
	config->acctrl &= ~AC_HYSMODE_gm;
	config->acctrl |= hysteresis;
}

/**
 * \brief Set analog comparator voltage scale
 *
 * This function helps the caller setting the analog comparator voltage scaler
 * for a channel configuration, the voltage scale is a numeric value.
 *
 * \note The analog comparator has one voltage scaler shared between the
 * channels. If both channels use the output from the voltage scaler they must
 * both share the same configuration.
 *
 * \param config Pointer to an \ref ac_config variable
 * \param scale Numeric value of the voltage scale
 */
static inline void ac_set_voltage_scaler(struct ac_config *config, uint8_t scale)
{
	Assert(scale < 64);
	config->ctrlb = scale;
}

#if XMEGA_A || XMEGA_AU
/**
 * \brief Set analog comparator high-speed mode
 *
 * This function helps the caller setting the analog comparator high-speed mode
 * for a channel configuration. High-speed mode enables the shortest
 * propagation delay, on the expense of power consumption.
 *
 * \note Enabling high-speed mode automatically disables low-power mode.
 *
 * \param config Pointer to an \ref ac_config variable
 */
static inline void ac_set_high_speed_mode(struct ac_config *config)
{
	config->acctrl |= AC_HSMODE_bm;
}
#endif

#if XMEGA_A || XMEGA_AU
/**
 * \brief Set analog comparator low-power mode
 *
 * This function helps the caller setting the analog comparator low-power mode
 * for a channel configuration. Low-power mode lowers the power consumption, on
 * the expense of propagation delay.
 *
 * \note Enabling low-power mode automatically disables high-speed mode.
 *
 * \param config Pointer to an \ref ac_config variable
 */
static inline void ac_set_low_power_mode(struct ac_config *config)
{
	config->acctrl &= ~AC_HSMODE_bm;
}
#endif
//@}

//! @}

/**
 * \page xmega_ac_quickstart Quick start guide for the XMEGA AC driver
 *
 * This is the quick start guide for the \ref ac_group "AC Driver", with
 * step-by-step instructions on how to configure and use the driver for
 * specific use cases.
 *
 * The section described below can be compiled into e.g. the main application
 * loop or any other function that might use the AC functionality.
 *
 * \section xmega_ac_quickstart_basic Basic use case of the AC driver
 *
 * In the basic use case, the Analog Comparator driver is set up to compare
 * an input voltage to the voltage VCC/2. If the voltage is below VCC/2, LED0
 * is lit up. If it is above VCC/2, LED 1 is lit.
 *
 * \subsection xmega_ac_quickstart_prereq Prerequisites
 *
 * This basic example uses the XMEGA interrupt functionality, and therefore
 * the \ref pmic_group PMIC driver must be included. The \ref gpio_group
 * driver must also be included to be able to control the LEDs.
 *
 * You must also enable the AC pins, this is done by defining:
 * \code
	#define CONF_BOARD_ENABLE_AC_PINS
\endcode
 * in your source code.
 *
 * The memset() function used to zero out our configuration structure is
 * located in string.h, so we must include it:
 * \code
	#include <string.h>
\endcode
 *
 * \section xmega_ac_quickstart_setup Setup
 *
 * \subsection xmega_ac_quickstart_setup_code Code
 *
 * \code
	 #define VOLTAGE_SCALED_VALUE 32

	 void ac_init()
	 {
	     struct ac_config aca_config;

	     memset(&aca_config, 0, sizeof(struct ac_config));

	     ac_set_mode(&aca_config, AC_MODE_SINGLE);
	     ac_set_voltage_scaler(&aca_config, VOLTAGE_SCALED_VALUE);
	     ac_set_hysteresis(&aca_config, AC_HYSMODE_LARGE_gc);
	     ac_set_negative_reference(&aca_config, AC_MUXNEG_SCALER_gc);
	     ac_set_positive_reference(&aca_config, AC_MUXPOS_PIN1_gc);

	     ac_set_interrupt_mode(&aca_config, AC_INT_MODE_BOTH_EDGES);
	     ac_set_interrupt_level(&aca_config, AC_INT_LVL_MED);
	     ac_set_interrupt_callback(&ACA, example_ac_window_interrupt);

	     ac_write_config(&ACA, 0, &aca_config);

	     ac_enable(&ACA, 0);

	     cpu_irq_enable();
	 }
\endcode
 *
 * Add to \c main():
 * \code
	ac_init();
\endcode
 *
 * \subsection xmega_ac_quickstart_setup_workflow Workflow
 *
 * We define how much we wish to scale the voltage:
 * \code
	#define VOLTAGE_SCALED_VALUE 32
\endcode
 *
 * We create a structure to store the configuration data for the AC:
 * \code
	struct ac_config aca_config;
\endcode
 *
 * We zero out the aca_config struct:
 * \code
	memset(&aca_config, 0, sizeof(struct ac_config));
\endcode
 * \attention This step is important, as uninitialized values in the struct
 *            may cause strange and unwanted behavior.
 *
 * First, we set the AC mode for the ACA comparator to single operation.
 * \code
	ac_set_mode(&aca_config, AC_MODE_SINGLE);
\endcode
 *
 * We set the reference voltage to VCC/2 (scaler 32/64):
 * \code
	ac_set_voltage_scaler(&aca_config, VOLTAGE_SCALED_VALUE-1);
\endcode
 *
 * We enable hysteresis, to avoid flickering due to disturbances:
 * \code
	ac_set_hysteresis(&aca_config, AC_HYSMODE_LARGE_gc);
\endcode
 *
 * We set the negative reference to be our scaled voltage, and the
 * positive reference to be the positive reference:
 * \code
	ac_set_negative_reference(&aca_config, AC_MUXNEG_SCALER_gc);
	ac_set_positive_reference(&aca_config, AC_MUXPOS_PIN1_gc);
\endcode
 *
 * We set the interrupt mode to call the interrupt function on
 * both edges, the interrupt level to medium, and set the callback
 * function:
 *
 * \code
	ac_set_interrupt_mode(&aca_config, AC_INT_MODE_BOTH_EDGES);
	ac_set_interrupt_level(&aca_config, AC_INT_LVL_MED);
	ac_set_interrupt_callback(&ACA, example_ac_window_interrupt);
\endcode
 *
 * We write the configuration to the AC A channel 0, and enable it:
 *
 * \code
	 ac_write_config(&ACA, 0, &aca_config);

	 ac_enable(&ACA, 0);
\endcode
 *
 * Finally, we must enable interrupts:
 * \code
	cpu_irq_enable();
\endcode
 *
 * \section xmega_ac_quickstart_example_code Use case
 *
 * The example itself is handled by the interrupt callback, which switches
 * the LEDS according to the AC status:
 *
 * \subsection xmega_ac_quickstart_use_case_code Example code
 *
 * \code
	 * static void example_ac_window_interrupt(AC_t *ac, uint8_t channel,
	 *         enum ac_status_t status)
	 * {
	 *     gpio_set_pin_high(LED0_GPIO);
	 *     gpio_set_pin_high(LED1_GPIO);

	 *     switch (status) {
	 *         case AC_STATUS_BELOW:
	 *             gpio_set_pin_low(LED0_GPIO);
	 *             break;
	 *         case AC_STATUS_ABOVE:
	 *             gpio_set_pin_low(LED1_GPIO);
	 *             break;
	 *         default: break;
	 *     }
	 * }
\endcode
 *
 * \subsection xmega_ac_quickstart_use_case_workflow Workflow
 *
 * We switch the LEDS off:
 * \code
	gpio_set_pin_high(LED0_GPIO);
	gpio_set_pin_high(LED1_GPIO);
\endcode
 *
 * We turn on LEDS according to where the AC reads our PIN to be
 * in relation to the reference.
 * \code
	switch (status) {
	    case AC_STATUS_BELOW:
	        gpio_set_pin_low(LED0_GPIO);
	        break;
	    case AC_STATUS_ABOVE:
	        gpio_set_pin_low(LED1_GPIO);
	        break;
	    default: break;
	}
\endcode
 * \note This interrupt function is called every time the AC sees a change
 *       on the AC output (rising or falling).
 *
 */
#endif /* DRIVERS_AC_AC_H */
