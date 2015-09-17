/**
 * \file
 *
 * \brief Timeout driver example 1 for XMEGA
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
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
 * This simple example shows how to use the \ref timeout_group to toggle
 * 4-8 LEDs (depending on the board) at different speeds.
 *
 * \section files Main files
 *
 * - example1.c:     The example application
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
 * The example will configure a tick speed of 4 Hz and use 4/8 different
 * periodic timeouts channels to toggle 4/8 different LEDs at different speeds.
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
#include <pmic.h>
#include <board.h>
#include <conf_timeout.h>
#include <conf_example.h>

//! Enum with index ID for all timeout channels
enum {
	EXAMPLE_TIMEOUT_0,
	EXAMPLE_TIMEOUT_1,
	EXAMPLE_TIMEOUT_2,
	EXAMPLE_TIMEOUT_3,
	EXAMPLE_TIMEOUT_4,
	EXAMPLE_TIMEOUT_5,
	EXAMPLE_TIMEOUT_6,
	EXAMPLE_TIMEOUT_7,
};

/**
 * \brief Timeout example 1 main application routine
 */
int main( void )
{
	// Initialize drivers
	sysclk_init();
	board_init();
	pmic_init();
	timeout_init();
	cpu_irq_enable();

	// Start 8 different timeouts
	timeout_start_periodic(EXAMPLE_TIMEOUT_0, 1);
	timeout_start_periodic(EXAMPLE_TIMEOUT_1, 2);
	timeout_start_periodic(EXAMPLE_TIMEOUT_2, 4);
	timeout_start_periodic(EXAMPLE_TIMEOUT_3, 8);
#if (BOARD != XMEGA_A3BU_XPLAINED)
	timeout_start_periodic(EXAMPLE_TIMEOUT_4, 16);
	timeout_start_periodic(EXAMPLE_TIMEOUT_5, 32);
	timeout_start_periodic(EXAMPLE_TIMEOUT_6, 64);
	timeout_start_periodic(EXAMPLE_TIMEOUT_7, 128);
#endif

	// Initialize LEDs
	gpio_set_pin_group_high(LED_PORT, LED_PORT_MASK);

	// Toggle LED when a timeout occurs
	while (1) {
		if(timeout_test_and_clear_expired(EXAMPLE_TIMEOUT_0))
			gpio_toggle_pin(LED0_GPIO);
		if(timeout_test_and_clear_expired(EXAMPLE_TIMEOUT_1))
			gpio_toggle_pin(LED1_GPIO);
		if(timeout_test_and_clear_expired(EXAMPLE_TIMEOUT_2))
			gpio_toggle_pin(LED2_GPIO);
		if(timeout_test_and_clear_expired(EXAMPLE_TIMEOUT_3))
			gpio_toggle_pin(LED3_GPIO);
#if (BOARD != XMEGA_A3BU_XPLAINED)
		if(timeout_test_and_clear_expired(EXAMPLE_TIMEOUT_4))
			gpio_toggle_pin(LED4_GPIO);
		if(timeout_test_and_clear_expired(EXAMPLE_TIMEOUT_5))
			gpio_toggle_pin(LED5_GPIO);
		if(timeout_test_and_clear_expired(EXAMPLE_TIMEOUT_6))
			gpio_toggle_pin(LED6_GPIO);
		if(timeout_test_and_clear_expired(EXAMPLE_TIMEOUT_7))
			gpio_toggle_pin(LED7_GPIO);
#endif
	}
}
