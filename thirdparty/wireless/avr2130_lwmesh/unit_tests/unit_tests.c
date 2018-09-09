/**
 * \file
 *
 * \brief LWMesh Stack Unit Test.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 * This is the unit test application for the \ref group_lwmesh "LWMesh Stack".
 */

#include <stdint.h>
#include <stdbool.h>
#include <asf.h>
#include <string.h>
#include "board.h"
#include "phy.h"
#include "sys.h"
#include "nwk.h"
#include "sysTimer.h"
#include "sysclk.h"
#include "conf_usb.h"
#include "stdio_usb.h"
#include "conf_test.h"
#include "unit_tests.h"

static void unitDataConf(NWK_DataReq_t *req);
static void unitDataReq(void);
static void NetworkStatusTimerHandler(SYS_Timer_t *timer);

static SYS_Timer_t NetworkStatusTimer;
static bool result = false, timeout = false;

/**
 *
 * Initializes the clock system, board and USB.
 * Then runs the wireless task continuously.
 */
int main(void)
{
	SYS_Init();
	stdio_usb_init();
	while (1) {
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

static void run_lwmesh_nwk_darareq_test(const struct test_case *test)
{
	unitDataReq();
	while (1) {
		if (result) {
			break;
		}

		if (timeout) {
			test_assert_true(test, 0,
					"AVR2130_LWMesh - NWK Data request failed");
		}

		SYS_TaskHandler();
	}
}

void main_cdc_set_dtr(bool b_enable)
{
	if (b_enable) {
		DEFINE_TEST_CASE(lwmesh_nwk_darareq_test, NULL,
				run_lwmesh_nwk_darareq_test,
				NULL, "AVR2130_LWMesh - NWK Data Request");

		/* Put test case addresses in an array. */
		DEFINE_TEST_ARRAY(lwmesh_tests) = {
			&lwmesh_nwk_darareq_test,
		};

		/* Define the test suite. */
		DEFINE_TEST_SUITE(lwmesh_suite, lwmesh_tests,
				"AVR2130_LWMesh - unit test suite");

		/* Run all tests in the test suite. */
		test_suite_run(&lwmesh_suite);
	} else {
	}
}

static void unitDataConf(NWK_DataReq_t *req)
{
	SYS_TimerStop(&NetworkStatusTimer);
	result = true;
}

static void unitDataReq(void)
{
	NWK_DataReq_t nwkDataReq;
	uint8_t appMsg[10] = "UnitTest\n\r";
	nwkDataReq.dstAddr = 0;
	nwkDataReq.dstEndpoint = APP_ENDPOINT;
	nwkDataReq.srcEndpoint = APP_ENDPOINT;
	nwkDataReq.data = (uint8_t *)&appMsg;
	nwkDataReq.size = sizeof(appMsg);
	nwkDataReq.confirm = unitDataConf;
	NWK_DataReq(&nwkDataReq);
	NetworkStatusTimer.interval = 2000;
	NetworkStatusTimer.mode = SYS_TIMER_PERIODIC_MODE;
	NetworkStatusTimer.handler = NetworkStatusTimerHandler;
	SYS_TimerStart(&NetworkStatusTimer);
}

static void NetworkStatusTimerHandler(SYS_Timer_t *timer)
{
	timeout = true;
	(void)timer;
}
