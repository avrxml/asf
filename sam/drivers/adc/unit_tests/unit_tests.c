/**
 * \file
 *
 * \brief Unit tests for ADC2 driver.
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
#include <conf_test.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test application for ADC2 driver.
 * It consists of test cases for the following functionality:
 * - ADC in TC trigger mode test
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_uart_serial.h
 * - \ref conf_sleepmgr.h
 *
 * \section device_info Device Info
 * The SAM4N devices can be used.
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for ARM. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
 */

/** Conversion data is ready flag */
volatile bool is_data_ready = false;

/**
 * \brief Callback function for ADC conversion data is ready interrupt.
 */
static void adc_set_data_ready_flag(void)
{
	is_data_ready = true;
	adc_disable_interrupt(ADC, ADC_INTERRUPT_EOC_1);
	tc_stop(TC0, 0);
}

/**
 * \brief Configure to trigger ADC by TIOA output of timer.
 */
static void configure_tc_trigger(void)
{
	uint32_t ul_div = 0;
	uint32_t ul_tc_clks = 0;
	uint32_t ul_sysclk = sysclk_get_cpu_hz();

	/* Enable peripheral clock. */
	pmc_enable_periph_clk(ID_TC0);

	/* TIOA configuration */
	ioport_set_pin_mode(PIN_TC0_TIOA0, PIN_TC0_TIOA0_MUX);
	ioport_disable_pin(PIN_TC0_TIOA0);

	/* Configure TC for a 10Hz frequency and trigger on RC compare. */
	tc_find_mck_divisor(10, ul_sysclk, &ul_div, &ul_tc_clks, ul_sysclk);
	tc_init(TC0, 0, ul_tc_clks | TC_CMR_CPCTRG | TC_CMR_WAVE |
			TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_SET);
	TC0->TC_CHANNEL[0].TC_RA = (ul_sysclk / ul_div) / 2;
	TC0->TC_CHANNEL[0].TC_RC = (ul_sysclk / ul_div) / 1;

	/* Start the Timer. */
	tc_start(TC0, 0);
}

/**
 * \brief Test ADC in TC trigger mode,
 *        which also tests interrupt driven conversions.
 *
 * \param test Current test case.
 */
static void run_adc_tc_trig_test(const struct test_case *test)
{
	adc_channel_enable(ADC, ADC_CHANNEL_1);

	adc_set_trigger(ADC, ADC_TRIG_TIO_CH_0);

	adc_set_callback(ADC, ADC_INTERRUPT_EOC_1,
			adc_set_data_ready_flag, 1);

	configure_tc_trigger();

	delay_ms(2000);

	test_assert_true(test, is_data_ready == true,
			"ADC using TC trigger test failed");
}

/**
 * \brief Run ADC2 driver unit tests.
 */
int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate   = CONF_TEST_BAUDRATE,
		.paritytype = CONF_TEST_PARITY
	};

	/* Initialize the system clock and board */
	sysclk_init();
	board_init();

	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	adc_enable();

	struct adc_config adc_cfg;

	adc_get_config_defaults(&adc_cfg);

	adc_init(ADC, &adc_cfg);

#if defined(__GNUC__)
	setbuf(stdout, NULL);
#endif

	/* Define all the test cases */
	DEFINE_TEST_CASE(adc_tc_trig_test, NULL, run_adc_tc_trig_test, NULL,
			"ADC TC Trig test");

	/* Put test case addresses in an array */
	DEFINE_TEST_ARRAY(adc_tests) = {
		&adc_tc_trig_test,
	};

	/* Define the test suite */
	DEFINE_TEST_SUITE(adc_suite, adc_tests,
			"SAM4N ADC driver test suite");

	/* Run all tests in the test suite */
	test_suite_run(&adc_suite);

	while (1) {
		/* Busy-wait forever. */
	}
}
