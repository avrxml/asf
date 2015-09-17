/**
 * \file
 *
 * \brief Unit tests for USART driver
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

#include <asf.h>
#include <conf_test.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test for the MEGARF USART driver. It communicates with
 * itself by looping back using a jumper on the external USART RX/TX pins.
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_usart_serial.h
 *
 * \section device_info Device Info
 * This example has been tested with the following setup(s):
 * - atmega128rfa1_stk600 and atmega256rfr2_xplained_pro
 *
 * \section description Description of the unit tests
 * See the documentation for the individual unit test functions
 * \ref unit_tests.c "here" for detailed descriptions of the tests.
 *
 * \section dependencies Dependencies
 * This example depends directly on the following modules:
 * - \ref test_suite_group
 * - \ref megarf_usart_group
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 */

/**
 * \brief Test physical loop-back with some characters.
 *
 * This function sends a character over USART on loop back to verify that init
 * and sending/receiving works. A jumper is connected on the USART.
 *
 * \param test Current test case.
 */
static void run_loopback_test(const struct test_case *test)
{
	uint8_t out_c = 'a';
	uint8_t in_c  = 0;
	
	const usart_rs232_options_t options = {
		.baudrate   = CONF_UNIT_BAUDRATE,
		.charlength = CONF_UNIT_CHARLENGTH,
		.paritytype = CONF_UNIT_PARITY,
		.stopbits   = CONF_UNIT_STOPBITS
	};

	usart_init_rs232(&CONF_UNIT_USART, &options);

	usart_putchar(&CONF_UNIT_USART, out_c);
	in_c = usart_getchar(&CONF_UNIT_USART);

	test_assert_true(test, in_c == out_c,
			"Read character is not correct: %d != %d", in_c, out_c);			
}

/**
 * \brief Test physical loop-back with some characters in sunc mode.
 *
 * This function sends a character over USART on loop back to verify that init
 * and sending/receiving works. A jumper is connected on the USART.
 *
 * \param test Current test case.
 */
static void run_loopback_syncmode_test(const struct test_case *test)
{
	uint8_t out_c = 'c';
	uint8_t in_c  = 0;
        port_pin_t sck_pin;
        
        sysclk_enable_module(POWER_RED_REG0, PRUSART0_bm);
        
        usart_set_mode(&CONF_UNIT_USART, USART_CMODE_SYNCHRONOUS_gc);

	sck_pin = IOPORT_CREATE_PIN(PORTE, 2);
	ioport_configure_port_pin(ioport_pin_to_port(sck_pin),
				ioport_pin_to_mask(sck_pin),
				IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH );
        
        usart_spi_set_baudrate(&CONF_UNIT_USART, CONF_UNIT_BAUDRATE,
			sysclk_get_source_clock_hz());
	usart_tx_enable(&CONF_UNIT_USART);
	usart_rx_enable(&CONF_UNIT_USART);

	usart_putchar(&CONF_UNIT_USART, out_c);
	in_c = usart_getchar(&CONF_UNIT_USART);

	test_assert_true(test, in_c == out_c,
	   "Read character through sync mode is not correct: %d != %d", in_c, out_c);			
}

/**
 * \brief Test setting different parameters of the USART module
 *
 * This function calls the different set functions, and verifies that the
 * correct values are being set.
 *
 * \param test Current test case.
 */
