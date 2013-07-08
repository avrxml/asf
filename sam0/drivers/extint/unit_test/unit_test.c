/**
 * \file
 *
 * \brief SAM D20 External Interrupt Unit test
 *
 * Copyright (C) 2013 Atmel Corporation. All rights reserved.
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
 * \mainpage EXTERNAL INTERRUPT UNIT TEST
 *
 * \section intro Introduction
 * This unit test carries out tests for the external interrupt driver.
 * It consists of test cases for the following functionalities:
 *      - Test for polled mode detection of external interrupt.
 *      - Test for callback mode detection of external interrupt.
 * Tests will be done for rising and falling edges of external signal.
 *
 * The following kit is required for carrying out the test:
 *      - SAM D20 Xplained Pro board
 *
 * \section Setup
 *
 * The following connection has to be made on the EXT1 header of the
 * SAM D20 Xplained Pro board using a jumper/wire.
 * EXT1 Header:
 *      - Pin 9 (PB06) <-----> Pin 10 (PB07)
 *
 * Once the connection is made the following has to be done:
 *      - Connect the SAM D20 Xplained Pro board to the computer using
 *        a micro USB cable.
 *      - Open the virtual COM port in a terminal application.
 * \note  The USB composite firmware running on the Embedded Debugger (EDBG)
 *        will enumerate as debugger, virtual COM port and EDBG data
 *        gateway.
 *      - Build the project, program the target and run the application.
 *        The terminal shows the results of the unit test.
 *
 * \section Description
 *
 *      - The unit test configures external interrupt on PB06 pin (channel 10)
 *        to detect falling edge.
 *      - Logic level on PB07 is changed from high to low (falling edge) and
 *        the channel is checked for interrupt detection.
 *      - The test is repeated for rising edge and with callback enabled.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 * Support and FAQ: http://support.atmel.no/
 */

#include <asf.h>
#include <stdio_serial.h>
#include <string.h>

/* GPIO pin used for testing the interrupts */
#define GPIO_TEST_PIN_EXTINT  EXT1_PIN_10

/* External interrupt channel number on PB06 */
#define EIC_TEST_CHANNEL      EXT1_IRQ_INPUT
#define EIC_TEST_PIN          EXT1_IRQ_PIN
#define EIC_TEST_PIN_MUX      EXT1_IRQ_PINMUX

/* Structure for UART module connected to EDBG (used for unit test output) */
struct usart_module cdc_uart_module;
/* Structure for pin configuration */
struct port_config pin_conf;
/* Structure for external interrupt channel configuration */
struct extint_chan_conf eic_conf;
/* Flag for test result used during polling test */
volatile bool result = false;
/* Interrupt flag used during callback test */
volatile bool interrupt_flag = false;

/**
 * \internal
 * \brief External interrupt callback function
 *
 * Called by external interrupt driver on interrupt detection.
 *
 * \param channel Channel number of the interrupt (not used)
 */
static void extint_user_callback(uint32_t channel)
{
	interrupt_flag = true;
}

/**
 * \brief Initialize the USART for unit test
 *
 * Initializes the SERCOM USART (SERCOM4) used for sending the
 * unit test status to the computer via the EDBG CDC gateway.
 */
static void cdc_uart_init(void)
{
	struct usart_config cdc_uart_config;

	/* Configure USART for unit test output */
	usart_get_config_defaults(&cdc_uart_config);
	cdc_uart_config.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	cdc_uart_config.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	cdc_uart_config.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	cdc_uart_config.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	cdc_uart_config.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
	cdc_uart_config.baudrate    = 115200;
	stdio_serial_init(&cdc_uart_module, EDBG_CDC_MODULE, &cdc_uart_config);
	usart_enable(&cdc_uart_module);
	/* Enable transceivers */
	usart_enable_transceiver(&cdc_uart_module, USART_TRANSCEIVER_TX);
	usart_enable_transceiver(&cdc_uart_module, USART_TRANSCEIVER_RX);
}

/**
 * \internal
 * \brief Setup function for polled mode test.
 *
 * This function sets up the GPIO pin and the external interrupt channel.
 *
 * \param test Current test case.
 */
static void setup_extint_polled_mode_test(const struct test_case *test)
{
	/* Configure the GPIO pin */
	port_get_config_defaults(&pin_conf);
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	pin_conf.input_pull = PORT_PIN_PULL_NONE;
	port_pin_set_config(GPIO_TEST_PIN_EXTINT, &pin_conf);
	port_pin_set_output_level(GPIO_TEST_PIN_EXTINT, true);

	/* Enable the external interrupt module */
	extint_enable();
	/* Configure the external interrupt channel */
	extint_chan_get_config_defaults(&eic_conf);
	eic_conf.gpio_pin           = EIC_TEST_PIN;
	eic_conf.gpio_pin_mux       = EIC_TEST_PIN_MUX;
	eic_conf.gpio_pin_pull      = EXTINT_PULL_UP;
	eic_conf.detection_criteria = EXTINT_DETECT_FALLING;
	extint_chan_set_config(EIC_TEST_CHANNEL, &eic_conf);
}

/**
 * \internal
 * \brief Cleanup function for polled mode test.
 *
 * This function disables the external interrupt module.
 *
 * \param test Current test case.
 */
static void cleanup_extint_polled_mode_test(const struct test_case *test)
{
	/* Disable the external interrupt module */
	extint_disable();
}

/**
 * \internal
 * \brief Test for external interrupt detection by polling.
 *
 * This test changes the logic level of PB07 from high to low
 * so that the external interrupt channel detects the edge.
 *
 * Detection is tested for both rising and falling edges.
 *
 * \param test Current test case.
 */
