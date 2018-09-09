/**
 * \file
 *
 * \brief Unit test functions for mt48LC16m16a2tg7 SDRAM component.
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include "unit_tests.h"

/**
 * \brief Test the data integrity
 *
 * Tests the integrity of the entire SDRAM by filling it a sequence of
 * incrementing 32-bit values and then verifying its content.
 *
 * \param test Current test case.
 */
void run_data_integrity_test(const struct test_case *test)
{
	struct ebi_test_params *params;
	volatile unsigned long *base;
	uint32_t               size;
	uint32_t               i;
	uint32_t               tmp;
	params = (struct ebi_test_params *)test_get_data();
	base = params->base;
	size = params->size >> 2;

	// Fill memory with a known pattern.
	for (i = 0; i < size; i++) {
		base[i] = i;
	}

	// Check each location and invert it for the second pass.
	for (i = 0; i < size; i++) {
		tmp = base[i];
		test_assert_true(test, tmp == i,
				"Read 0x%08lx @ 0x%08lx, expected 0x%08lx",
				tmp, (uint32_t)i, i);
	}
}
