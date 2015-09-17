/**
 * \file
 *
 * \brief UC3-A3 Xplained Demo Application
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

/**
 * \mainpage
 *
 * \section intro Introduction
 *
 * This application demonstrates the QTouch functionality of the UC3-A3
 * Xplained board. On power-on, the board will initialize the required touch
 * libraries, showing the current touch state of the slider and touch button on
 * the board LEDs.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/avr/">Atmel AVR UC3</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "gpio.h"
#include "board.h"
#include "pm.h"
#include "tc.h"
#include "board.h"
#include "sysclk.h"
#include "uc3-a3_xplained_demo.h"

/**
 * \brief Initializes the TC subsystem ready to generate a LED PWM wave.
 *
 * Initializes the on-chip TC module in PWM generation mode, and configures the
 * board LED as an output so that the LED brightness can be adjusted.
 */
static void pwm_timer_init(void)
{
	// Assign output pin to timer/counter 0 channel B
	gpio_enable_module_pin(AVR32_TC0_B0_0_0_PIN,
			AVR32_TC0_B0_0_0_FUNCTION);

	// Timer waveform options
	const tc_waveform_opt_t waveform_options = {
		//! Channel selection.
		.channel  = 0,

		//! Software trigger effect on TIOB.
		.bswtrg   = TC_EVT_EFFECT_NOOP,
		//! External event effect on TIOB.
		.beevt    = TC_EVT_EFFECT_NOOP,
		//! RC compare effect on TIOB.
		.bcpc     = TC_EVT_EFFECT_CLEAR,
		//! RB compare effect on TIOB.
		.bcpb     = TC_EVT_EFFECT_SET,

		//! Software trigger effect on TIOA.
		.aswtrg   = TC_EVT_EFFECT_NOOP,
		//! External event effect on TIOA.
		.aeevt    = TC_EVT_EFFECT_NOOP,
		//! RC compare effect on TIOA.
		.acpc     = TC_EVT_EFFECT_NOOP,
		//! RA compare effect on TIOA.
		.acpa     = TC_EVT_EFFECT_NOOP,

		//! Waveform selection
		.wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,
		//! External event trigger enable.
		.enetrg   = false,
		//! External event selection (non-zero for Channel B to work)
		.eevt     = !0,
		//! External event edge selection.
		.eevtedg  = TC_SEL_NO_EDGE,
		//! Counter disable when RC compare.
		.cpcdis   = false,
		//! Counter clock stopped with RC compare.
		.cpcstop  = false,

		//! Burst signal selection.
		.burst    = false,
		//! Clock inversion selection.
		.clki     = false,
		//! Internal source clock 5, fPBA/128.
		.tcclks   = TC_CLOCK_SOURCE_TC5,
	};

	// Setup timer/counter waveform mode
	sysclk_enable_peripheral_clock(&AVR32_TC0);
	tc_init_waveform(&AVR32_TC0, &waveform_options);

	// Write the TOP (RC) and COMPARE (RB) values
	tc_write_rb(&AVR32_TC0, 0, 1);   // Set RB value.
	tc_write_rc(&AVR32_TC0, 0, 255); // Set RC value.

	// Start the timer PWM channel
	tc_start(&AVR32_TC0, 0);
}

/**
 * \brief Application main routine
 */
int main(void)
{
	board_init();
	sysclk_init();

	irq_initialize_vectors();
	cpu_irq_enable();

	pwm_timer_init();
	touch_init();

	while (true) {
		touch_handler();
	}
}