static void run_extint_polled_mode_test(const struct test_case *test)
{
	/* Testing the falling edge detection */
	port_pin_set_output_level(GPIO_TEST_PIN_EXTINT, false);
	/* Wait for the pin level to stabilize */
	delay_ms(1);
	result = extint_chan_is_detected(EIC_TEST_CHANNEL);
	test_assert_true(test, result,
			"External interrupt falling edge detection by polling failed");
	extint_chan_clear_detected(EIC_TEST_CHANNEL);

	/* Testing the rising edge detection */
	result = false;
	eic_conf.detection_criteria = EXTINT_DETECT_RISING;
	extint_chan_set_config(EIC_TEST_CHANNEL, &eic_conf);
	/* Wait for the pin level to stabilize */
	delay_ms(1);
	port_pin_set_output_level(GPIO_TEST_PIN_EXTINT, true);
	result = extint_chan_is_detected(EIC_TEST_CHANNEL);
	test_assert_true(test, result,
			"External interrupt rising edge detection by polling failed");
	extint_chan_clear_detected(EIC_TEST_CHANNEL);
}

/**
 * \internal
 * \brief Setup function for callback mode test.
 *
 * This function sets up the GPIO pin, the external interrupt channel &
 * the callback function.
 *
 * \param test Current test case.
 */
static void setup_extint_callback_mode_test(const struct test_case *test)
{
	/* Configure the GPIO pin */
	port_get_config_defaults(&pin_conf);
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	pin_conf.input_pull = PORT_PIN_PULL_NONE;
	port_pin_set_config(GPIO_TEST_PIN_EXTINT, &pin_conf);
	port_pin_set_output_level(GPIO_TEST_PIN_EXTINT, true);

	/* Enable the external interrupt module */
	extint_enable();
	/* Configure the external interrupt channel */
	extint_chan_get_config_defaults(&eic_conf);
	eic_conf.gpio_pin           = EIC_TEST_PIN;
	eic_conf.gpio_pin_mux       = EIC_TEST_PIN_MUX;
	eic_conf.gpio_pin_pull      = EXTINT_PULL_UP;
	eic_conf.detection_criteria = EXTINT_DETECT_FALLING;
	extint_chan_set_config(EIC_TEST_CHANNEL, &eic_conf);
	/* Register and enable the callback function */
	extint_register_callback(extint_user_callback,
			EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(EIC_TEST_CHANNEL,
			EXTINT_CALLBACK_TYPE_DETECT);
}

/**
 * \internal
 * \brief Cleanup function for callback mode test.
 *
 * This function disables the callback & external interrupt module.
 *
 * \param test Current test case.
 */
static void cleanup_extint_callback_mode_test(const struct test_case *test)
{
	/* Unregister and disable the callback function */
	extint_unregister_callback(extint_user_callback,
			EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_disable_callback(EIC_TEST_CHANNEL,
			EXTINT_CALLBACK_TYPE_DETECT);
	/* Disable the external interrupt module */
	extint_disable();
}

/**
 * \internal
 * \brief Test for external interrupt detection using callback.
 *
 * This test changes the logic level of PB07 from high to low
 * so that the external interrupt channel detects the edge
 * and calls the callback function.
 *
 * Detection is tested for both rising and falling edges.
 *
 * \param test Current test case.
 */
static void run_extint_callback_mode_test(const struct test_case *test)
{
	uint16_t timeout_cycles;

	/* Testing the falling edge detection */
	interrupt_flag = false;
	timeout_cycles = 100;
	port_pin_set_output_level(GPIO_TEST_PIN_EXTINT, false);
	do {
		timeout_cycles--;
		if (interrupt_flag) {
			break;
		}
	} while (timeout_cycles != 0);
	test_assert_true(test, timeout_cycles > 0,
			"Timeout in interrupt detection");
	test_assert_true(test, interrupt_flag,
			"External interrupt falling edge detection failed");

	/* Testing the rising edge detection */
	eic_conf.detection_criteria = EXTINT_DETECT_RISING;
	extint_chan_set_config(EIC_TEST_CHANNEL, &eic_conf);
	interrupt_flag = false;
	timeout_cycles = 100;
	port_pin_set_output_level(GPIO_TEST_PIN_EXTINT, true);
	do {
		timeout_cycles--;
		if (interrupt_flag) {
			break;
		}
	} while (timeout_cycles != 0);
	test_assert_true(test, timeout_cycles > 0,
			"Timeout in interrupt detection");
	test_assert_true(test, interrupt_flag,
			"External interrupt rising edge detection failed");
}

/**
 * \brief Run External Interrupt unit tests
 *
 * Initializes the system and serial output, then sets up the
 * External Interrupt unit test suite and runs it.
 */
int main(void)
{
	system_init();
	delay_init();
	cdc_uart_init();

	/* Define Test Cases */
	DEFINE_TEST_CASE(extint_polled_mode_test,
			setup_extint_polled_mode_test,
			run_extint_polled_mode_test,
			cleanup_extint_polled_mode_test,
			"Testing external interrupt by polling");

	DEFINE_TEST_CASE(extint_callback_mode_test,
			setup_extint_callback_mode_test,
			run_extint_callback_mode_test,
			cleanup_extint_callback_mode_test,
			"Testing external interrupt using callback");

	/* Put test case addresses in an array */
	DEFINE_TEST_ARRAY(extint_tests) = {
		&extint_polled_mode_test,
		&extint_callback_mode_test,
	};

	/* Define the test suite */
	DEFINE_TEST_SUITE(extint_test_suite, extint_tests,
			"SAM D20 External Interrupt driver test suite");

	/* Run all tests in the suite*/
	test_suite_run(&extint_test_suite);

	while (true) {
		/* Intentionally left empty */
	}
}
