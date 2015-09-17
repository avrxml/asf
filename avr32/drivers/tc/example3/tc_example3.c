/**
 * \file
 *
 * \brief Timer/Counter example 3.
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
 * every 1 ms. Upon interrupt, a LED0 is toggled thus producing a square
 * signal of frequency 500 Hz. Check the toggled GPIO pin connected to LED0 on
 *  an oscilloscope and see the square signal at frequency 500 Hz.
 *
 * Also, a message is displayed on USART and the 'Timer:' value is updated
 * every second.
 * The displayed message is:\n
 * <code> ATMEL \n AVR32 UC3 - TC example \n Timer: 1 s </code>
 *
 * \section files Main Files
 * - tc.c: TC driver
 * - tc.h: TC driver header file
 * - tc_example3.c: TC example 3
 * - conf_example.h: TC Configuration
 * - conf_clock.h: Clock configuration for example
 * - conf_board.h: Simulator configuration
 *
 * \section compinfo Compilation Info
 * This software was written for GCC for AVR32 and IAR Embedded Workbench
 * for AVR32. Other compilers may or may not work.
 *
 * \section Configuration Information
 * This example has been tested with the following configuration:
 * - EVK1100, EVK1101, EVK1104, EVK1105, UC3L_EK, UC3C_EK, UC3-A3 XPLAINED,
 *   UC3-L0 XPLAINED, STK600 + RCUC3D routing board boards.
 * - CPU clock: 16MHz in UC3C_EK and 12 MHz in all other boards.
 * - PC terminal settings:
 *   - 57600 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/uc3">Atmel AVR UC3</A>.\n
*/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <stdint.h>
#include <stdbool.h>

#include "sysclk.h"
#include "board.h"
#include "print_funcs.h"
#include "tc.h"
#include "gpio.h"
#if defined (__GNUC__)
#  include "intc.h"
#endif
#include "conf_example.h"


#if !defined(EXAMPLE_TC) || !defined(EXAMPLE_TC_IRQ)
#  error The TC preprocessor configuration to use in this example is missing.
#endif

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
#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
#pragma handler = EXAMPLE_TC_IRQ_GROUP, 1
__interrupt
#endif
static void tc_irq(void)
{
	// Increment the ms seconds counter
	tc_tick++;

	// Clear the interrupt flag. This is a side effect of reading the TC SR.
	tc_read_sr(EXAMPLE_TC, EXAMPLE_TC_CHANNEL);

	// specify that an interrupt has been raised
	update_timer = true;
	// Toggle a GPIO pin (this pin is used as a regular GPIO pin).
	LED_Toggle(LED0);
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
		/*
		 * RA compare effect on TIOA.
		 * (other possibilities are none, set and clear).
		 */
		.acpa     = TC_EVT_EFFECT_NOOP,
		/*
		 * Waveform selection: Up mode with automatic trigger(reset)
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
	 * We configure it to count every 1 milliseconds.
	 * We want: (1 / (fPBA / 8)) * RC = 1 ms, hence RC = (fPBA / 8) / 1000
	 * to get an interrupt every 10 ms.
	 */
	tc_write_rc(tc, EXAMPLE_TC_CHANNEL, (sysclk_get_pba_hz() / 8 / 1000));
	// configure the timer interrupt
	tc_configure_interrupts(tc, EXAMPLE_TC_CHANNEL, &tc_interrupt);
	// Start the timer/counter.
	tc_start(tc, EXAMPLE_TC_CHANNEL);
}

/*! \brief Main function:
 *  - Configure the CPU to run at 12MHz
 *  - Configure the USART
 *  - Register the TC interrupt (GCC only)
 *  - Configure, enable the CPCS (RC compare match) interrupt, and start a
 *    TC channel in waveform mode
 *  - In an infinite loop, update the USART message every second.
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
	// Initialize the USART module for trace messages
	init_dbg_rs232(sysclk_get_pba_hz());
	// Disable the interrupts
	cpu_irq_disable();

#if defined (__GNUC__)
	// Initialize interrupt vectors.
	INTC_init_interrupts();
	// Register the RTC interrupt handler to the interrupt controller.
	INTC_register_interrupt(&tc_irq, EXAMPLE_TC_IRQ, EXAMPLE_TC_IRQ_PRIORITY);
#endif
	// Enable the interrupts
	cpu_irq_enable();
	// Initialize the timer module
	tc_init(tc);

	while (1) {
		// Update the display on USART every second.
		if ((update_timer) && (!(tc_tick%1000))) {
			timer++;
			// Set cursor to the position (1; 5)
			print_dbg("\x1B[5;1H");
			// Print the timer value
			print_dbg("ATMEL AVR UC3 - Timer/Counter Example 3\n\rTimer: ");
			print_dbg_ulong(timer);
			print_dbg(" s");
			// Reset the timer update flag to wait till next timer interrupt
			update_timer = false;
		}
	}
}
