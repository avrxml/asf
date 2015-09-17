/**
 * \file
 *
 * \brief AT86RFx Wireless Module Unit Test.
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

/*! \mainpage
 * \section intro Introduction
 * This is the unit test application for the \ref at86rfx.h "AT86RFx component".
 * It consists of test cases for the following:
 * - Module initialization
 * - Read/write module register
 * - Frame transmission
 *
 * \section files Main Files
 * - \ref unit_tests.c: AT86RFx Wireless unit tests application.
 * - \ref at86rfx_driver.c: The AT86RFx module driver
 * - \ref at86rfx_driver.h: The AT86RFx module driver header
 * - \ref conf_test.h: test configuration file
 * - \ref conf_board.h: board initialization process configuration
 * - \ref conf_clock.h: clock specific initialization
 * - \ref conf_sleepmgr.h: sleep mode specific configuration
 * - \ref conf_spi_master.h: SPI master mode specific configuration
 * - \ref conf_at86rfx.h: AT86RFx module driver configuration (including SPI
 * service selection)
 * - \ref conf_usb.h: USB specific configuration
 *
 * \section deviceinfo Device Info
 * All AVR devices can be used. This application has been tested
 * with the following setup:
 *   - RZ600 Evaluation kits with a AT86RFx connected to an SPI interface.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 */

#include <stdint.h>
#include <stdbool.h>
#include <asf.h>
#include <string.h>
#include "board.h"
#include "sysclk.h"
#include "conf_usb.h"
#include "stdio_usb.h"
#include "conf_test.h"
#include "unit_tests.h"
#include "common_sw_timer.h"
#include "tal.h"
#include "ieee_const.h"

#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
#ifdef CHINESE_BAND
#define DEFAULT_CHANNEL                 (0)
#define DEFAULT_CHANNEL_PAGE            (5)
#else
#define DEFAULT_CHANNEL                 (1)
#define DEFAULT_CHANNEL_PAGE            (0)
#endif  /* #ifdef CHINESE_BAND */
#else
#define DEFAULT_CHANNEL                 (20)
#define DEFAULT_CHANNEL_PAGE            (0)
#endif  /* #if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B)) */

/**
 * \brief Run Wireless Module unit tests
 *
 * Initializes the clock system, board and USB.
 * Then runs the wireless task continuously.
 */
int main(void)
{
	irq_initialize_vectors();
	sysclk_init();

	/* Initialize the board.
	 * The board-specific conf_board.h file contains the configuration of
	 * the board initialization.
	 */
	board_init();

	sw_timer_init();
	tal_init();
	/* Enable interrupts */
	cpu_irq_enable();

	stdio_usb_init();

	while (1) {
		tal_task();
	}
}

/**
 * \brief Performs a Reset check on AT86RFx module
 *
 * This function will simply test the output of the function
 * \ref tal_reset and returns an error in case of failure.
 *
 * \param test Current test case.
 */

static void run_tal_reset_test(const struct test_case *test)
{
	retval_t status;

	status = tal_reset(true);
	test_assert_true(test, status == MAC_SUCCESS,
			"AVR2025_MAC - TAL Reset request failed");
}

static void run_tal_pib_set_test(const struct test_case *test)
{
	retval_t status;

	uint8_t temp;

	temp = DEFAULT_CHANNEL;

	status = tal_pib_set(phyCurrentChannel, (pib_value_t *)&temp);

	test_assert_true(test, status == MAC_SUCCESS,
			"AVR2025_MAC - TAL Setting Current Channel failed");
	temp = DEFAULT_CHANNEL_PAGE;
	status = tal_pib_set(phyCurrentPage, (pib_value_t *)&temp);
	test_assert_true(test, status == MAC_SUCCESS,
			"AVR2025_MAC - TAL Setting Current Page failed");
}

void main_cdc_set_dtr(bool b_enable)
{
	if (b_enable) {
		DEFINE_TEST_CASE(tal_reset_test, NULL, run_tal_reset_test,
				NULL, "AVR2025_MAC - TAL Reset request");
		DEFINE_TEST_CASE(tal_pib_set_test, NULL,
				run_tal_pib_set_test, NULL,
				"AVR2025_MAC - TAL PIB Set test");

		/* Put test case addresses in an array. */
		DEFINE_TEST_ARRAY(tal_tests) = {
			&tal_reset_test,
			&tal_pib_set_test
		};

		/* Define the test suite. */
		DEFINE_TEST_SUITE(tal_suite, tal_tests,
				"AVR2025_MAC - TAL unit test suite");

		/* Run all tests in the test suite. */
		test_suite_run(&tal_suite);
	} else {
	}
}

#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1) || defined(__DOXYGEN__)
void tal_ed_end_cb(uint8_t energy_level)
{
}

#endif /* (MAC_SCAN_ED_REQUEST_CONFIRM == 1) */

void tal_rx_frame_cb(frame_info_t *rx_frame)
{
}

void tal_tx_frame_done_cb(retval_t status, frame_info_t *frame)
{
}
