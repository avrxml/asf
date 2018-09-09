/**
 * \file
 *
 * \brief AT86RFx Wireless Module Unit Test.
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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
 * <A href="http://www.microchip.com/">Microchip</A>.\n
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
#include "tfa.h"
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
	tfa_init();
	/* Enable interrupts */
	cpu_irq_enable();

	stdio_usb_init();

	while (1) {
		tal_task();
	}
}

/**
 * \brief Performs a initialization check on AT86RFx module
 *
 * This function will simply test the output of the function
 * \ref at86rfx_init and returns an error in case of failure.
 *
 * \param test Current test case.
 */

static void run_tfa_pib_set_test(const struct test_case *test)
{
	retval_t status;

	int8_t temp;

	temp = TFA_PIB_RX_SENS_DEF;

	status = tfa_pib_set(TFA_PIB_RX_SENS, (void *)&temp);

	test_assert_true(test, status == MAC_SUCCESS,
			"AVR2025_MAC - TFA Setting RX Sensitivity failed");
}

void main_cdc_set_dtr(bool b_enable)
{
	if (b_enable) {
		DEFINE_TEST_CASE(tfa_pib_set_test, NULL,
				run_tfa_pib_set_test, NULL,
				"AVR2025_MAC - TFA PIB Set RX Sensitivity ");

		/* Put test case addresses in an array. */
		DEFINE_TEST_ARRAY(tfa_tests) = {
			&tfa_pib_set_test
		};

		/* Define the test suite. */
		DEFINE_TEST_SUITE(tfa_suite, tfa_tests,
				"AVR2025_MAC - TFA unit test suite");

		/* Run all tests in the test suite. */
		test_suite_run(&tfa_suite);
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