static void run_set_functions_test(const struct test_case *test)
{
	bool success;

	/* Set USART mode and verify that it has been correctly set. */
	usart_set_mode(&CONF_UNIT_USART, USART_CMODE_MSPI_gc);
	success = (CONF_UNIT_USART.UCSRnC & USART_UMSEL01_gm) ==
			USART_CMODE_MSPI_gc;
	test_assert_true(test, success,
			"Trying to set USART mode to master SPI failed.");
        
        /* Set USART sync mode and verify that it has been correctly set. */
	usart_set_mode(&CONF_UNIT_USART, USART_CMODE_SYNCHRONOUS_gc);
	success = (CONF_UNIT_USART.UCSRnC & USART_UMSEL01_gm) ==
			USART_CMODE_SYNCHRONOUS_gc;
	test_assert_true(test, success,
			"Trying to set USART mode to sync mode failed.");
			
	/* Test enabling and disabling USART double baud*/
	usart_double_baud_enable(&CONF_UNIT_USART);
	success = (CONF_UNIT_USART.UCSRnA & USART_U2X_bm);
	test_assert_true(test, success, "Trying to enable USART double baud failed.");
	
	usart_double_baud_disable(&CONF_UNIT_USART);
	success = !(CONF_UNIT_USART.UCSRnA & USART_U2X_bm);
	test_assert_true(test, success, "Trying to disable USART double baud failed.");        

	/* Test enabling and disabling USART RX */
	usart_rx_enable(&CONF_UNIT_USART);
	success = (CONF_UNIT_USART.UCSRnB & USART_RXEN_bm);
	test_assert_true(test, success, "Trying to enable USART RX failed.");

	usart_rx_disable(&CONF_UNIT_USART);
	success = !(CONF_UNIT_USART.UCSRnB & USART_RXEN_bm);
	test_assert_true(test, success, "Trying to disable USART RX failed.");

	/* Test enabling and disabling USART TX */
	usart_tx_enable(&CONF_UNIT_USART);
	success = (CONF_UNIT_USART.UCSRnB & USART_TXEN_bm);
	test_assert_true(test, success, "Trying to enable USART TX failed.");

	usart_tx_disable(&CONF_UNIT_USART);
	success = !(CONF_UNIT_USART.UCSRnB & USART_TXEN_bm);
	test_assert_true(test, success, "Trying to disable USART TX failed.");
	
	/* Test enabling and disabling USART TX complete interrupt*/
	usart_tx_complete_interrupt_enable(&CONF_UNIT_USART);
	success = (CONF_UNIT_USART.UCSRnB & USART_TXC_bm);
	test_assert_true(test, success, "Trying to enable USART TX Complete interrupt failed.");

	usart_tx_complete_interrupt_disable(&CONF_UNIT_USART);
	success = !(CONF_UNIT_USART.UCSRnB & USART_TXC_bm);
	test_assert_true(test, success, "Trying to disable USART TX Complete interrupt failed.");
	
	/* Test enabling and disabling USART RX complete interrupt*/
	usart_rx_complete_interrupt_enable(&CONF_UNIT_USART);
	success = (CONF_UNIT_USART.UCSRnB & USART_RXC_bm);
	test_assert_true(test, success, "Trying to enable USART RX Complete interrupt failed.");

	usart_rx_complete_interrupt_disable(&CONF_UNIT_USART);
	success = !(CONF_UNIT_USART.UCSRnB & USART_RXC_bm);
	test_assert_true(test, success, "Trying to disable USART RX Complete interrupt failed.");
	
	/* Test enabling and disabling USART data register empty interrupt*/
	usart_data_empty_interrupt_enable(&CONF_UNIT_USART);
	success = (CONF_UNIT_USART.UCSRnB & USART_DRIE_bm);
	test_assert_true(test, success, "Trying to enable USART data register empty interrupt failed.");

	usart_data_empty_interrupt_disable(&CONF_UNIT_USART);
	success = !(CONF_UNIT_USART.UCSRnB & USART_DRIE_bm);
	test_assert_true(test, success, "Trying to disable USART data register empty interrupt failed.");	

	/* Try to set format. */
	usart_format_set(&CONF_UNIT_USART, USART_CHSIZE_8BIT_gc,
			USART_PMODE_EVEN_gc, true);
	success = !(CONF_UNIT_USART.UCSRnA & USART_FE_bm);
	test_assert_true(test, success,
			"Trying to set the Frame Format failed.");
}

/**
 * \brief Test checking different registers of the USART module
 *
 * This function calls the different check functions and make sure they set the
 * correct values.
 *
 * \param test Current test case.
 */
static void run_check_registers_test(const struct test_case *test)
{
	bool success;
	uint8_t data = 'b';

	
	const usart_rs232_options_t options = {
		.baudrate   = CONF_UNIT_BAUDRATE,
		.charlength = CONF_UNIT_CHARLENGTH,
		.paritytype = CONF_UNIT_PARITY,
		.stopbits   = CONF_UNIT_STOPBITS
	};

	usart_init_rs232(&CONF_UNIT_USART, &options);
      
	/* Test empty data register */
	success = usart_data_register_is_empty(&CONF_UNIT_USART);
	test_assert_true(test, success,
			"Checking if the data register is empty failed");
	
	/* Test finished data transfers */
	usart_put(&CONF_UNIT_USART, data);
	for(volatile uint16_t delay=0;delay<20000;delay++);
    
	success = usart_rx_is_complete(&CONF_UNIT_USART);
	usart_get(&CONF_UNIT_USART);
	test_assert_true(test, success,
   	                "Checking if the receive is finished failed");		       
					
	success = usart_tx_is_complete(&CONF_UNIT_USART);
	test_assert_true(test, success,
	                "Checking if the transmit is finished failed");

}

