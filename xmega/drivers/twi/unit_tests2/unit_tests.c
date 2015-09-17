/**
 * \file
 *
 * \brief Unit tests for TWI driver in bridge mode (only available on XMEGA E)
 * Test are performed at 1MHz to check Fast Mode + mode. 
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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <asf.h>
#include <conf_test.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test for the XMEGA E TWI driver (bridge mode).
 * Bridge mode separates slave and master pads of TWIC:
 * - master is on PORTC (PC0=SDA, PC1=SCL)
 * - slave is on PORTD (PD0=SDA, PD1=SCL)
 * Remark: No TWID bloc is available on XmegaE, only TWIC slave pins.
 * The test requires the following external connection
 * on the board that runs the tests:
 * - TWIC's SDA and TWID's SDA connected together (J1.1 and J4.1 on XMEGA-E5
 * Xplained board)
 * - TWIC's SCL and TWID's SCL connected together (J1.2 and J4.2 on XMEGA-E5
 * Xplained board)
 *
 * FMPEN bit and SFMPEN bit are set and TWI frequency is 1MHz.
 * External SDA and SCL pullups are needed (3.3KOhms).
 * Internal pullups are not used because of the fixed value not well adjusted (24kOhms)
 *
 * UART is used to display results (USARTC, Rx=PC2, Tx=PC3, 38400) 
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_twim.h
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_usart_serial.h
 *
 * \section device_info Device Info
 * This example has been tested with the following setup(s):
 * XMEGA-E5 Xplained
 * \section description Description of the unit tests
 * See the documentation for the individual unit test functions
 * \ref unit_tests.c "here" for detailed descriptions of the tests.
 *
 * \section dependencies Dependencies
 * Relevant module dependencies for this application are:
 * - \ref test_suite_group
 * - \ref twi_common.h
 * - \ref twim.h
 * - \ref twis.h
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
#define TWI_MASTER           TWIC

/**
 * \internal
 * \brief TWI master module port
 */
#define TWI_MASTER_PORT      PORTC

/**
 * \internal
 * \brief TWI slave module
 */
#define TWI_SLAVE            TWIC

/**
 * \internal
 * \brief TWI data transfer rate
 */
#define TWI_SPEED            1000000

/**
 * \internal
 * \brief TWI master address
 */
#define TWI_MASTER_ADDR      0x50

/**
 * \internal
 * \brief TWI slave address
 */
#define TWI_SLAVE_ADDR       0x50

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

/** \internal
 * \brief TWI slave struct
 */
TWI_Slave_t slave;
//@}

//! \name TWI unit test helper functions
//@{

/**
 * \internal
 * \brief Pointer to process data function for TWI slave
 *
 */
static void slave_process(void)
{
	// Intentionally left empty
}

/**
 * \internal
 * \brief Interrupt routine for TWI slave
 * Calls \ref TWI_SlaveInterruptHandler
 */
ISR(TWIC_TWIS_vect) {
	TWI_SlaveInterruptHandler(&slave);
}

//@}

//! \name TWI unit test functions
//@{

/**
 * \internal
 * \brief This test sends a packet from the master, and checks
 * that the sending happens without errors.
 *
 * \param test Current test case.
 */
static void run_twi_master_send_test(
		const struct test_case *test)
{
	status_code_t master_status;
	// Package to send
	twi_package_t packet = {
		// No address or command
		.addr_length = 0,
		// issue to slave
		.chip        = TWI_SLAVE_ADDR,
		.buffer      = (void *)test_pattern,
		.length      = PATTERN_TEST_LENGTH,
		// Wait if bus is busy
		.no_wait     = false
	};

	// TWI master options
	twi_options_t m_options = {
		.speed = TWI_SPEED,
		.chip  = TWI_MASTER_ADDR,
		.speed_reg = TWI_BAUD(sysclk_get_cpu_hz(), TWI_SPEED)
	};

	irq_initialize_vectors();

	// Initialize TWI_MASTER
	sysclk_enable_peripheral_clock(&TWI_MASTER);
	twi_master_init(&TWI_MASTER, &m_options);
	twi_master_enable(&TWI_MASTER);

	// Initialize TWI_SLAVE
	sysclk_enable_peripheral_clock(&TWI_SLAVE);
	TWI_SlaveInitializeDriver(&slave, &TWI_SLAVE, *slave_process);
	TWI_SlaveInitializeModule(&slave, TWI_SLAVE_ADDR,
			TWI_SLAVE_INTLVL_MED_gc);

	cpu_irq_enable();

	// Send package to slave
	master_status = twi_master_write(&TWI_MASTER, &packet);

	test_assert_true(test, master_status == STATUS_OK,
			"Master write not ok");
}

/**
 * \internal
 * \brief This test sends a packet from the master to the slave,
 * and checks that the correct packet is received.
 *
 * \param test Current test case.
 */
