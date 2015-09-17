/**
 * \file
 *
 * \brief TC Example application for AVR UC3 TC driver.
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
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the TC driver.\n It also gives an example
 * of the usage of the TC module.
 *
 * This example will start a timer/counter and generates "tick" interrupts.
 *
 * The selected timer input clock is the internal clock labelled TC3
 * referred to as TIMER_CLOCK3 in the datasheet. TIMER_CLOCK3 is connected to
 * fPBA / 8 (see datasheet).
 *
 * The 16-bit timer/counter channel will cycle from 0x0000 to RC. RC is
 * initialized to (fPBA / 8) / 100, so that an interrupt will be triggered
 * every 10 ms. Upon interrupt, a GPIO line is toggled thus producing a square
 * signal of frequency 50 Hz.
 *
 *
 * \section files Main Files
 * - tc.c: TC driver
 * - tc.h: TC driver header file
 * - tc_simulator_example1.c: TC example
 * - conf_example.h: TC Configuration
 * - conf_clock.h: Clock configuration for example
 * - conf_board.h: Simulator configuration
 *
 * \section compinfo Compilation Info
 * This software was written for the GCC compiler for AVR32
 *
 * \section deviceinfo Device Info
 * All AVR32 devices with a TC module supported in AVR Studio 5 Simulator
 * can be used. This example has been tested with the following board:
 * - AVR Simulator
 *
 * \section setupinfo Setup Information
 * This example has been tested with the following configuration:
 * - AVR Simulator
 * - CPU clock: <i> Internal RC oscillator (about 115200 Hz) </i>.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <stdint.h>
#include <stdbool.h>

#include "tc.h"
#include "sysclk.h"
#include "board.h"
#include "gpio.h"
#include "intc.h"
#include "conf_example.h"


#if !defined(EXAMPLE_TC) || !defined(EXAMPLE_TC_IRQ)
#  error The TC preprocessor configuration to use in this example is missing.
#endif

#if !defined(EXAMPLE_TOGGLE_PIN)
#  error The preprocessor configuration of the GPIO pin to toggle is missing.
#endif
//! @}


//! \name Example configuration
//!@{
/**
 * \def CONFIG_SYSCLK_SOURCE
 * \brief Clock source to use
 */
/**
 * \def EXAMPLE_TC
 * \brief Base address of TC module to use.
 */
/**
 * \def EXAMPLE_TC_CHANNEL
 * \brief Channel to use with TC module.
 */
/**
 * \def EXAMPLE_TC_IRQ
 * \brief IRQ Line number for TC module
 */
 /**
 * \def EXAMPLE_TC_IRQ_PRIORITY
 * \brief IRQ Group number for TC module
 */
/**
 * \def EXAMPLE_TOGGLE_PIN
 * \brief GPIO Line to toggle on compare match.
 */
//!@}

// Flag to update the timer value
volatile static bool update_timer = true;
// Variable to contain the time ticks occurred
volatile static uint32_t tc_tick = 0;

/**
 * \brief TC interrupt.
 *
 * The ISR handles RC compare interrupt and sets the update_timer flag to
 * update the timer value.
 */
__attribute__((__interrupt__))
static void tc_irq(void)
{
	// Increment the ms seconds counter
	tc_tick++;
	/*
	 * TODO: Place a breakpoint here and watch the update of tc_tick variable
	 * in the Watch Window.
	 */

	// Clear the interrupt flag. This is a side effect of reading the TC SR.
	tc_read_sr(EXAMPLE_TC, EXAMPLE_TC_CHANNEL);

	// specify that an interrupt has been raised
	update_timer = true;
	// Toggle the GPIO line
	gpio_tgl_gpio_pin(EXAMPLE_TOGGLE_PIN);
}

/**
 * \brief TC Initialization
 *
 * Initializes and start the TC module with the following:
 * - Counter in Up mode with automatic reset on RC compare match.
 * - fPBA/8 is used as clock source for TC
 * - Enables RC compare match interrupt
 * \param tc Base address of the TC module
 */
