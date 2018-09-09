/**
 * \file
 *
 * \brief Timeout driver example 1 for XMEGA
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
 * <A href="http://www.microchip.com//">Microchip</A>.\n
 * Support and FAQ: https://www.microchip.com/support/
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
