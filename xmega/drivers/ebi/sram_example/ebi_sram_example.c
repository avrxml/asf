/**
 * \file
 *
 * \brief AVR XMEGA External Bus Interface (EBI) example
 *
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
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
 * \section intro Introduction
 * This example demonstrates how to use the external bus interface driver.
 *
 * \section files Main Files
 *  - ebi_sram_example.c example application
 *  - conf_board.h board configuration
 *
 * \section ebi_driver_section drivers/ebi/ebi.h driver
 * The external bus interface driver is described in the \ref ebi_group
 * section.
 *
 * \section device_info Device Info
 * All AVR XMEGA devices with an external bus interface (EBI) and a SRAM wired
 * to the EBI can be used. This example has been tested with the following
 * setup:
 *  - STK600 top board with ATxmega128A1 and a 1 Mbit SRAM (10 ns) device
 *
 * \section example_configuration Example Configuration
 * The example code is configurable by defining the following symbols in the
 * conf_board.h header file.
 *  - \ref CONFIG_HAVE_HUGEMEM must be defined to use the 8-bit AVR huge memory
 *  interface. See \ref hugemem_group for more information.
 *  - \ref BOARD_EBI_SRAM_BASE must be defined and given the value of the base
 *  address of the external SRAM device.
 *  - \ref BOARD_EBI_SRAM_SIZE must be defined and given the size of the
 *  external SRAM device.
 *
 * \section example_description Description of the example
 * The example will configure the external bus interface to address the SRAM
 * device at a board configurable address. The SRAM will addressable through
 * \ref hugemem_group.
 *
 * After configuring the external bus interface the example code will run
 * through three test applications which can detect faults to address lines,
 * data lines and finally a data integrity test which will write both 0s and 1s
 * to the entire SRAM device.
 *
 * This example uses the on-board LEDs to provide user feedback, the output
 * from the LEDs are as follows:
 *  - LED0: external bus interface configured
 *  - LED1: data bus test completed successfully
 *  - LED2: address bus test completed successfully
 *  - LED3: data integrity test completed successfully
 *  - LED7: example application has completed
 *
 * If LED7 lights up before LED0 to LED4 has been lit, then the first of the
 * missing lit LEDs indicates that the corresponding test has failed.
 *
 * The test requires about 1 second to complete on an 1 Mbit SRAM device with a
 * 64 MHz EBI configuration.
 *
 * \section dependencies Dependencies
 * This example depends on the following modules:
 *  - \ref ebi_group
 *  - \ref gpio_group for LED control
 *  - \ref sleepmgr_group for being able to go to sleep
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <stdint.h>
#include <conf_board.h>
#include <asf.h>

/**
 * \brief EBI chip select configuration
 *
 * This struct holds the configuration for the chip select used to set up the
 * SRAM. The example code will use the EBI helper function to setup the
 * contents before writing the configuration using ebi_cs_write_config().
 */
static struct ebi_cs_config     cs_config;

/**
 * \brief Test the EBI data bus wired to the SRAM
 *
 * This function will perform a walking 1s to locate any shorts or open leads
 * to the SRAM device.
 *
 * \param base Base address of the external memory device
 *
 * \retval STATUS_OK on success, and \ref status_code_t error code on failure
 */
static status_code_t ebi_test_data_bus(hugemem_ptr_t base)
{
	hugemem_ptr_t   p;
	uint_fast8_t    i;

	/* Write walking 1s */
	for (p = base, i = 0; i < 32; i++) {
		hugemem_write32(p, 1UL << i);
		p = (hugemem_ptr_t)((uint32_t)p + sizeof(uint32_t));
	}

	/* Read walking 1s, write walking 0s */
	for (p = base, i = 0; i < 32; i++) {
		uint32_t        expected = 1UL << i;
		uint32_t        actual;

		actual = hugemem_read32(p);
		if (actual != expected) {
			return ERR_IO_ERROR;
		}

		hugemem_write32(p, ~expected);
		p = (hugemem_ptr_t)((uint32_t)p + sizeof(uint32_t));
	}

	/* Read walking 0s */
	for (p = base, i = 0; i < 32; i++) {
		uint32_t        actual;
		uint32_t        expected = ~(1UL << i);

		actual = hugemem_read32(p);
		if (actual != expected) {
			return ERR_IO_ERROR;
		}

		p = (hugemem_ptr_t)((uint32_t)p + sizeof(uint32_t));
	}

	return STATUS_OK;
}

/**
 * \brief Test the EBI address bus wired to the SRAM
 *
 * This function will perform an address bus test to locate any shorts or open
 * leads to the SRAM device.
 *
 * \param base Base address of the external memory device
 * \param size Size of the external memory device
 *
 * \retval STATUS_OK on success, and \ref status_code_t error code on failure
 */
