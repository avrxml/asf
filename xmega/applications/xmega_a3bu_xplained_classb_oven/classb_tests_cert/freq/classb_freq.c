/**
 * \file
 *
 * \brief CPU frequency test.
 *
 * \par Application note:
 *      AVR1610: Guide to IEC60730 Class B compliance with XMEGA
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
