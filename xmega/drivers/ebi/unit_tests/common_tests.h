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
#ifndef COMMON_TESTS_H
#define COMMON_TESTS_H

#include <hugemem.h>
#include <stdint.h>
#include <unit_test/suite.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup ebi_group
 * \weakgroup ebi_tests_group Common EBI test functions
 *
 * This module contains common functions for testing of \ref ebi_group and the
 * setup of external RAM devices. The functions tests for:
 * - data bus faults
 * - address bus faults
 * - data integrity issues
 *
 * The test functions are meant for use with \ref test_suite_group and require
 * that the test data has been set to an initialized \ref ebi_test_params
 * struct.
 *
 * @{
 */

//! EBI test parameters
struct ebi_test_params {
	hugemem_ptr_t base; //<! Base address of the external RAM
	uint32_t size; //<! Size of the external RAM
};

void run_data_bus_test(const struct test_case *test);
void run_address_bus_test(const struct test_case *test);
void run_data_integrity_test(const struct test_case *test);

#ifdef __cplusplus
}
#endif

#endif
