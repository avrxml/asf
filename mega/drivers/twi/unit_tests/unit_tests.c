/**
 * \file
 *
 * \brief Unit tests for TWI driver
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
#include <asf.h>
#include <conf_test.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test for the MEGARF TWI driver.
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_twi.h
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_usart_serial.h
 *
 * \section device_info Device Info
 * This example has been tested with the following setup(s):
 * - atmega128rfa1_stk600 and atmega256rfr2_atmega256rfr2_xplained_pro
 *
 * \section description Description of the unit tests
 * See the documentation for the individual unit test functions
 * \ref unit_tests.c "here" for detailed descriptions of the tests.
 *
 * \section dependencies Dependencies
 * Relevant module dependencies for this application are:
 * - \ref test_suite_group
 * - \ref twi_megarf.h
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
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
 * \brief Stopbit configuration of USART
 */
//@}

//! \name TWI unit test definitions
//@{

/**
 * \internal
 * \brief TWI master module
 */
#define TWI_MASTER           &TWBR

/**
 * \internal
 * \brief TWI data transfer rate
 */
#define TWI_SPEED            125000

/**
 * \internal
 * \brief TWI slave address
 */
#define TWI_SLAVE_ADDR       0xA0

/**
 * \internal
 * \brief TWI slave memeroy address
 */
 #define SLAVE_MEM_ADDR      0x10
 
 /**
 * \internal
 * \brief TWI slave memeory address length
 */
 #define SLAVE_MEM_ADDR_LENGTH   TWI_SLAVE_ONE_BYTE_SIZE


/** \internal
 * \brief Test Pattern size
 */
#define PATTERN_TEST_LENGTH  sizeof(test_pattern)

/** \internal
 * \brief Test Pattern
 */
const uint8_t test_pattern[] = {
	0x55,
	0xA5,
	0x5A,
	0x77,
	0x99
};


//@}

//! \name TWI unit test functions
//@{

/**
 * \internal
 * \brief This test sends a packet from the master, and checks
 * that the sending happens without errors. Master is megaRF device and the slave 
 * is on board EEPROM.Configuring EEPROM as slave is not required.
 * \param test Current test case.
 */
static void run_twi_master_send_test(const struct test_case *test)
{
	status_code_t master_status;
	volatile uint64_t delay=0;
	
	// Package to send
	twi_package_t packet = {
		.addr[0]         = (uint8_t) SLAVE_MEM_ADDR, /* TWI slave memory
		                                         * address data */
		.addr_length  = (uint8_t)SLAVE_MEM_ADDR_LENGTH, /* TWI slave
		                                                 * memory
		                                                 * address data
		                                                 * size */
		.chip         = TWI_SLAVE_ADDR, /* TWI slave bus address */
		.buffer       = (void *)test_pattern, /* transfer data source
		                                       * buffer */
		.length       = PATTERN_TEST_LENGTH /* transfer data size
				                            * (bytes) */
    };

	/* TWI master initialization options. */
	twi_master_options_t m_options = {
		.speed      = TWI_SPEED,
		.chip  = TWI_SLAVE_ADDR,
		
	};
	m_options.baud_reg = TWI_CLOCK_RATE(sysclk_get_cpu_hz(), m_options.speed);

	// Initialize TWI_MASTER
	sysclk_enable_peripheral_clock(TWI_MASTER);
	twi_master_init(TWI_MASTER, &m_options);

	// Send package to slave
	master_status = twi_master_write(TWI_MASTER, &packet);
        
    /* Write completion time for EEPROM */
	for(delay=0;delay<10000;delay++);

	test_assert_true(test, master_status == STATUS_OK,
			"Master write not ok");
}

/**
 * \internal
 * \brief This test requests previously sent packet to be sent from the slave,
 * and checks that the correct packet is received by the master. Master is 
 * megaRF device and the slave is on board EEPROM.Configuring EEPROM as slave 
 * is not required.
 * \param test Current test case.
 */
static void run_twi_master_recv_test(const struct test_case *test)
{
	uint8_t i = 0;
	uint8_t recv_pattern[PATTERN_TEST_LENGTH] = {0};

	// Package to send
	twi_package_t packet = {
		.addr[0]         = (uint8_t) SLAVE_MEM_ADDR, /* TWI slave memory
		                                         * address data */
		.addr_length  = (uint8_t)SLAVE_MEM_ADDR_LENGTH, /* TWI slave
		                                                 * memory
		                                                 * address data
		                                                 * size */
		.chip        = TWI_SLAVE_ADDR,
		.buffer      = (void *)recv_pattern,
		.length      = PATTERN_TEST_LENGTH,
	};
	
	/* TWI master initialization options. */
	twi_master_options_t m_options = {
		.speed      = TWI_SPEED,
		.chip  = TWI_SLAVE_ADDR,
		
	};
	m_options.baud_reg = TWI_CLOCK_RATE(sysclk_get_cpu_hz(), m_options.speed);

	// Initialize TWI_MASTER
	sysclk_enable_peripheral_clock(TWI_MASTER);
	twi_master_init(TWI_MASTER, &m_options);

	// Send package to slave
	twi_master_read(TWI_MASTER, &packet);

	for (i = 0; i < PATTERN_TEST_LENGTH; i++) {
		test_assert_true(test, recv_pattern[i] == test_pattern[i],
				"Wrong twi data[%d] received, %d != %d", i,
				recv_pattern[i],
				test_pattern[i]);
	}
}

//@}

/**
 * \brief Run TWI unit tests
 *
 * Initializes the clock system, board and serial output, then sets up the
 * TWI unit test suite and runs it.
 */
int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate   = CONF_TEST_BAUDRATE,
		.charlength = CONF_TEST_CHARLENGTH,
		.paritytype = CONF_TEST_PARITY,
		.stopbits   = CONF_TEST_STOPBITS,
	};

	sysclk_init();
	board_init();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	// Define single ended conversion test cases
	DEFINE_TEST_CASE(twi_master_send_test, NULL,
			run_twi_master_send_test, NULL,
			"Sending packet from master test");
	DEFINE_TEST_CASE(twi_master_recv_test, NULL,
			run_twi_master_recv_test, NULL,
			"Receiving packet from slave test");

	// Put test case addresses in an array
	DEFINE_TEST_ARRAY(twi_tests) = {
		&twi_master_send_test,
		&twi_master_recv_test,
	};

	// Define the test suite
	DEFINE_TEST_SUITE(twi_suite, twi_tests,
			"MEGARF TWI driver test suite");

	// Run all tests in the suite
	test_suite_run(&twi_suite);

	while (1) {
		// Intentionally left empty.
	}
}

