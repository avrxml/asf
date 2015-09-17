/**
 * \file
 *
 * \brief LWMesh Stack Unit Test.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
