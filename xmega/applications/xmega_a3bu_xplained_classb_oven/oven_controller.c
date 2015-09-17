/**
 * \file
 *
 * \brief Class B Oven controller
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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

#include <asf.h>
#include "oven.h"
#include "oven_controller.h"
#include "oven_classb.h"

#include <error_handler.h>
#include <classb_cpu.h>
#include <classb_interrupt_monitor.h>
#include <classb_sram.h>

/**
 * \var classb_error_value::CLASSB_ERROR_OTHER
 *
 * \note In this application, this value is used to signal that the periodic
 * temperature sanity test (in \ref oven_controller.c) failed.
 */

/**
 * \name Internal test functions
 *
 * @{
 */

/**
 * \brief Perform a sanity check on the temperature measured by the ADC
 *
 * This is an example of how the Class B interrupt monitor is used,
 * and as such does not perform a rigorous check.
 *
 * An error can be induced by removing the jumper between ADC2 and ADC4
 * on the J2 header on the XPLAINED board.
 *
 * \return Nothing is returned, but the test will alter the state
 * of the global \ref classb_error if an error is detected.
 */
static void ovenctl_periodic_temperature_sanity_check(void)
{
	uint16_t temp;

	temp = ovenctl_get_plate_temperature();
	if ((temp > 100) || (temp == 0)) {
		classb_error = CLASSB_ERROR_OTHER;
	}

	classb_intmon_increase(TEMP_SANITY_TEST);
}

/**
 * \brief Execute SRAM and CPU tests
 *
 * This is an example of how the Class B interrupt monitor is used.
 * The tests performed in this function are not required to be run
 * periodically for Class B certification.
 *
 * \return Nothing is returned, but the tests will alter the state
 * of the global classb_error if an error is detected.
 */
static void ovenctl_periodic_classb_tests(void)
{
	/* Check a section of SRAM */
	cli();
	classb_sram_test();

	/* Check the register file */
	classb_register_test();
	sei();

	/* Tell the interrupt monitor that we have executed */
	classb_intmon_increase(PER_CLASSB_TESTS);
}

/**
 * \brief Run tests and turn on power to simulated oven plate
 *
 * This functions runs some class B tests, reinitializes Timer/Counters, ADC and
 * DAC used by the oven, then turns on and starts monitoring of periodic tests.
 */
static void ovenctl_turn_on_plate(void)
{
	/* Store current DAC value since the analog IO test is destructive */
	uint16_t dac_val = DACB.CH0DATA;

	/* Run tests -- classb_error is updated if any of them fail */
	oven_classb_run_tests();

	/* Initialize the ADC and DAC modules, as well as the Timer/Counters
	 * we use to emulate real world application.
	 */
	main_init_adc_dac();
	DACB.CH0DATA = dac_val;
	main_init_tc();

	/* Enable and set up timer for periodic temperature checking */
	tc_enable(&OVEN_PERIODIC_TEMPTEST_TC);
	/* Set timer to overflow every 600ms: 24MHz / (1024 * 14063) = 0.6s */
	tc_write_clock_source(&OVEN_PERIODIC_TEMPTEST_TC, TC_CLKSEL_DIV1024_gc);
	tc_write_period(&OVEN_PERIODIC_TEMPTEST_TC, 14062);

	/* Set up temperature check as interrupt callback function, then enable
	 * interrupts
	 */
	tc_set_overflow_interrupt_callback(&OVEN_PERIODIC_TEMPTEST_TC,
			ovenctl_periodic_temperature_sanity_check);
	tc_set_overflow_interrupt_level(&OVEN_PERIODIC_TEMPTEST_TC,
			TC_OVFINTLVL_LO_gc);

	/* Enable monitoring of the periodic temperature check */
	classb_intmon_set_state(TEMP_SANITY_TEST, M_ENABLE);

	/* Enable and set up timer for periodic execution of Class B tests */
	tc_enable(&OVEN_PERIODIC_CLASSB_TC);
	/* Set timer to overflow every second: 24MHz / (1024 * 23438) = 1s */
	tc_write_clock_source(&OVEN_PERIODIC_CLASSB_TC, TC_CLKSEL_DIV1024_gc);
	tc_write_period(&OVEN_PERIODIC_CLASSB_TC, 23437);
	/* Set up periodic class B test as interrupt callback function, then
	 * enable interrupts
	 */
	tc_set_overflow_interrupt_callback(&OVEN_PERIODIC_CLASSB_TC,
			ovenctl_periodic_classb_tests);
	tc_set_overflow_interrupt_level(&OVEN_PERIODIC_CLASSB_TC,
			TC_OVFINTLVL_LO_gc);

	/* Enable monitoring of the periodic temperature check */
	classb_intmon_set_state(PER_CLASSB_TESTS, M_ENABLE);
}

/**
 * \brief Turn off power to simulated oven plate, disable tests
 *
 * This function modified the simulation input to zero (zero watts, no
 * power input), then turns off the periodic tests and disables monitoring of
 * them.
 *
 * \param *wattage Pointer to the value representing desired power level
 * \param *power Pointer to the value representing whether the desired power
 * level should be applied to the induction element.
 */
