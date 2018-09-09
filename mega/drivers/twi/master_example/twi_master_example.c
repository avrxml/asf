/**
 * \file
 *
 * \brief TWi master example for megaRF device
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
 * <A href="http://www.microchip.com/design-centers/8-bit">Atmel AVR</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
