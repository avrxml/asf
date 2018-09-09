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

/*
 * === INCLUDES ============================================================ */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>

#include <asf.h>

#include "app_config.h"
#include "conf_board.h"
#include "pal.h"
#include "led.h"
#include "delay.h"
#include "tal.h"
#include "vendor_data.h"
#include "pb_pairing.h"
#include "rf4ce.h"
#include "common_sw_timer.h"

FLASH_DECLARE(uint16_t VendorIdentifier) = (uint16_t)NWKC_VENDOR_IDENTIFIER;
FLASH_DECLARE(uint8_t vendor_string[7]) = NWKC_VENDOR_STRING;
FLASH_DECLARE(uint8_t app_user_string[15]) = APP_USER_STRING;
#ifdef ZRC_CMD_DISCOVERY
FLASH_DECLARE(uint8_t supported_cec_cmds[32]) = SUPPORTED_CEC_CMDS;
#endif

static uint8_t nlme_reset_conf_rcvd = false;
static uint8_t nlme_reset_conf_status = FAILURE;
static uint8_t nlme_auto_discovery_conf_rcvd = false;
static uint8_t nlme_auto_discovery_conf_status = FAILURE;

static void nlme_reset_confirm(nwk_enum_t Status);
static void nlme_auto_discovery_confirm(nwk_enum_t Status);

/**
 * \brief Run Wireless Module unit tests
 *
 * Initializes the clock system, board and USB.
 * Then runs the wireless task continuously.
 */
int main(void)
{
	irq_initialize_vectors();
	board_init();
	sysclk_init();

	sw_timer_init();
	nwk_init();
	/* Enable interrupts */
	cpu_irq_enable();

	stdio_usb_init();

	while (1) {
		nwk_task();
	}
}

static void run_nlme_reset_test(const struct test_case *test)
{
	nlme_reset_request(true
#ifdef RF4CE_CALLBACK_PARAM
			, (FUNC_PTR)nlme_reset_confirm
#endif
			);
	while (!nlme_reset_conf_rcvd) {
		nwk_task();
	}
	test_assert_true(test, nlme_reset_conf_status == NWK_SUCCESS,
			"NWK Reset request failed");
}

static void run_nlme_auto_discovery_test(const struct test_case *test)
{
	dev_type_t RecDevTypeList[DEVICE_TYPE_LIST_SIZE];
	profile_id_t RecProfileIdList[PROFILE_ID_LIST_SIZE];

	RecDevTypeList[0] = (dev_type_t)SUPPORTED_DEV_TYPE_0;
	RecProfileIdList[0] = SUPPORTED_PROFILE_ID_0;
	nlme_auto_discovery_request( 0x13,
			RecDevTypeList,
			RecProfileIdList,
			0x08,
#ifdef RF4CE_CALLBACK_PARAM
			(FUNC_PTR)nlme_auto_discovery_confirm
#endif
			);
	while (!nlme_auto_discovery_conf_rcvd) {
		nwk_task();
	}
	test_assert_true(test, (nlme_auto_discovery_conf_status == NWK_SUCCESS) ||
			(nlme_auto_discovery_conf_status ==
			NWK_DISCOVERY_TIMEOUT),
			"NWK DISCOVERY test failed");
}

static void nlme_reset_confirm(nwk_enum_t Status)
{
	nlme_reset_conf_rcvd = true;
	nlme_reset_conf_status = Status;
}

static void nlme_auto_discovery_confirm(nwk_enum_t Status)
{
	nlme_auto_discovery_conf_rcvd = true;
	nlme_auto_discovery_conf_status = Status;
}

void main_cdc_set_dtr(bool b_enable)
{
	if (b_enable) {
		DEFINE_TEST_CASE(nlme_reset_test, NULL, run_nlme_reset_test,
				NULL, "NWK Reset request");
		DEFINE_TEST_CASE(nlme_auto_discovery_test, NULL,
				run_nlme_auto_discovery_test, NULL,
				"NWK DISCOVERY test (this covers all ASF drivers/services used");

		/* Put test case addresses in an array. */
		DEFINE_TEST_ARRAY(nwk_tests) = {
			&nlme_reset_test,
			&nlme_auto_discovery_test
		};

		/* Define the test suite. */
		DEFINE_TEST_SUITE(nwk_suite, nwk_tests,
				"NWK unit test suite");

		/* Run all tests in the test suite. */
		test_suite_run(&nwk_suite);
	} else {
	}
}