static void run_twi_slave_recv_test(
		const struct test_case *test)
{
	uint8_t i = 0;

	// Package to send
	twi_package_t packet = {
		// No address or command to issue to slave
		.addr_length = 0,
		.chip        = TWI_SLAVE_ADDR,
		.buffer      = (void *)test_pattern,
		.length      = PATTERN_TEST_LENGTH,
		// Wait if bus is busy
		.no_wait      = false
	};

	// TWI master options
	twi_options_t m_options = {
		.speed = TWI_SPEED,
		.chip  = TWI_MASTER_ADDR,
		.speed_reg = TWI_BAUD(sysclk_get_cpu_hz(), TWI_SPEED)
	};

	irq_initialize_vectors();
	
	// Initialize TWI_MASTER
	sysclk_enable_peripheral_clock(&TWI_MASTER);
	twi_master_init(&TWI_MASTER, &m_options);
	twi_master_enable(&TWI_MASTER);

	// Initialize TWI_SLAVE
	for (i = 0; i < TWIS_SEND_BUFFER_SIZE; i++) {
		slave.receivedData[i] = 0;
	}
	sysclk_enable_peripheral_clock(&TWI_SLAVE);
	TWI_SlaveInitializeDriver(&slave, &TWI_SLAVE, *slave_process);
	TWI_SlaveInitializeModule(&slave, TWI_SLAVE_ADDR,
			TWI_SLAVE_INTLVL_MED_gc);

	cpu_irq_enable();

	// Send package to slave
	twi_master_write(&TWI_MASTER, &packet);

	// Wait for slave to receive packet and check that packet is correct
	do {} while (slave.result != TWIS_RESULT_OK);
	for (i = 0; i < PATTERN_TEST_LENGTH; i++) {
		test_assert_true(test, slave.receivedData[i] == test_pattern[i],
				"Wrong data[%d] received, %d != %d", i,
				slave.receivedData[i],
				test_pattern[i]);
	}
}

/**
 * \internal
 * \brief This test requests a packet to be sent from the slave,
 * and checks that the correct packet is received by the master.
 *
 * \param test Current test case.
 */
static void run_twi_master_recv_test(const struct test_case *test)
{
	uint8_t i = 0;
	uint8_t recv_pattern[TWIS_SEND_BUFFER_SIZE] = {0};

	// Package to send
	twi_package_t packet = {
		// No address or command to issue to slave
		.addr_length = 0,
		.chip        = TWI_SLAVE_ADDR,
		.buffer      = (void *)recv_pattern,
		// Wait if bus is busy
		.length      = PATTERN_TEST_LENGTH,
		.no_wait     = false
	};
	// TWI master options
	twi_options_t m_options = {
		.speed = TWI_SPEED,
		.chip  = TWI_MASTER_ADDR,
		.speed_reg = TWI_BAUD(sysclk_get_cpu_hz(), TWI_SPEED)
	};

	// Data for slave to send, same as test_pattern
	slave.sendData[0] = 0x55;
	slave.sendData[1] = 0xA5;
	slave.sendData[2] = 0x5A;
	slave.sendData[3] = 0x77;
	slave.sendData[4] = 0x99;

	irq_initialize_vectors();

	// Initialize TWI_MASTER
	sysclk_enable_peripheral_clock(&TWI_MASTER);
	twi_master_init(&TWI_MASTER, &m_options);
	twi_master_enable(&TWI_MASTER);

	// Initialize TWI_SLAVE
	for (i = 0; i < TWIS_SEND_BUFFER_SIZE; i++) {
		slave.receivedData[i] = 0;
	}
	sysclk_enable_peripheral_clock(&TWI_SLAVE);
	TWI_SlaveInitializeDriver(&slave, &TWI_SLAVE, *slave_process);
	TWI_SlaveInitializeModule(&slave, TWI_SLAVE_ADDR,
			TWI_SLAVE_INTLVL_MED_gc);

	cpu_irq_enable();

	// Send package to slave
	twi_master_read(&TWI_MASTER, &packet);

	// Wait for slave to send packet
	do {} while (slave.result != TWIS_RESULT_OK);

	for (i = 0; i < PATTERN_TEST_LENGTH; i++) {
		test_assert_true(test, recv_pattern[i] == test_pattern[i],
				"Wrong data[%d] received, %d != %d", i,
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
	DEFINE_TEST_CASE(twi_slave_recv_test, NULL,
			run_twi_slave_recv_test, NULL,
			"Receiving packet from master test");
	DEFINE_TEST_CASE(twi_master_recv_test, NULL,
			run_twi_master_recv_test, NULL,
			"Receiving packet from slave test");

	// Put test case addresses in an array
	DEFINE_TEST_ARRAY(twi_tests) = {
		&twi_master_send_test,
		&twi_slave_recv_test,
		&twi_master_recv_test,
	};

	// Define the test suite
	DEFINE_TEST_SUITE(twi_suite, twi_tests,
			"XMEGA TWI driver test suite");

	//enable bridge mode
	//master or slave have the same address
	twi_bridge_enable(&TWI_MASTER);
	
	//enable fast mode plus on master and slave
	twi_fast_mode_enable(&TWI_MASTER);
	twi_slave_fast_mode_enable(&TWI_SLAVE);
	
	// Run all tests in the suite
	test_suite_run(&twi_suite);

	while (1) {
		// Intentionally left empty.
	}
}
