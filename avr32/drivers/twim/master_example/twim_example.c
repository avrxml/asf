/*! \file *********************************************************************
 *
 * \brief Example for TWIM driver implementation.
 *
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 with TWIM controller.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
/*****************************************************************************
 *
 * \file
 *
 * \brief Example for TWIM driver implementation.
 *
 *****************************************************************************/

/**
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

 /** \mainpage
   * \section intro Introduction
   * This is the documentation for the TWIM driver.
   *
   * This example demonstrates how to use the TWI interface with TWIM Module.
   *
   * \section files Main Files
   * - twim_example.c: the TWIM Master example
   *
   * \section compilinfo Compilation Information
   * This software is written for GNU GCC for AVR32 and for IAR Embedded
   * Workbench for Atmel AVR32. Other compilers may or may not work.
   *
   * \section deviceinfo Device Information
   * All AVR32 devices with a TWIM can be used.
   *
   * \section configinfo Configuration Information
   * This example has been tested with the following configuration:
   * - EVK1104, UC3C_EK, UC3L_EK, STK600+RCUC3D Routing board.
   * - CPU clock: 12 MHz;
   * - USART abstracted with a USB CDC connection to a PC;
   *   port via a standard RS232 DB9 cable;
   * - PC terminal settings:
   *   - 57600 bps,
   *   - 8 data bits,
   *   - no parity bit,
   *   - 1 stop bit,
   *   - no flow control.
   *
   * \section contactinfo Contact Information
   * For further information, visit
   * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
   * Support and FAQ: http://www.atmel.com/design-support/
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

// Include Files
#include "print_funcs.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "twim.h"
#include "gpio.h"
#include <stdio.h>
#include <stdint.h>


//------------------  C O N F I G U R A T I O N S  ----------------------------
/**
 * \brief Information about Clock configuration and TWIM Module configuration
 * \name System Clock Frequencies
 */
//! @{
#define FCPU_HZ       12000000  // CPU Frequency
#define FPBA_HZ       12000000  // PBA Frequency
#if BOARD == UC3L_EK
#define DFLL_FREQ_HZ  96000000  // DFLL target frequency, in Hz
static scif_gclk_opt_t gc_dfllif_ref_opt = { SCIF_GCCTRL_SLOWCLOCK, 0, false};
static pcl_freq_param_t pcl_freq_param =
{
	.main_clk_src = PCL_MC_DFLL0,
	.cpu_f        = FCPU_HZ,
	.pba_f        = FPBA_HZ,
	.dfll_f       = DFLL_FREQ_HZ,
	.pextra_params = &gc_dfllif_ref_opt
};
#else
static pcl_freq_param_t pcl_freq_param = {
	.cpu_f = FCPU_HZ,
	.pba_f = FPBA_HZ,
	.osc0_f = FOSC0,
	.osc0_startup = OSC0_STARTUP
};
#endif
//! @}

/**
 * \brief Information about TWIM Module
 *
 * \name TWIM Module Configuration
 */
//! @{
#if BOARD == STK600_RCUC3D
#define TWIM               (&AVR32_TWIM)  //! TWIM Module Used
#else
#define TWIM               (&AVR32_TWIM0)  //! TWIM Module Used
#endif
#define TARGET_ADDRESS     0x05            //! Target's TWI address
#define TARGET_ADDR_LGT    3               //! Internal Address length
#define VIRTUALMEM_ADDR    0x123456        //! Internal Address
#define TWIM_MASTER_SPEED  50000           //! Speed of TWI
//! @}

//------------------  D E F I N I T I O N S  ----------------------------------
// Constants to define the sent and received pattern
//! Number of test bytes
#define PATTERN_TEST_LENGTH (sizeof(write_data)/sizeof(uint8_t))
//! Array to store the test data for sending
const uint8_t write_data[] = {
	0xAA,
	0x55,
	0xA5,
	0x5A,
	0x77,
	0x99
};
//! Array to store the received test data
uint8_t read_data[6];

//-----------------------------------------------------------------------------
//-------------------------- T W I M   M A S T E R ----------------------------
//-----------------------------------------------------------------------------

/**
 *
 * \remarks The function will initialize the clocks.
 */
static void init_sys_clocks (void)
{
	if (pcl_configure_clocks (&pcl_freq_param) != PASS) {
		while (1){
			cpu_relax();
		}
	}
}

/**
 * \remarks TWI interface initialization
 *
 */
