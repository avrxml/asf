/**
 * \file
 *
 * \brief TWi master example for megaRF device
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

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the TWI master mode example.
 *
 * \section files Main Files
 * - twi_master_example.c: example application.
 * - conf_board.h: board configuration
 * - conf_twi.h: TWI master configuration used in this example
 *
 * \section driverinfo twi master Driver
 * The twi driver can be found \ref group_megarf_drivers_twi "here".
 *
 * \section deviceinfo Device Info
 * AVR MEGARF device can be used.
 *
 * \section exampledescription Description of the example
 * The given example uses one kit as a TWI master:
 *    - the TWI master performs a write  to the TWI slave,
 *    - the TWI master performs a read  of what it just wrote to the TWI
 *    - the read data are compared with the originally written data.
 *
 * LED gives the result of the test:
 * If PASS: LED is on at the end of the test.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/avr">Atmel AVR</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "asf.h"
#include <conf_twi.h>
#include <conf_board.h>

/**  \name Slave EEPROM memory Test Pattern Constants */
/**@{ */

#define PATTERN_TEST_LENGTH     sizeof(test_pattern)
const uint8_t test_pattern[] = {
	0x05,
	0x15,
	0x25,
	0x35,
	0x45,
	0x55,
	0x65,
	0x75,
};

const uint8_t slave_mem_addr[SLAVE_MEM_ADDR_LENGTH] = {
	0x00,
};
/** @}*/

/* ! \name TWI Base Address */
/**@{ */
#define TWI_EXAMPLE  &TWBR
/** @}*/



/** \brief TWI Master Example Main
 *
 * The master example begins by initializing required board resources.  The
 * system clock, basic GPIO pin mapping, and interrupt vectors are established.
 *
 * A memory location on a TWI slave is written with a fixed test pattern which
 * is then read back into a separate buffer.  As a basic sanity check, the
 * original write-buffer values are compared with values read from the slave to
 * a separate buffer.  An LED on the development board is illuminated when there
 * is a match between the written and read data.
 *
 * \return Nothing.
 */
int main(void)
{
	/* Initialize the common clock service, board-specific initialization,
	 * and
	 * interrupt vector support prior to using the TWI master interfaces.
	 */
	sysclk_init();

	board_init();

	/* TWI master initialization options. */
	twi_master_options_t opt = {
		.speed      = TWI_SPEED_HZ,
		.chip  = SLAVE_BUS_ADDR,
		
	};
	opt .baud_reg = TWI_CLOCK_RATE(sysclk_get_cpu_hz(), opt.speed);

	/* Enable the peripheral clock for TWI module */
	sysclk_enable_peripheral_clock(TWI_EXAMPLE);
	
	/* Initialize the TWI master driver. */
	twi_master_init(TWI_EXAMPLE,&opt);

	twi_package_t packet = {
		.addr[0]         =  slave_mem_addr[0], /* TWI slave memory
		                                         * address data */
		.addr_length  = (uint8_t)SLAVE_MEM_ADDR_LENGTH, /* TWI slave
		                                                 * memory
		                                                 * address data
		                                                 * size */
		.chip         = SLAVE_BUS_ADDR, /* TWI slave bus address */
		.buffer       = (void *)test_pattern, /* transfer data source
		                                       * buffer */
		.length       = PATTERN_TEST_LENGTH /* transfer data size
		                                     * (bytes) */
	};

	/* Perform a multi-byte write access then check the result. */

	while (twi_master_write(TWI_EXAMPLE,&packet) != TWI_SUCCESS) {
	}
	
	/* Write completion time for EEPROM */
	delay_ms(5);

	uint8_t data_received[PATTERN_TEST_LENGTH] = {0};

	twi_package_t packet_received = {
		.addr[0]         = slave_mem_addr[0], /* TWI slave memory
		                                         * address data */
		.addr_length  = (uint8_t)SLAVE_MEM_ADDR_LENGTH, /* TWI slave
		                                                 * memory
		                                                 * address data
		                                                 * size */
		.chip         = SLAVE_BUS_ADDR, /* TWI slave bus address */
		.buffer       = data_received, /* transfer data destination
		                                * buffer */
		.length       = PATTERN_TEST_LENGTH /* transfer data size
		                                     * (bytes) */
	};

	/* Perform a multi-byte read access then check the result. */
	while (twi_master_read(TWI_EXAMPLE,&packet_received) != TWI_SUCCESS) {
	}

	/* Verify that the received data matches the sent data. */
	for (int i = 0; i < PATTERN_TEST_LENGTH; i++) {
		if (data_received[i] != test_pattern[i]) {
			/* Error */

			while (1) {
			}
		}
	}

	/* test PASS */

	/* Green LED ON */
	LED_On(LED_GREEN_GPIO);

	while (1) {
	}
}