static void tc_init(volatile avr32_tc_t *tc)
{

	// Options for waveform generation.
	static const tc_waveform_opt_t waveform_opt = {
		// Channel selection.
		.channel  = EXAMPLE_TC_CHANNEL,
		// Software trigger effect on TIOB.
		.bswtrg   = TC_EVT_EFFECT_NOOP,
		// External event effect on TIOB.
		.beevt    = TC_EVT_EFFECT_NOOP,
		// RC compare effect on TIOB.
		.bcpc     = TC_EVT_EFFECT_NOOP,
		// RB compare effect on TIOB.
		.bcpb     = TC_EVT_EFFECT_NOOP,
		// Software trigger effect on TIOA.
		.aswtrg   = TC_EVT_EFFECT_NOOP,
		// External event effect on TIOA.
		.aeevt    = TC_EVT_EFFECT_NOOP,
		// RC compare effect on TIOA.
		.acpc     = TC_EVT_EFFECT_NOOP,
		/* RA compare effect on TIOA.
		 * (other possibilities are none, set and clear).
		 */
		.acpa     = TC_EVT_EFFECT_NOOP,
		/* Waveform selection: Up mode with automatic trigger(reset)
		 * on RC compare.
		 */
		.wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,
		// External event trigger enable.
		.enetrg   = false,
		// External event selection.
		.eevt     = 0,
		// External event edge selection.
		.eevtedg  = TC_SEL_NO_EDGE,
		// Counter disable when RC compare.
		.cpcdis   = false,
		// Counter clock stopped with RC compare.
		.cpcstop  = false,
		// Burst signal selection.
		.burst    = false,
		// Clock inversion.
		.clki     = false,
		// Internal source clock 3, connected to fPBA / 8.
		.tcclks   = TC_CLOCK_SOURCE_TC3
	};

	// Options for enabling TC interrupts
	static const tc_interrupt_t tc_interrupt = {
		.etrgs = 0,
		.ldrbs = 0,
		.ldras = 0,
		.cpcs  = 1, // Enable interrupt on RC compare alone
		.cpbs  = 0,
		.cpas  = 0,
		.lovrs = 0,
		.covfs = 0
	};
	// Initialize the timer/counter.
	tc_init_waveform(tc, &waveform_opt);

	/*
	 * Set the compare triggers.
	 * We configure it to count every 10 milliseconds.
	 * We want: (1 / (fPBA / 8)) * RC = 10 ms, hence RC = (fPBA / 8) / 100
	 * to get an interrupt every 10 ms.
	 */
	tc_write_rc(tc, EXAMPLE_TC_CHANNEL, (sysclk_get_pba_hz() / 8 / 100));
	// configure the timer interrupt
	tc_configure_interrupts(tc, EXAMPLE_TC_CHANNEL, &tc_interrupt);
	// Start the timer/counter.
	tc_start(tc, EXAMPLE_TC_CHANNEL);
}

/**
 * \brief Main function
 *
 * Main function performs the following:
 * - Configure the TC Module
 * - Register the TC interrupt
 * - Configure, enable the CPCS (RC compare match) interrupt,
 * - and start a TC channel in waveform mode
 */
int main(void)
{
	volatile avr32_tc_t *tc = EXAMPLE_TC;
	uint32_t timer = 0;
	/**
	 * \note the call to sysclk_init() will disable all non-vital
	 * peripheral clocks, except for the peripheral clocks explicitly
	 * enabled in conf_clock.h.
	 */
	sysclk_init();
	// Enable the clock to the selected example Timer/counter peripheral module.
	sysclk_enable_peripheral_clock(EXAMPLE_TC);
	// Disable the interrupts
	cpu_irq_disable();
	// Initialize interrupt vectors.
	INTC_init_interrupts();
	// Register the RTC interrupt handler to the interrupt controller.
	INTC_register_interrupt(&tc_irq, EXAMPLE_TC_IRQ, EXAMPLE_TC_IRQ_PRIORITY);
	// Enable the interrupts
	cpu_irq_enable();
	// Initialize the timer module
	tc_init(tc);

	while(1) {
		// Update the timer every 10 milli second.
		if ((update_timer)) {
			timer++;
			/*
			 * TODO: Place a breakpoint here and watch the update of timer
			 * variable in the Watch Window.
			 */
			 // Reset the timer update flag to wait till next timer interrupt
			update_timer = false;
		}
	}
}