/* Calculate the baud rate */
static uint32_t calculate_baudrate(uint32_t baud, uint32_t cpu_hz)
{
	uint32_t min_rate;
	uint32_t max_rate;
	uint32_t ubrr;

	/*
	 * Check if the hardware supports the given baud rate
	 */
	/* 8 =  8 * (0 + 1) = (UBRRn_min + 1) */
	max_rate = cpu_hz / 8;
	/* 524288 =  8 * (65535 + 1)  =  8 *(UBRRn_max+1) */
	min_rate = cpu_hz / 524288;

	if (CONF_UNIT_USART.UCSRnA & USART_U2X_bm) {
		max_rate /= 2;
		min_rate /= 2;
	}

	if ((baud > max_rate) || (baud < min_rate)) {
		return false;
	}

	/* Check if double speed is enabled. */
	if (CONF_UNIT_USART.UCSRnA & USART_U2X_bm) {
		baud *= 2;
		ubrr = (uint32_t)(cpu_hz / 8 / baud) - 1;
	} else {
		ubrr = (uint32_t)(cpu_hz / 16 / baud) - 1;
	}

	return ubrr;
}


/**
 * \brief Test setting different baud rates of the USART module
 *
 * This function calls the set function and check that the correct baud-rate has
 * been set in the register.
 *
 * \param test Current test case.
 */
static void run_baudrate_test(const struct test_case *test)
{
	bool success;

	uint32_t baud;
	uint32_t cpu_hz;
	uint32_t ubrr = 0;
	
	/* Get the system cpu frequency */
	cpu_hz = sysclk_get_cpu_hz();
        
        /* Test for baud rate equal to 2400 */
	baud = 2400;
	usart_set_baudrate(&CONF_UNIT_USART, baud, cpu_hz);
	ubrr = calculate_baudrate(baud, cpu_hz);
	success = CONF_UNIT_USART.UBRR == ubrr;
	test_assert_true(test, success, "Setting baud rate to 2400 failed");
	
	/* Test for baud rate equal to 9600 */
	baud = 9600;
	usart_set_baudrate(&CONF_UNIT_USART, baud, cpu_hz);
	ubrr = calculate_baudrate(baud, cpu_hz);
	success = CONF_UNIT_USART.UBRR == ubrr;
	test_assert_true(test, success, "Setting baud rate to 9600 failed");

	/* Test for baud rate equal to 19200 */
	baud = 19200;
	usart_set_baudrate(&CONF_UNIT_USART, baud, cpu_hz);
	ubrr = calculate_baudrate(baud, cpu_hz);
	success = CONF_UNIT_USART.UBRR == ubrr;
	test_assert_true(test, success, "Setting baud rate to 19200 failed");

	/* Test for baud rate equal to 38400 */
	baud = 38400;
	usart_set_baudrate(&CONF_UNIT_USART, baud, cpu_hz);
	ubrr = calculate_baudrate(baud, cpu_hz);
	success = CONF_UNIT_USART.UBRR == ubrr;
	test_assert_true(test, success, "Setting baud rate to 38400 failed");
}

/**
 * \brief Run USART unit tests
 *
 * Initializes the clock system, board and serial output, then sets up the
 * USART unit test suite and runs it.
 */
int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate   = CONF_TEST_BAUDRATE,
		.charlength = CONF_TEST_CHARLENGTH,
		.paritytype = CONF_TEST_PARITY,
		.stopbits   = CONF_TEST_STOPBITS,
	};

	board_init();
	sysclk_init();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	DEFINE_TEST_CASE(loopback_test, NULL, run_loopback_test, NULL,
			"Test looping back characters through USART");
	DEFINE_TEST_CASE(sync_loopback_test, NULL, run_loopback_syncmode_test, NULL,
			"Test looping back characters through sync mode of USART");        
	DEFINE_TEST_CASE(set_functions_test, NULL, run_set_functions_test, NULL,
			"Test setting of various properties");
	DEFINE_TEST_CASE(check_registers_test, NULL, run_check_registers_test,
			NULL, "Test checking registers and transfers");
	DEFINE_TEST_CASE(baudrate_test, NULL, run_baudrate_test, NULL,
			"Test setting standard baudrates");

	/* Put test case addresses in an array */
	DEFINE_TEST_ARRAY(usart_tests) = {
		&loopback_test,
		&sync_loopback_test,                
		&set_functions_test,
		&check_registers_test,
		&baudrate_test,
	};

	/* Define the test suite */
	DEFINE_TEST_SUITE(usart_suite, usart_tests,
			"MEGARF USART driver test suite");

	/* Run all tests in the suite */
	test_suite_run(&usart_suite);

	while (1) {
		/* Intentionally left empty. */
	}
}
