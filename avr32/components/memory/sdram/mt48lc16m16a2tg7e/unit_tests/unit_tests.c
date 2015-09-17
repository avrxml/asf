/**
 * \file
 *
 * \brief Unit test functions for mt48LC16m16a2tg7 SDRAM component.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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