static status_code_t ebi_test_addr_bus(hugemem_ptr_t base, uint32_t size)
{
	uint32_t        offset;
	uint_fast8_t    i;

	/* Initialize all power-of-two locations with 0x55 */
	hugemem_write8(base, 0x55);
	for (offset = 1; offset < size; offset <<= 1) {
		hugemem_ptr_t   p;

		p = (hugemem_ptr_t)((uint32_t)base + offset);
		hugemem_write8(p, 0x55);
	}

	/* Check for address lines stuck high */
	hugemem_write8(base, 0xaa);
	for (i = 0, offset = 1; offset < size; offset <<= 1, i++) {
		hugemem_ptr_t   p;
		uint8_t         actual;

		p = (hugemem_ptr_t)((uint32_t)base + offset);
		actual = hugemem_read8(p);
		if (actual != 0x55) {
			return ERR_IO_ERROR;
		}
	}

	/* Check for address lines stuck low or shorted */
	hugemem_write8(base, 0x55);
	for (i = 0, offset = 1; offset < size; offset <<= 1, i++) {
		hugemem_ptr_t   p;
		uint32_t        offset2;
		uint_fast8_t    j;
		uint8_t         actual;

		p = (hugemem_ptr_t)((uint32_t)base + offset);
		hugemem_write8(p, 0xaa);

		actual = hugemem_read8(base);
		if (actual != 0x55) {
			return ERR_IO_ERROR;
		}

		for (j = 0, offset2 = 1; offset2 < size; offset2 <<= 1, j++) {
			hugemem_ptr_t   q;

			if (offset2 == offset)
				continue;

			q = (hugemem_ptr_t)((uint32_t)base + offset2);
			actual = hugemem_read8(q);
			if (actual != 0x55) {
				return ERR_IO_ERROR;
			}
		}

		hugemem_write8(p, 0x55);
	}

	return STATUS_OK;
}

/**
 * \brief Perform a SRAM data integrity test
 *
 * This function will perform a SRAM data integrity test by writing 0s and 1s
 * to the entire external device.
 *
 * \param base Base address of the external memory device
 * \param size Size of the external memory device
 *
 * \retval STATUS_OK on success, and \ref status_code_t error code on failure
 */
static status_code_t ebi_test_data_integrity(hugemem_ptr_t base, uint32_t size)
{
	uint32_t        offset;
	uint32_t        pattern;

	/* Fill memory with a known pattern. */
	for (pattern = 1, offset = 0; offset < size; pattern++,
			offset += sizeof(uint32_t)) {
		hugemem_ptr_t   p;

		p = (hugemem_ptr_t)((uint32_t)base + offset);
		hugemem_write32(p, pattern);
	}

	/* Check each location and invert it for the second pass. */
	for (pattern = 1, offset = 0; offset < size; pattern++,
			offset += sizeof(uint32_t)) {
		hugemem_ptr_t   p;
		uint32_t        actual;
		uint32_t        expected;

		p = (hugemem_ptr_t)((uint32_t)base + offset);

		actual = hugemem_read32(p);
		if (actual != pattern) {
			return ERR_IO_ERROR;
		}

		expected = ~pattern;
		hugemem_write32(p, expected);
	}

	/* Check each location for the inverted pattern and zero it. */
	for (pattern = 1, offset = 0; offset < size; pattern++,
			offset += sizeof(uint32_t)) {
		hugemem_ptr_t   p;
		uint32_t        actual;
		uint32_t        expected;

		p = (hugemem_ptr_t)((uint32_t)base + offset);

		expected = ~pattern;
		actual = hugemem_read32(p);
		if (actual != expected) {
			return ERR_IO_ERROR;
		}
	}

	return STATUS_OK;
}

/**
 * \brief Main function.
 */
int main(void)
{
	status_code_t   retval;

	board_init();
	sysclk_init();
	sleepmgr_init();

	/*
	 * Configure the EBI port with 17 address lines, enable both address
	 * latches, no low pin count mode, and set it in SRAM mode with 3-port
	 * EBI port.
	 */
	ebi_setup_port(17, 2, 0, EBI_PORT_SRAM | EBI_PORT_3PORT
			| EBI_PORT_CS0);

	/*
	 * Configure the EBI chip select for an 128 kB SRAM located at
	 * \ref BOARD_EBI_SDRAM_BASE.
	 */
	ebi_cs_set_mode(&cs_config, EBI_CS_MODE_SRAM_gc);
	ebi_cs_set_address_size(&cs_config, EBI_CS_ASPACE_128KB_gc);
	ebi_cs_set_base_address(&cs_config, BOARD_EBI_SRAM_BASE);

	/*
	 * Further, setup the SRAM timing. The SRAM device has a 10 ns delay
	 * from address to data ready. The EBI is configured to run at maximum
	 * speed, 64 MHz, which gives a minimum wait state of 16 ns per clock
	 * cycle. 1 clock cycle as wait state gives enough headroom.
	 */
	ebi_cs_set_sram_wait_states(&cs_config, EBI_CS_SRWS_1CLK_gc);

	/* Write the chip select configuration into the EBI registers. */
	ebi_cs_write_config(0, &cs_config);

	ebi_enable_cs(0, &cs_config);

	/* Enable LED0: EBI is configured and enabled. */
	gpio_set_pin_low(LED0_GPIO);

	retval = ebi_test_data_bus((hugemem_ptr_t)BOARD_EBI_SRAM_BASE);
	if (retval) {
		goto out;
	}

	/* Enable LED1: data bus test completed successfully. */
	gpio_set_pin_low(LED1_GPIO);

	retval = ebi_test_addr_bus((hugemem_ptr_t)BOARD_EBI_SRAM_BASE,
			BOARD_EBI_SRAM_SIZE);
	if (retval) {
		goto out;
	}

	/* Enable LED2: address bus test completed successfully. */
	gpio_set_pin_low(LED2_GPIO);

	retval = ebi_test_data_integrity((hugemem_ptr_t)BOARD_EBI_SRAM_BASE,
			BOARD_EBI_SRAM_SIZE);
	if (retval) {
		goto out;
	}

	/* Enable LED3: data integrity test completed successfully. */
	gpio_set_pin_low(LED3_GPIO);

out:
	/* Enable LED7: example application has completed. */
	gpio_set_pin_low(LED7_GPIO);

	for (;;) {
		sleepmgr_enter_sleep();
	}
}
