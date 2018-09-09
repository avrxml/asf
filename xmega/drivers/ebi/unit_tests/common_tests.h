/**
 * \file
 *
 * \brief Common test functions for External Bus Interface
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
