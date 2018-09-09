/**
 * \file
 *
 * \brief UC3-A3 Xplained Demo Application
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
 * <A href="http://www.microchip.com/design-centers/8-bit/avr-mcus">Atmel AVR UC3</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
