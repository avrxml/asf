/**
 * \file
 *
 * \brief megaAVR Timer/Counter (TC) Timeout Driver Example 1
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
 * This simple example shows how to use the \ref tc_timeout_group to toggle
 * two LEDs at different speeds.
 *
 * \section files Main files:
 *  - tc_timeout_example1.c: This example application
 *  - tc_timeout.c: TC Timeout Driver implementation
 *  - tc_timeout.h: TC Timeout Driver definitions
 *  - conf_tc_timeout.h: Configuration of the TC Timeout Driver
 *
 * \section driverinfo TC Timeout Driver
 * The driver can be found \ref tc_timeout_group "here".
 *
 * \section deviceinfo Device Info
 * All megaAVR devices with a Timer/Counter module can be used.
 *
 * \section setup Setup
 * The following connections are required on STK600 for this example:
 * - For LED output, connect PORTD pin header to LED pin header with a 10-pin
 *   cable.
 * - For 32KHz clock, connect a wire between 32KHz and TOSC2 on the AUX pin
 *   header.
 *
 * \section exampledescription Description of the example
 * The example will configure a tick speed of 1024 Hz and use two different
 * periodic timeouts to toggle two different LEDs. One will toggle at 1Hz and
 * the other at 1/3Hz.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include "compiler.h"
#include "tc_timeout.h"

// Use an enum to differentiate between the different timeouts
enum {
	EXAMPLE_TIMEOUT_SLOW,
	EXAMPLE_TIMEOUT_FAST,
};

#define LED0_MASK (1 << 0)
#define LED1_MASK (1 << 1)

/**
 * \brief Toggle LED
 *
 * Newer megaAVR devices can toggle a I/O pin by writing to PIN register, while on
 * older ones this needs to be done by a read-modify-write operation. This function
 * abstract these differences.
 *
 * \param mask Mask of port LEDs to toggle
 */
static inline void toggle_led(uint8_t mask)
{
#if defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) \
		|| defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)
	/*
	 * For older megaAVR devices read-modify-write PORT register.
	 * This isn't safe for interrupts.
	 */
	PORTD ^= mask;
#else
	// Use PIN register to toggle on newer megaAVR devices
	PIND = mask;
#endif
}

int main(void)
{
	// Set LED0 and LED1 connected to port D to outputs
	DDRD |= LED0_MASK | LED1_MASK;

	// Initialize the timeout internals before using it
	tc_timeout_init();

	// Enable global interrupts
	sei();

	/*
	 * Start the two timeouts. One at 1 second interval and the other at
	 * 1/3 second interval.
	 */
	tc_timeout_start_periodic(EXAMPLE_TIMEOUT_SLOW, TC_TIMEOUT_TICK_HZ);
	tc_timeout_start_periodic(EXAMPLE_TIMEOUT_FAST, TC_TIMEOUT_TICK_HZ / 3);

	while (1) {
		// Loop and test for each expire and toggle LED
		if (tc_timeout_test_and_clear_expired(EXAMPLE_TIMEOUT_SLOW)) {
			toggle_led(LED0_MASK);
		}
		if (tc_timeout_test_and_clear_expired(EXAMPLE_TIMEOUT_FAST)) {
			toggle_led(LED1_MASK);
		}
	}
}