static void twim_init (void)
{
	int8_t status;
#if BOARD == UC3L_EK
	/**
	* \internal For UC3L devices, TWI default pins are,
	* TWIMS0 -> PB05,PA21
	* TWIMS1 -> PB04
	* To enable TWI clock/data in another pin, these have
	* to be assigned to other peripherals or as GPIO.
	* \endinternal
	 */
	/**
	* \internal
	* PIN 1 & 2 in Header J44 can be used in UC3L_EK
	* \endinternal
	*/
	gpio_enable_gpio_pin(AVR32_PIN_PB05);
	gpio_enable_gpio_pin(AVR32_PIN_PA21);
	const gpio_map_t TWIM_GPIO_MAP = {
		{AVR32_TWIMS0_TWCK_0_0_PIN, AVR32_TWIMS0_TWCK_0_0_FUNCTION},
		{AVR32_TWIMS0_TWD_0_2_PIN, AVR32_TWIMS0_TWD_0_2_FUNCTION}
	};
#elif BOARD == STK600_RCUC3D
	/**
	* \internal For UC3D devices, TWI default pins are,
	* TWIMS -> PB09 (TWCK),PA10 (TWD)
	* To enable TWI clock/data in another pin, these have
	* to be assigned to other peripherals or as GPIO.
	* \endinternal
	 */
	/**
	* \internal
	* TWCK can be found on header PORTB.PB1 of the STK600 with RCUC3D
	* TWD can be found on header PORTB.PB2 of the STK600 with RCUC3D
	* \endinternal
	*/
	gpio_enable_gpio_pin(AVR32_PIN_PB09);
	gpio_enable_gpio_pin(AVR32_PIN_PA10);
	const gpio_map_t TWIM_GPIO_MAP = {
		{AVR32_TWIMS_TWCK_0_PIN, AVR32_TWIMS_TWCK_0_FUNCTION},
		{AVR32_TWIMS_TWD_0_PIN, AVR32_TWIMS_TWD_0_FUNCTION}
	};
#else
	/**
	* \internal
	* PIN 2 & 3 in Header J24 can be used in EVK1104
	* PIN 1 & 2 in Header J44 can be used in UC3C_EK
	* \endinternal
	*/
	const gpio_map_t TWIM_GPIO_MAP = {
	{AVR32_TWIMS0_TWCK_0_0_PIN, AVR32_TWIMS0_TWCK_0_0_FUNCTION},
	{AVR32_TWIMS0_TWD_0_0_PIN, AVR32_TWIMS0_TWD_0_0_FUNCTION}
	};
#endif
	// Set TWIM options
	const twi_options_t TWIM_OPTIONS = {
		.pba_hz = FPBA_HZ,
		.speed = TWIM_MASTER_SPEED,
		.chip = TARGET_ADDRESS,
		.smbus = false,
	};
	// TWIM gpio pins configuration
	gpio_enable_module (TWIM_GPIO_MAP,
			sizeof (TWIM_GPIO_MAP) / sizeof (TWIM_GPIO_MAP[0]));
	// Initialize as master.
	status = twim_master_init (TWIM, &TWIM_OPTIONS);
	// Check whether TARGET device is connected
	if (status == STATUS_OK) {
		// display test result to user
		print_dbg ("TARGET SLAVE FIND:\tPASS\r\n");
	} else {
		// display test result to user
		print_dbg ("TARGET CHIP FIND:\tFAILED\r\n");
	}
}

/**
 * \remarks Main function.
 * @{
 */
int main (void)
{
	twi_package_t packet, packet_received;
	uint8_t i;
	status_code_t status;
	// Configure the system clock
	init_sys_clocks ();
	// Init debug serial line
	init_dbg_rs232 (FPBA_HZ);
	// Display a header to user
	print_dbg ("Master Example\r\n");
	print_dbg ("Master Started\r\n");
	// Initialize and enable interrupt
	irq_initialize_vectors();
	cpu_irq_enable();
	// Initialize the TWIM Module
	twim_init ();
	// TWI Packet Configuration
	// TWI chip address to communicate with
	packet.chip = TARGET_ADDRESS;
	// TWI address/commands to issue to the other chip (node)
	packet.addr[0] = (VIRTUALMEM_ADDR >> 16) & 0xff;
	packet.addr[1] = (VIRTUALMEM_ADDR >> 8) & 0xff;
	packet.addr[2] = VIRTUALMEM_ADDR & 0xff;
	// Length of the TWI data address segment (1-3 bytes)
	packet.addr_length = TARGET_ADDR_LGT;
	// Where to find the data to be written
	packet.buffer = (void *) write_data;
	// How many bytes do we want to write
	packet.length = PATTERN_TEST_LENGTH;
	print_dbg ("Writing data to TARGET\r\n");
	// Write data to TARGET
	status = twi_master_write (TWIM, &packet);
	// Check status of transfer
	if (status == STATUS_OK) {
		// display test result to user
		print_dbg ("WRITE TEST:\tPASS\r\n");
	} else {
		// display test result to user
		print_dbg ("WRITE TEST:\tFAILED\r\n");
	}
	// TWI chip address to communicate with
	packet_received.chip = TARGET_ADDRESS;
	// Length of the TWI data address segment (1-3 bytes)
	packet_received.addr_length = TARGET_ADDR_LGT;
	// How many bytes do we want to write
	packet_received.length = PATTERN_TEST_LENGTH;
	// TWI address/commands to issue to the other chip (node)
	packet_received.addr[0] = (VIRTUALMEM_ADDR >> 16) & 0xff;
	packet_received.addr[1] = (VIRTUALMEM_ADDR >> 8) & 0xff;
	packet_received.addr[2] = VIRTUALMEM_ADDR & 0xff;
	// Where to find the data to be written
	packet_received.buffer = read_data;
	print_dbg ("Reading data from TARGET\r\n");
	// Read data from TARGET
	status = twi_master_read (TWIM, &packet_received);
	// Check Status
	if (status == STATUS_OK) {
		// Display test result to user
		print_dbg ("READ TEST:\tPASS\r\n");
	} else {
		// Display test result to user
		print_dbg ("READ TEST:\tFAILED\r\n");
	  twi_master_read (TWIM, &packet_received);
	}
	// Check received data against sent data
	for (i = 0; i < PATTERN_TEST_LENGTH; i++) {
		if (read_data[i] != write_data[i]) {
			// Error
			print_dbg ("Check Read:\tFAIL\r\n");
			while (1){
				cpu_relax();
			}
		}
	}
	// No errors in communication
	print_dbg ("- Done\r\n");
	while (1){
		cpu_relax();
	}
}
//! @}

// END OF FILE
