/**
 * \file
 *
 * \brief Unit tests for MEGARF MAC Symbol Counter
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>
#include <conf_test.h>
#include <conf_macsc.h>
//
/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test application for the \ref macsc_group. It will
 * test overflow,compare and backoff slot counter functionality of the MACSC.
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_macsc.h
 * - \ref conf_usart_serial.h
 *
 * \section device_info Device Info
 * All AVR MEGARF devices with MACSC can be used
 *
 * \section description Description of the unit tests
 * See the documentation for the individual unit test functions
 * \ref unit_tests.c "here" for detailed descriptions of the tests.
 *
 * \section dependencies Dependencies
 * This example depends directly on the following modules:
 * - \ref test_suite_group
 * - \ref macsc_group
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.microchip.com/">Microchip</a>.\n
 * Support and FAQ: https://www.microchip.com/support/
 */

//! \name Unit test configuration
//@{
/**
 * \def CONF_TEST_USART
 * \brief USART to redirect STDIO to
 */
/**
 * \def CONF_TEST_BAUDRATE
 * \brief Baudrate of USART
 */
/**
 * \def CONF_TEST_CHARLENGTH
 * \brief Character length (bits) of USART
 */
/**
 * \def CONF_TEST_PARITY
 * \brief Parity mode of USART
 */
/**
 * \def CONF_TEST_STOPBITS
 * \brief Stop bit configuration of USART
 */
//@}

//! Variable to keep track of test results
static bool success = false;
static struct cntstatus
{
  bool cmp1:1;
  bool cmp2:1;
  bool cmp3:1;
  bool backoff:1;
}check;

/**
 * \brief Symbol Counter Compare 1 interrupt callback function
 *
 * This function is called when a compare match has occured on channel 1 of
 * symbol counter.
 */
static void unit_test_cmp1_int_cb(void)
{
	check.cmp1 = true;
}

/**
 * \brief Symbol Counter Compare 2 interrupt callback function
 *
 * This function is called when a compare match has occured on channel 2 of
 * symbol counter.
 */
static void unit_test_cmp2_int_cb(void)
{
	check.cmp2 = true;
}

/**
 * \brief Symbol Counter Compare 3 interrupt callback function
 *
 * This function is called when a compare match has occured on channel 3 of
 * symbol counter.
 */
static void unit_test_cmp3_int_cb(void)
{
	check.cmp3 = true;
}

/**
 * \brief Symbol Counter back-off slot counter interrupt callback function
 *
 * This function is called when a slot counter interrupt has occured 
 */
static void unit_test_backoff_slot_cntr_int_cb(void)
{
	check.backoff = true;
}

/**
 * \brief Run MACSC enable test
 *
 * This test verifies that no timeouts are running after initialization
 *
 * \param test Current test case.
 */
static void test_macsc_enable(const struct test_case *test)
{
	success = true;
	
        if(!(is_macsc_enable()))
		success = false;
		
	test_assert_true(test, success, "MAC symbol counter enable failed");
}

/**
 * \brief Run MACSC compare test 
 *
 * This test verifies compare channel functionality
 * for all 3 cmp channels of the macsc and if cmp match
 * occurs after set compare interval
 * \param test Current test case.
 */
static void test_macsc_cmp(const struct test_case *test)
{
	success = true;
	
	macsc_set_cmp1_int_cb(unit_test_cmp1_int_cb);
	macsc_set_cmp2_int_cb(unit_test_cmp2_int_cb);
	macsc_set_cmp3_int_cb(unit_test_cmp3_int_cb);

	/*
	 * Configure MACSC to generate compare interrupts from channels 1,2,3
	 * Set compare mode to absolute
	 */
	macsc_enable_cmp_int(MACSC_CC1);
	macsc_enable_cmp_int(MACSC_CC2);
	macsc_enable_cmp_int(MACSC_CC3);
	
	macsc_use_cmp(COMPARE_MODE, (MACSC_READ32(SCCNT) + CONFIG_MACSC_TIMEOUT_TICK_HZ) / 4,
			MACSC_CC1);
	macsc_use_cmp(COMPARE_MODE, (MACSC_READ32(SCCNT) + CONFIG_MACSC_TIMEOUT_TICK_HZ / 2),
			MACSC_CC2);
	macsc_use_cmp(COMPARE_MODE, (MACSC_READ32(SCCNT) + CONFIG_MACSC_TIMEOUT_TICK_HZ), MACSC_CC3);

	delay_ms(1500);
        
        if((check.cmp1 == true)&(check.cmp2 == true) & (check.cmp3 == true))
          success = true;
        else 
          success = false;
    
	test_assert_true(test, success,"MACSC compare channels test failed");
}

/**
 * \brief Backoff slot counter test
 *
 * This test starts the backoff slot counter and verifies that the timeout
 * occurs as expected 10 times.
 *
 * \param test Current test case.
 */
static void test_macsc_backoff(const struct test_case *test)
{
    success = true;
	
	macsc_set_backoff_slot_cntr_int_cb(unit_test_backoff_slot_cntr_int_cb);

	macsc_backoff_slot_cnt_enable();
	
	if(!(is_macsc_backoff_enable()))
		success = false;
	
	delay_us(400);
        
        if(!check.backoff)
          success = false;
        
        macsc_backoff_slot_cnt_disable();
    
	test_assert_true(test, success,"MACSC backoff slot counter test failed");
}

/**
 * \brief Run MACSC unit tests
 */
int main (void)
{
	const usart_serial_options_t usart_serial_options =
	{
		.baudrate     = CONF_TEST_BAUDRATE,
		.charlength   = CONF_TEST_CHARLENGTH,
		.paritytype   = CONF_TEST_PARITY,
		.stopbits     = CONF_TEST_STOPBITS,
	};

	board_init();
	sysclk_init();
	ioport_init();

	sysclk_enable_peripheral_clock(&TRX_CTRL_0);
	
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);
	
	cpu_irq_enable();
	
	macsc_enable();
	macsc_write_clock_source(SYS_CLK_SRC);
	
	// Define all the test cases
	
	DEFINE_TEST_CASE(macsc_enable_test, NULL, test_macsc_enable, NULL,
			"Testing MACSC enable");
			
	DEFINE_TEST_CASE(macsc_cmp_test, NULL, test_macsc_cmp, NULL,
			"Testing MACSC compare channels");


	DEFINE_TEST_CASE(macsc_backoff_test, NULL, test_macsc_backoff, NULL,
			"Testing MACSC backoff slot counter");

	// Put test case addresses in an array
	DEFINE_TEST_ARRAY(macsc_tests) = {
		&macsc_enable_test,
		&macsc_cmp_test,
		&macsc_backoff_test,
	};

	// Define the test suite
	DEFINE_TEST_SUITE(macsc_suite, macsc_tests,
			"MEGARF MAC Symbol Counter test suite");

	// Run all tests in the test suite
	test_suite_run(&macsc_suite);

	while (1) {
		// Loop for infinity
	}
}
