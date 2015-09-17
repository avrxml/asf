/**
 * \file
 *
 * \brief Common test functions for External Bus Interface
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
#include "common_tests.h"

/**
 * \brief Test the data bus
 *
 * Tests the data bus for stuck and coupled lines by writing and verifying bit
 * patterns in RAM.
 *
 * First, unique 32-bit patterns with a single set bit are written to successive
 * locations in memory. These patterns are referred to as a walking 1 since the
 * bit pattern is shifted by 1 position for each new memory location.
 *
 * The patterns are then read back for verification, inverted and written back
 * to the memory location -- the memory locations now contain a walking 0. These
 * patterns are read back again for verification.
 *
 * \param test Current test case.
 */
void run_data_bus_test(const struct test_case *test)
{
	struct ebi_test_params *params;
	hugemem_ptr_t          base;
	hugemem_ptr_t          p;
	uint_fast8_t           i;

	params = (struct ebi_test_params *)test_get_data();
	base = params->base;

	// Write walking 1s
	for (p = base, i = 0; i < 32; i++) {
		hugemem_write32(p, 1UL << i);
		p = (hugemem_ptr_t)((uint32_t)p + sizeof(uint32_t));
	}

	// Read walking 1s, write walking 0s
	for (p = base, i = 0; i < 32; i++) {
		uint32_t expected = 1UL << i;
		uint32_t actual;

		actual = hugemem_read32(p);
		test_assert_true(test, actual == expected,
				"Read 0x%04x, expected 0x%04x (walking 1)",
				actual, expected);

		hugemem_write32(p, ~expected);
		p = (hugemem_ptr_t)((uint32_t)p + sizeof(uint32_t));
	}

	// Read walking 0s
	for (p = base, i = 0; i < 32; i++) {
		uint32_t actual;
		uint32_t expected = ~(1UL << i);

		actual = hugemem_read32(p);
		test_assert_true(test, actual == expected,
				"Read 0x%04x, expected 0x%04x (walking 0)",
				actual, expected);

		p = (hugemem_ptr_t)((uint32_t)p + sizeof(uint32_t));
	}
}

/**
 * \brief Test the address bus
 *
 * Tests the address bus for stuck and coupled lines.
 *
 * The bytes at base and all power-of-two locations are initialized with the
 * value 0x55 before any testing of the address bus is done.
 *
 * The first test of the address bus is for lines that are stuck high. This is
 * done by writing the value 0xAA to the base, then verifying that all
 * power-of-two locations still contain 0x55.
 *
 * The address bus is then checked for lines that are stuck low or coupled. The
 * base location is reinitialized with the value 0x55 before this test. The
 * value 0xAA is then temporarily written to the individual power-of-two
 * locations while the content of the others (base and power-of-two) are
 * verified to still contain 0x55.
 *
 * \param test Current test case.
 */
void run_address_bus_test(const struct test_case *test)
{
	struct ebi_test_params *params;
	hugemem_ptr_t          base;
	uint32_t               size;
	uint32_t               offset;

	params = (struct ebi_test_params *)test_get_data();
	base = params->base;
	size = params->size;

	// Initialize all power-of-two locations with 0x55
	hugemem_write8(base, 0x55);
	for (offset = 1; offset < size; offset <<= 1) {
		hugemem_ptr_t p;

		p = (hugemem_ptr_t)((uint32_t)base + offset);
		hugemem_write8(p, 0x55);
	}

	// Check for address lines stuck high
	hugemem_write8(base, 0xaa);
	for (offset = 1; offset < size; offset <<= 1) {
		hugemem_ptr_t p;
		uint8_t       actual;

		p = (hugemem_ptr_t)((uint32_t)base + offset);
		actual = hugemem_read8(p);
		test_assert_true(test, actual == 0x55,
				"Read 0x%02x @ 0x%08lx, expected 0x55 (stuck "
				"high)",
				actual, (uint32_t)p);
	}

	// Check for address lines stuck low or shorted
	hugemem_write8(base, 0x55);
	for (offset = 1; offset < size; offset <<= 1) {
		hugemem_ptr_t p;
		uint32_t      offset2;
		uint8_t       actual;

		p = (hugemem_ptr_t)((uint32_t)base + offset);
		hugemem_write8(p, 0xaa);

		actual = hugemem_read8(base);
		test_assert_true(test, actual == 0x55,
				"Read 0x%02x @ 0x%08lx after writing 0xaa @ "
				"0x%08lx, expected 0x55 (stuck low or coupled)",
				actual, (uint32_t)base, (uint32_t)p);

		for (offset2 = 1; offset2 < size; offset2 <<= 1) {
			hugemem_ptr_t q;

			if (offset2 == offset)
				continue;

			q = (hugemem_ptr_t)((uint32_t)base + offset2);
			actual = hugemem_read8(q);
			test_assert_true(test, actual == 0x55,
					"Read 0x%02x @ 0x%08lx after writing 0xaa"
					" @ 0x%08lx, expected 0x55 (stuck low or "
					"coupled)",
					actual, (uint32_t)q, (uint32_t)p);
		}

		hugemem_write8(p, 0x55);
	}
}

/**
 * \brief Test the data integrity
 *
 * Tests the integrity of the entire SDRAM by filling it a sequence of
 * incrementing 32-bit values and then verifying its content.
 * This is done in two passes, with an inversion of the content after
 * the first pass of verification.
 *
 * \note This test does not give 100% coverage for possible faults in SDRAM.
 * For exhaustive testing of memory, you may refer to literature on MARCH test
 * algorithms.
 *
 * \param test Current test case.
 */
void run_data_integrity_test(const struct test_case *test)
{
	struct ebi_test_params *params;
	hugemem_ptr_t          base;
	uint32_t               size;
	uint32_t               offset;
	uint32_t               pattern;

	params = (struct ebi_test_params *)test_get_data();
	base = params->base;
	size = params->size;

	// Fill memory with a known pattern.
	for (pattern = 1, offset = 0; offset < size; pattern++,
			offset += sizeof(uint32_t)) {
		hugemem_ptr_t p;

		p = (hugemem_ptr_t)((uint32_t)base + offset);
		hugemem_write32(p, pattern);
	}

	// Check each location and invert it for the second pass.
	for (pattern = 1, offset = 0; offset < size; pattern++,
			offset += sizeof(uint32_t)) {
		hugemem_ptr_t p;
		uint32_t      actual;
		uint32_t      expected;

		p = (hugemem_ptr_t)((uint32_t)base + offset);

		actual = hugemem_read32(p);
		test_assert_true(test, actual == pattern,
				"Read 0x%08lx @ 0x%08lx, expected 0x%08lx",
				actual, (uint32_t)p, pattern);

		expected = ~pattern;
		hugemem_write32(p, expected);
	}

	// Check each location for the inverted pattern and zero it.
	for (pattern = 1, offset = 0; offset < size; pattern++,
			offset += sizeof(uint32_t)) {
		hugemem_ptr_t p;
		uint32_t      actual;
		uint32_t      expected;

		p = (hugemem_ptr_t)((uint32_t)base + offset);

		expected = ~pattern;
		actual = hugemem_read32(p);
		test_assert_true(test, actual == expected,
				"Read 0x%08lx @ 0x%08lx, expected 0x%08lx",
				actual, (uint32_t)p, expected);
	}
}
