/**
 * \file  example2.c
 *
 * \brief XMEGA timeout example 2
 *
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
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
 * <A href="http://www.microchip.com//">Microchip</A>.\n
 * Support and FAQ: https://www.microchip.com/support/
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
