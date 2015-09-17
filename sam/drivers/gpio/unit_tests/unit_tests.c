/**
 * \file
 *
 * \brief Unit tests for GPIO driver.
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include <conf_test.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test application for GPIO driver.
 * It consists of test cases for the following functionality:
 * - GPIO interupt support
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_uart_serial.h
 *
 * \section device_info Device Info
 * SAM4L devices can be used.
 * This example has been tested with the following setup:
 * - sam4l4c_sam4l_ek (Need to connect PA06 and PB05 on the board)
 * - sam4l4c_sam4l_xplained_pro (Need to connect EXT3/PIN9 and EXT4/PIN3 on the board)
 * - sam4l4c_sam4l8_xplained_pro (Need to connect EXT3/PIN9 and EXT4/PIN3 on the board)
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for ARM. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
 */

/** \name Unit test configuration */
/* @{ */
/**
 * \def CONF_TEST_USART
 * \brief USART to redirect STDIO to
 */

/**
 * \def CONF_TEST_BAUDRATE
 * \brief Baudrate of USART
 */

/**
 * \def CONF_TEST_CHARLENGTH
 * \brief Character length (bits) of USART
 */

/**
 * \def CONF_TEST_PARITY
 * \brief Parity mode of USART
 */

/**
 * \def CONF_TEST_STOPBITS
 * \brief Stopbit configuration of USART
 */
/* @} */

volatile bool gpio_int_happened = false;

/**
 * GPIO pin interrupt callback.
 */
static void gpio_pin_callback(void)
{
	gpio_int_happened = true;
}

/**
 * \brief Test GPIO interrupt.
 *
 * \param test Current test case.
 */
static void run_gpio_int_test(const struct test_case *test)
{
	/* Configure an input pin to trigger an interrupt on falling edge */
	ioport_set_pin_dir(CONF_TEST_GPIO_IN, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(CONF_TEST_GPIO_IN, IOPORT_MODE_PULLUP |
			IOPORT_MODE_GLITCH_FILTER);
	ioport_set_pin_sense_mode(CONF_TEST_GPIO_IN, IOPORT_SENSE_FALLING);
	gpio_set_pin_callback(CONF_TEST_GPIO_IN, gpio_pin_callback, 1);
	gpio_enable_pin_interrupt(CONF_TEST_GPIO_IN);

	/* Configure an output pin */
	ioport_set_pin_dir(CONF_TEST_GPIO_OUT, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(CONF_TEST_GPIO_OUT, IOPORT_PIN_LEVEL_HIGH);

	/* Trigger an interrupt */
	gpio_int_happened = false;
	ioport_set_pin_level(CONF_TEST_GPIO_OUT, IOPORT_PIN_LEVEL_LOW);
	delay_ms(5);
	test_assert_true(test, gpio_int_happened,
			"No interrupt has been triggered!");
}

/**
 * \brief Run GPIO driver unit tests.
 */
int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate = CONF_TEST_BAUDRATE,
		.charlength = CONF_TEST_CHARLENGTH,
		.paritytype = CONF_TEST_PARITY,
		.stopbits = CONF_TEST_STOPBITS
	};

	sysclk_init();
	board_init();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	/* Define all the test cases */
	DEFINE_TEST_CASE(gpio_int_test, NULL, run_gpio_int_test, NULL,
			"Test GPIO driver with interrupt support.");

	/* Put test case addresses in an array */
	DEFINE_TEST_ARRAY(gpio_tests) = {
		&gpio_int_test
	};

	/* Define the test suite */
	DEFINE_TEST_SUITE(gpio_suite, gpio_tests, "GPIO driver test suite");

	/* Run all tests in the test suite */
	test_suite_run(&gpio_suite);

	while (1) {
		/* Busy-wait forever. */
	}
}
