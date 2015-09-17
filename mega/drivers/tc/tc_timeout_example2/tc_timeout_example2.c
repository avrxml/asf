/**
 * \file
 *
 * \brief megaAVR Timer/Counter (TC) Timeout Driver Example 2
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
 * This example shows how to use the \ref tc_timeout_group to implement a
 * software debounce filter on a button input.
 *
 * \section files Main files:
 *  - tc_timeout_example2.c: This example application
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
 * - For button input, connect PORTB pin header to SWITCHES pin header with a
 *   10-pin cable.
 * - For LED output, connect PORTD pin header to LEDS pin header with a 10-pin
 *   cable.
 * - For 32KHz clock, connect a wire between 32KHz and TOSC2 on the AUX pin
 *   header.
 *
 * \section exampledescription Description of the example
 * This example will continuously sample button input from SW0 on STK600, and
 * then apply a software debounce filter to get a stable button press
 * indication. A valid button down-press will cause LED0 to toggle.
 *
 * This filter is implemented by starting a 50ms timeout on every change on the
 * button input sample. Any noise will cause the timeout to restart, but if the
 * signal is stable for a minimum of 50ms it will be considered as a valid
 * button press.
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

// Use an enum to get a timeout id index
enum {
	DEBOUNCE_TIMEOUT,
};

/**
 * \brief Length of debounce timeout, in ticks
 *
 * We want 50ms debounce timeout so we calculate that from the actual tick rate
 * defined by TC_TIMEOUT_TICK_HZ.
 */
#define DEBOUNCE_TICKS (50 * TC_TIMEOUT_TICK_HZ / 1000)

// Masks for the button and LED
#define BUTTON_MASK (1 << 0)
#define LED_MASK (1 << 0)

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
	bool button_state;
	bool button_last_state = false;

	// Set LED0 connected to port D to output
	DDRD |= LED_MASK;

	// Initialize the timeout internals before using it
	tc_timeout_init();

	// Enable global interrupts
	sei();

	while (true) {
		// Sample the button
		button_state = !(PINB & BUTTON_MASK);
		// Test for any change in state since last sample
		if (button_last_state != button_state) {
			/* Start debounce timeout. This will cancel any
			 * currently running timeout for selected id.
			 */
			tc_timeout_start_singleshot(DEBOUNCE_TIMEOUT,
					DEBOUNCE_TICKS);
			button_last_state = button_state;
		}

		// Check for debounce timeout expire
		if (tc_timeout_test_and_clear_expired(DEBOUNCE_TIMEOUT)) {
			// Toggle LED if button is pressed down
			if (button_state)
				toggle_led(LED_MASK);
		}
	}
}

