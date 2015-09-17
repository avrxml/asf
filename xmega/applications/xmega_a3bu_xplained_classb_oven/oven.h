/**
 * \file
 *
 * \brief Class B Oven settings
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
#ifndef OVEN_H
#define OVEN_H

/**
 * \name Timing configurations
 *
 * @{
 */

/**
 * \brief Number of RTC ticks between WDT resets.
 *
 * Should be in the middle of the open window period of the WDT,
 * e.g. at 625 ticks if WPER is 500CLK and PER is 250CLK.
 */
#define OVEN_WDT_RESET_PERIOD 1250
/** How often a simulation step should be executed [ms] */
#define OVEN_SIM_STEP_TIME    100
/** How often a control step should be executed [ms] */
#define OVEN_CTL_STEP_TIME    20

/** @} */

/**
 * \name Simulation control signal timers selection
 *
 * @{
 */

/** Timer used to output frequency signal controlling the induction element */
#define OVEN_FREQ_TC           TCC0
/** Initial period for the frequency timer */
#define FREQ_TIMER_PERIOD_INIT 0x100
/** Timer used to capture the generated frequency signal */
#define OVEN_FREQ_CAPT_TC      TCC1

/** @} */

/**
 * \name Test timer selection
 *
 * @{
 */

/** Timer used by the Class B frequency consistency test of the CPU clock. */
#define CLASSB_FREQTEST_TC        TCD1
/** Timer used to execute periodic temperature sanity test */
#define OVEN_PERIODIC_TEMPTEST_TC TCE1
/** Timer used to execute periodic Class B tests */
#define OVEN_PERIODIC_CLASSB_TC   TCE0

/** @} */

/**
 * \name Display configuration
 *
 * @{
 */

/** X offset for the plot window */
#define OVEN_PLOT_OFFSET_X 5
/** Rightmost X coordinate of the plot window */
#define OVEN_PLOT_MAX_X    112
/** Lowest Y coordinate in the plot window */
#define OVEN_PLOT_MAX_Y    29
/** Topmost Y coordinate in the plot window */
#define OVEN_PLOT_MIN_Y    0
/** Display height */
#define OVEN_DISP_MAX_Y    32
/** Maximum watt level */
#define MAX_WATTAGE        15

/** @} */

/**
 * \name Oven state enumerations
 *
 * @{
 */

/** Current state of the oven */
enum ovenctl_plate_state {
	/** The power to the oven is turned off */
	POWER_OFF,
	/** Actuation is desired, but there is nothing on the plate */
	ON_NO_POT,
	/** Actuation is desired and is applied */
	ON_ACTUATING
};

/** Status LED color enumerations */
enum status_led {
	S_ORANGE,
	S_GREEN,
	S_RED,
	S_OFF
};

/** Pot status on or off plate enumeration */
enum pot_t {POT_OFF, POT_ON};

/** @} */

/** Global container for last WDT reset time */
extern volatile uint32_t classb_last_wdt_reset;

/**
 * \brief Reset the WDT when OVEN_WDT_RESET_PERIOD has passed
 *
 * This macro depends on the ASF RTC driver and that the timer is running
 * because it is used to get correct timing of watchdog resets in windowed mode.
 */
#define oven_wdt_periodic_reset() do {                                         \
	if ((rtc_get_time() - classb_last_wdt_reset) > OVEN_WDT_RESET_PERIOD) {\
		wdt_reset();                                                   \
		classb_last_wdt_reset = rtc_get_time();                        \
	}                                                                      \
} while(0)

void main_init_tc(void);
void main_init_adc_dac(void);

#endif /* OVEN_H */