static void ovenctl_turn_off_plate(uint8_t *wattage, bool *power)
{
	*wattage = 0;
	*power = false;

	/* Turn off and stop monitoring temperature test */
	tc_write_clock_source(&OVEN_PERIODIC_TEMPTEST_TC, TC_CLKSEL_OFF_gc);
	tc_reset(&OVEN_PERIODIC_TEMPTEST_TC);
	classb_intmon_set_state(TEMP_SANITY_TEST, M_DISABLE);

	/* Turn off and stop monitoring periodic Class B tests. */
	tc_write_clock_source(&OVEN_PERIODIC_CLASSB_TC, TC_CLKSEL_OFF_gc);
	tc_reset(&OVEN_PERIODIC_CLASSB_TC);
	classb_intmon_set_state(PER_CLASSB_TESTS, M_DISABLE);
}

/**
 * \brief Updates or turns off the signal to the induction element
 *
 * Converts a wattage value to a signal period. Higher wattage means
 * shorter period, which means more induced power if this was a
 * physical stove top.
 *
 * \param wattage value representing the desired power level.
 */
static void ovenctl_actuate_induction_element(uint8_t wattage)
{
	if (wattage == 0) {
		tc_write_clock_source(&OVEN_FREQ_TC, TC_CLKSEL_OFF_gc);
	} else {
		tc_write_clock_source(&OVEN_FREQ_TC, TC_CLKSEL_DIV256_gc);
	}

	OVEN_FREQ_TC.CCABUF = 512 / wattage;
}

/** @} */

/**
 * \brief Execute oven control step.
 *
 * This function takes information from the user interface, temperature sensor
 * and plate (QTouch) sensing and controls the output to the induction elements
 * via a frequency signal.
 *
 * The oven has three states:
 *    - POWER_OFF where we just wait for input from the user interface to turn
 *      on.
 *    - ON_NO_POT where the user has indicated that power should be applied, but
 *      there is no pot in the plate. A safety timer changes the state back to
 *      POWER_OFF.
 *    - ON_ACTUATING where we have a pot on the oven, and actuate the induction
 *      elements while waiting for user input, and check if there is a pot on
 *      the oven.
 *
 * \param time Current system time in milliseconds.
 * \param *wattage Pointer to the variable representing the desired effect
 * \param *power Pointer to the variable controlling whether the induction
 * coils should be actuated.
 * \param potstate A variable representing the latest information on whether
 * anything is present on the plate (QTouch sensor)
 */
void ovenctl_execute_control_step(uint32_t time, uint8_t *wattage, bool *power,
		enum pot_t potstate)
{
	/* Current state of the oven */
	static enum ovenctl_plate_state plate_control_state = POWER_OFF;
	static uint32_t time_pot_off = 0;

	switch (plate_control_state) {
	case POWER_OFF:
		/* If the user has requested that the power be turned on,
		 * run Class B tests and go to ON_NO_POT
		 */
		if (*wattage > 0) {
			ovenctl_turn_on_plate();
			time_pot_off = rtc_get_time();
			plate_control_state = ON_NO_POT;
		}
		break;

	case ON_NO_POT:
		/* If too long has passed without pot on the plate, turn off
		 * oven
		 */
		if ((rtc_get_time() - time_pot_off) > (20 * 1000)) {
			ovenctl_turn_off_plate(wattage, power);
			plate_control_state = POWER_OFF;

		/* If user turned off power, turn off power. */
		} else if (*wattage == 0) {
			ovenctl_turn_off_plate(wattage, power);
			plate_control_state = POWER_OFF;

		/* If pot is put back before timeout, go back to active mode */
		} else if (potstate == POT_ON) {
			*power = true;
			plate_control_state = ON_ACTUATING;
		}
		break;

	case ON_ACTUATING:
		/* Check if pot is on. If it is not, go to inactive mode. */
		if (potstate == POT_OFF) {
			*power = false;
			time_pot_off = rtc_get_time();
			plate_control_state = ON_NO_POT;

		/* If user turned off power, turn off power. */
		} else if (*wattage == 0) {
			ovenctl_turn_off_plate(wattage, power);
			plate_control_state = POWER_OFF;
		}

		break;
	}

	/* Update the output signal to the induction element according to
	 * selected power level, and whether power should be applied.
	 */
	ovenctl_actuate_induction_element(*power ? *wattage : 0);
}

/**
 * \brief Reads a 16-bit analog value on ADC channel 0 and returns it.
 *
 * In this application CH0 is set up to read the analog voltage from
 * a simulated thermometer.
 *
 * \return Temperature of the induction element.
 */
uint16_t ovenctl_get_plate_temperature(void)
{
	adc_start_conversion(&ADCA, ADC_CH0);
	adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);

	return adc_get_unsigned_result(&ADCA, ADC_CH0) / 4;
}
