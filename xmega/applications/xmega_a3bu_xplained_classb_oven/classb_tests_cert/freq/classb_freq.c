/**
 * \file
 *
 * \brief CPU frequency test.
 *
 * \par Application note:
 *      AVR1610: Guide to IEC60730 Class B compliance with XMEGA
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include "classb_freq.h"

/**
 * \ingroup cpu_freq_func
 * \brief Counter variable for the number of TC overflows.
 *
 * This variable is used to keep the MSW (bits 31 to 16) of the TC count.
 */
volatile uint16_t classb_tc_ovf_cnt = 0;

/**
 * \brief Set up the TC for use in the frequency test.
 *
 * This configures the period, prescaler and overflow interrupt.
 */
void classb_freq_setup_timer(void)
{
	/* Set up period as configured in the header file */
	CLASSB_TEST_TC.PER = CLASSB_TC_PER;

	/* Set up overflow interrupt */
	CLASSB_TEST_TC.INTCTRLA = TC_OVFINTLVL_LO_gc;

	/* Start the TC with the prescaling factor configured in the header file
	 */
	CLASSB_TEST_TC.CTRLA = CLASSB_TC_PRESCALER_gc;

	/* Reset count */
	CLASSB_TEST_TC.CNT = 0;
}

/**
 * \brief Callback function for the RTC interrupt.
 *
 * This function calculates the difference between the TC count and the
 * predefined reference. The error handler \ref CLASSB_ERROR_HANDLER_FREQ() is
 * called if the relative difference is larger than the tolerance set with
 * \ref CLASSB_TC_MAX_DIFF.
 */
void classb_freq_callback(void)
{
	/* Read the count in the TC and include the overflow counter as MSB */
	volatile uint32_t tccount = CLASSB_TEST_TC.CNT;
	tccount |= ((uint32_t)classb_tc_ovf_cnt) << 16;

	/* Compute the absolute difference between reference and counter */
	tccount = (tccount > CLASSB_TC_COUNT_REF)
			? (tccount - CLASSB_TC_COUNT_REF)
			: (CLASSB_TC_COUNT_REF - tccount);

	/* If the difference was larger than expected, the error should be
	 * handled
	 */
	if (tccount > CLASSB_TC_MAX_DIFF) {
		CLASSB_ERROR_HANDLER_FREQ();
	}

	/* Reset the overflow counter */
	classb_tc_ovf_cnt = 0;

	/* Reset TC */
	CLASSB_TEST_TC.CNT = 0;
}

/**
 * \brief TC overflow interrupt.
 *
 * This counts the number of TC overflows and the error handler is called if
 * the configurable maximum is exceeded. The purpose of the latter is to detect
 * RTC failure: the TC is reset in the RTC interrupt. If the RTC interrupt
 * is too slow, the number of TC overflows will exceed the limit. Note that
 * if the TC frequency increases abruptly, the error would probably be detected
 * here.
 */
void classb_freq_tc_callback(void)
{
	/* Increase the overflow counter. If the TC had more overflows than
	 * expected, the error should be handled
	 */
	if (classb_tc_ovf_cnt++ >= CLASSB_TC_MAX_OVF_COUNT) {
		CLASSB_ERROR_HANDLER_FREQ();
	}
}
