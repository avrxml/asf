/**
 * \file
 *
 * \brief Unit test for PARC driver
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
#include "conf_test.h"

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test application for the PARC driver.
 * It contains 3 test case for the PARC module:
 * - Test PARC register write/read function.
 * - Test PARC data capture in polling way.
 * - Test PARC data capture in callback way.
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_uart_serial.h
 *
 * \section device_info Device Info
 * Only SAM4L devices can be used.
 * The unit tests have been tested with the following setup:
 * - sam4lc4c_sam4l_xplained_pro
 * - sam4lc8c_sam4l8_xplained_pro
 *
 * \section connection Board Connection
 * The connections on SAM4L Xplained Pro or SAM4L8 Xplained Pro should be:
 *  EXT3-P9 (PIN_PA06) -- EXT3-P15(PCCK)
 *  EXT1-P7 (PIN_PC00) -- EXT3-P8 (PCDATA0)
 *  EXT1-P8 (PIN_PC01) -- EXT3-P10(PCDATA1)
 *  EXT1-P6 (PIN_PC02) -- EXT4-P15(PCDATA2)
 *  EXT1-P15(PIN_PC03) -- EXT4-P7 (PCDATA3)
 *  EXT2-P7 (PIN_PC04) -- EXT4-P8 (PCDATA4)
 *  EXT2-P8 (PIN_PC05) -- EXT4-P10(PCDATA6)
 *  EXT2-P9 (PIN_PC06) -- EXT4-P9 (PCDATA7)
 *  EXT4-P5 (PIN_PC17) -- EXT4-P18(PCEN1)
 *  EXT4-P6 (PIN_PC18) -- EXT4-P17(PCEN2)
 * Please note the PCDATA5 is only connected to LCD connector (EXT5)
 * which can not be connected easily by plugging wires. So in this example
 * PCDATA5 is nor required to be connected.
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for ARM. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
 */

/* delay time between two captures */
#define DELAY_TIME          100

/* PARC module instance*/
struct parc_module module_inst;
/** PARC configure stuct */
struct parc_config config;

uint32_t captured_data = 0;
static bool callback_data_ready = false;


/**
 * \brief PARC input signal generating through GPIO port.
 *
 * \param risingedge  PARC sampling edge, rising edge(true),fall edge (false)
 * \param data        Data to send
 */
static void parc_port_input_simulation(bool risingedge, uint32_t data)
{
	/* PCCK signal simulation */
	if (risingedge) {
		ioport_set_pin_level(PIN_PCCK, IOPORT_PIN_LEVEL_LOW);
	} else {
		ioport_set_pin_level(PIN_PCCK, IOPORT_PIN_LEVEL_HIGH);
	}

	/* Place the data on port */
	place_data_to_port(data);

	/* PCCK signal simulation */
	if (risingedge) {
		ioport_set_pin_level(PIN_PCCK, IOPORT_PIN_LEVEL_HIGH);
	} else {
		ioport_set_pin_level(PIN_PCCK, IOPORT_PIN_LEVEL_LOW);
	}
}

/**
 * \brief Test PARC functions in polling way.
 *
 * \param test Current test case.
 */
static void run_parc_polled_test(const struct test_case *test)
{
	uint32_t input_data = 0x01;

	/* PARC config */
	parc_get_config_defaults(&config);
	parc_init(&module_inst, PARC, &config);
	parc_enable(&module_inst);
	parc_start_capture(&module_inst);

	for (uint32_t i = 0; i < 8; i++) {
		parc_port_input_simulation(true, input_data);
		delay_ms(DELAY_TIME);
		test_assert_true(test, parc_is_data_ready(&module_inst) == true,
				"Capture on rising edge failure!");
		parc_read(&module_inst, &captured_data);
		test_assert_true(test, captured_data == (input_data & DATA_MASK),
				"Wrong captured data!");
		input_data = 1 << i;
	}
	parc_stop_capture(&module_inst);
	parc_disable(&module_inst);
}

/**
 * \brief callback handler for PARC data ready interrupt.
 *
 * \param module The parc_module instance.
 */
static void parc_complete_callback(struct parc_module *const module)
{
	callback_data_ready = true;
	parc_read(module, &captured_data);
}

/**
 * \brief Test PARC functions in callback way.
 *
 * \param test Current test case.
 */
static void run_parc_callback_test(const struct test_case *test)
{
	uint32_t input_data = 0x01;
	parc_get_config_defaults(&config);
	parc_init(&module_inst, PARC, &config);
	parc_enable(&module_inst);

	parc_register_callback(&module_inst,
			(parc_callback_t)parc_complete_callback, PARC_CALLBACK_DATA_READY);
	parc_enable_interrupts(&module_inst, PARC_INTERRUPT_DRDY);
	parc_enable_callback(&module_inst, PARC_CALLBACK_DATA_READY);
	parc_start_capture(&module_inst);

	for (uint32_t i = 0; i < 8; i++) {
		callback_data_ready = false;
		parc_port_input_simulation(true, input_data);
		delay_ms(DELAY_TIME);
		test_assert_true(test, callback_data_ready == true,
				"Capture data failure");
		test_assert_true(test, captured_data == (input_data & DATA_MASK),
				"Wrong captured data!");
		input_data = 1 << i;
	}
	parc_disable_interrupts(&module_inst, PARC_INTERRUPT_DRDY);
	parc_disable_callback(&module_inst,PARC_CALLBACK_DATA_READY);
	parc_stop_capture(&module_inst);
	parc_disable(&module_inst);
}

/**
 * \brief Test PARC register write/read.
 *
 * \param test Current test case.
 */
static void run_parc_ctrl_test(const struct test_case *test)
{
	parc_get_config_defaults(&config);
	parc_init(&module_inst, PARC, &config);
	parc_enable(&module_inst);

	test_assert_true(test, parc_get_status(&module_inst) == PARC_STATUS_EN,
			"Test PARC enable: enable failed");

	parc_start_capture(&module_inst);
	test_assert_true(test, parc_get_status(&module_inst) ==
			(PARC_STATUS_CS | PARC_STATUS_EN),
			"Test PARC start: start failed");
	parc_stop_capture(&module_inst);
	parc_disable(&module_inst);
}

/**
 * \brief Run PARC driver unit tests.
 */
int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate   = CONF_UART_BAUDRATE,
		.charlength = CONF_UART_CHAR_LENGTH,
		.paritytype = CONF_UART_PARITY,
		.stopbits   = CONF_UART_STOP_BITS
	};
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();
	/* Configure console USART. */
	stdio_serial_init(CONF_UART, &usart_serial_options);

	parc_port_source_simulation_config();

	/* Define all the test cases. */
	DEFINE_TEST_CASE(parc_ctrl_test, NULL, run_parc_ctrl_test, NULL,
			"SAM PARC enable/disable test");
	DEFINE_TEST_CASE(parc_polled_test, NULL, run_parc_polled_test, NULL,
			"SAM PARC polled test.");
	DEFINE_TEST_CASE(parc_callback_test, NULL, run_parc_callback_test, NULL,
			"SAM PARC callback test.");

	/* Put test case addresses in an array. */
	DEFINE_TEST_ARRAY(parc_tests) = {
		&parc_ctrl_test,
		&parc_polled_test,
		&parc_callback_test
	};

	/* Define the test suite. */
	DEFINE_TEST_SUITE(parc_suite, parc_tests, "SAM PARC driver test suite");

	/* Run all tests in the test suite. */
	test_suite_run(&parc_suite);

	while(1){
		/* Busy-wait forever. */
	}
}
