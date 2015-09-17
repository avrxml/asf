/**
 * \file  example2.c
 *
 * \brief XMEGA timeout example 2
 *
 * Copyright (C) 2010-2015 Atmel Corporation. All rights reserved.
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
 * This simple example shows how to setup a debounce filter on a button
 * using the the \ref timeout_group.
 *
 * \section files Main files
 *
 * - example2.c:     The example application
 * - timeout.c:      The XMEGA timeout service implementation
 * - timeout.h:      The XMEGA timeout service API definition
 * - conf_timeout.h  Configuration of the timeout service
 *
 * \section driverinfo Timeout service
 * The timeout service can be found \ref timeout_group "here".
 *
 * \section deviceinfo Device Info
 * All XMEGA devices with RTC or RTC32 can be used
 *
 * \section exampledescription Description of the example
 * This example will demonstrate the effect of a debounce filter on a push
 * button. Button0 includes the debounce filter and toggles LED0 when toggled.
 * Button1 will toggle LED1, but without the debounce filter causing a
 * jittering and unpredictable behaviour.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include <conf_timeout.h>
#include <conf_example.h>

//! Enum to get timeout index
enum {
	DEBOUNCE_TIMEOUT,
};

//! Length of debounce timeout, in ticks
#define DEBOUNCE_TICKS (50 * TIMEOUT_TICK_HZ / 1000)

/**
 * \brief Timeout example 2 main application routine
 */
int main( void )
{
	// Initialize drivers
	sysclk_init();
	board_init();
	pmic_init();
	timeout_init();
	cpu_irq_enable();

	bool button_pressed;
	bool button_previous_state_pressed = false;

	// Initialize LEDs
	gpio_set_pin_group_high(LED_PORT, LED_PORT_MASK);

	while (1) {

		button_pressed = gpio_pin_is_low(GPIO_PUSH_BUTTON_0);

		// Test for any change in state since last sample
		if (button_previous_state_pressed != button_pressed) {
			/* Start debounce timeout. This will cancel any
			   currently running timeout for selected id. */
			timeout_start_singleshot(DEBOUNCE_TIMEOUT,
					DEBOUNCE_TICKS);
			button_previous_state_pressed = button_pressed;
		}

		// Check for debounce timeout expire
		if (timeout_test_and_clear_expired(DEBOUNCE_TIMEOUT)) {
			// Toggle LED0 if button is pressed down
			if (button_pressed) {
				gpio_toggle_pin(LED0_GPIO);
			}
		}

		// No debounce filter on button 1
		if (gpio_pin_is_low(GPIO_PUSH_BUTTON_1)) {
			gpio_toggle_pin(LED1_GPIO);
		}
	}
}
