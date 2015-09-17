/*****************************************************************************
 *
 * \file
 *
 * \brief Example for TWIS Module driver.
 *
 *****************************************************************************/

/**
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
 /** \mainpage
   * \section intro Introduction
   * This is the documentation for the TWIS Module driver.
   *
   * This example demonstrates how to use the TWI interface with TWIS modules.
   *
   * \section files Main Files
   * - twis_example.c: the TWIS Module example
   *
   * \section compilinfo Compilation Information
   * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
   * for Atmel AVR32. Other compilers may or may not work.
   *
   * \section deviceinfo Device Information
   * All AVR32 devices with a TWIS can be used.
   *
   * \section configinfo Configuration Information
   * This example has been tested with the following configuration:
   * - UC3C-EK or EVK1104 or UC3L-EK
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

// Include Files
#include "print_funcs.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "twis.h"
#include "gpio.h"
#include <stdint.h>
#include <stdio.h>

//------------------  C O N F I G U R A T I O N S  ----------------------------
/**
 * \brief Information about Clock configuration and TWIM Module configuration
 * \name System Clock Frequencies
 */
//! @{
#define FCPU_HZ         12000000   // CPU Frequency
#define FPBA_HZ         12000000   // PBA Frequency
#if (BOARD == UC3L_EK)
#define DFLL_FREQ_HZ    96000000   // DFLL target frequency, in Hz
static scif_gclk_opt_t gc_dfllif_ref_opt = { SCIF_GCCTRL_SLOWCLOCK, 0, false};
static pcl_freq_param_t pcl_freq_param =
{
	.main_clk_src  = PCL_MC_DFLL0,
	.cpu_f         = FCPU_HZ,
	.pba_f         = FPBA_HZ,
	.dfll_f        = DFLL_FREQ_HZ,
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

#define TEST_TWIS_TWCK_PIN         AVR32_TWIMS0_TWCK_0_0_PIN
#define TEST_TWIS_TWCK_FUNCTION    AVR32_TWIMS0_TWCK_0_0_FUNCTION
#if (BOARD == UC3L_EK)
/**
* \internal
* PIN 1 & 2 in Header J44 can be used in UC3L_EK
* \endinternal
*/
#define TEST_TWIS_TWD_PIN          AVR32_TWIMS0_TWD_0_2_PIN
#define TEST_TWIS_TWD_FUNCTION     AVR32_TWIMS0_TWD_0_2_FUNCTION
#else
/**
* \internal
* PIN 2 & 3 in Header J24 can be used in EVK1104
* PIN 1 & 2 in Header J44 can be used in UC3C_EK
* \endinternal
*/
#define TEST_TWIS_TWD_PIN          AVR32_TWIMS0_TWD_0_0_PIN
#define TEST_TWIS_TWD_FUNCTION     AVR32_TWIMS0_TWD_0_0_FUNCTION
#endif

//! @}

/** \brief Information about TWIM Module
 * \name TWIM Module Configuration
 */
//! @{
#define TWIS                   (&AVR32_TWIS0) // TWIM Module Used
#define SLAVE_ADDRESS          0x05           // Target's TWI address
#define SLAVE_ADDR_LGT         3              // Address length of virtual mem
#define VIRTUALMEM_ADDR_START  0x123456       // Address of virtual mem
#define TWI_SPEED              50000         // Speed of TWI
//! @}

//! \brief Defines to manage a virtual TWI memory
/*! \name Virtual Memory Management
 */
//! @{
#define TWI_MEM_SIZE    20  // The size of the virtual mem
#define TWI_MEM_IDLE    0   // Idle state
#define TWI_MEM_ADDR    1   // Address state
#define TWI_MEM_DATA    2   // Data state
//! @}

//------------------  D E F I N I T I O N S  ----------------------------------
// Constants & Variables to define the sent and received pattern

//! State variable
uint8_t s_status_cmd = TWI_MEM_IDLE;
/** Offset in the address value
 * (because we receive the address one Byte at a time)
 */
uint8_t s_u8_addr_pos;
//! The current address in the virtual mem
uint32_t s_u32_addr;
//! Content of the Virtual mem
uint8_t s_memory[TWI_MEM_SIZE] = { 0 };


/** \brief Manage the received data on TWI
*/
/*! \remarks User defined operations after reception
 */
//! @{
void twis_slave_rx( uint8_t u8_value )
{
	switch( s_status_cmd ) {
		case TWI_MEM_IDLE:
		s_u8_addr_pos = SLAVE_ADDR_LGT; // Init before receiving the target address.
		s_u32_addr = 0;
		// No break to continue on next case

		case TWI_MEM_ADDR:
		s_u8_addr_pos--;
		// Receiving the Nth Byte that makes the address (MSB first).
		s_u32_addr += ((U32)u8_value << (s_u8_addr_pos*8));
		if( 0 == s_u8_addr_pos ) {
			// the address is completely received => switch to data mode.
			s_status_cmd = TWI_MEM_DATA;
		} else {
			s_status_cmd = TWI_MEM_ADDR;
		}
		break;

		case TWI_MEM_DATA:      // We are receiving data
		// Check that we're still in the range of the virtual mem
		if( TWI_MEM_SIZE > (s_u32_addr-VIRTUALMEM_ADDR_START) ) {
			s_memory[s_u32_addr-VIRTUALMEM_ADDR_START] = u8_value;
		} else {
			s_u32_addr=VIRTUALMEM_ADDR_START;
		}
		s_u32_addr++;  // Update to next position
		break;
	}
}
//! @}

/** \brief Transmit a data on TWIS
*/
/*! \remarks User defined operations during transmission
 */
//! @{
uint8_t twis_slave_tx( void )
{
	uint8_t u8_value;
	// This callback is called after a read request from the TWI master, for each
	// Byte to transmit.
	s_status_cmd = TWI_MEM_IDLE;
	// Check that we're still in the range of the virtual mem
	if( TWI_MEM_SIZE > (s_u32_addr-VIRTUALMEM_ADDR_START) ) 	{
		u8_value = s_memory[s_u32_addr-VIRTUALMEM_ADDR_START];
	} else {
		u8_value = 0xFF;
	}
	s_u32_addr++;  // Update to next position
	return u8_value;
}
//! @}

/** \brief Manage stop transfer reception on TWIS
*/
/*! \remarks User defined operations on Stop condition
 */
//! @{
void twis_slave_stop()
{
	s_status_cmd = TWI_MEM_IDLE;
}
//! @}

/*! \brief Initializes the MCU system clocks.
*/
/*! \remarks System clock configuration
 */
//! @{
static void init_sys_clocks (void)
{
	if (pcl_configure_clocks (&pcl_freq_param) != PASS) {
		while (1);
	}
}
//! @}

/*! \brief TWIS interface initialization
*/
/*! \remarks TWIS Module Configuration
 */
//! @{
void twis_init (void)
{
	twis_slave_fct_t twis_slave_fct;
#if BOARD == UC3L_EK
	/**
	* \internal For UC3L devices, TWI default pins are,
	* TWIMS0 -> PB05,PA21
	* TWIMS1 -> PB04
	* To enable TWI clock/data in another pin, these have
	* to be assigned to other peripherals or as GPIO.
	* \endinternal
	 */
	gpio_enable_gpio_pin(AVR32_PIN_PB05);
	gpio_enable_gpio_pin(AVR32_PIN_PA21);
#endif
	const gpio_map_t TWIS_GPIO_MAP = {
		{TEST_TWIS_TWCK_PIN, TEST_TWIS_TWCK_FUNCTION},
		{TEST_TWIS_TWD_PIN, TEST_TWIS_TWD_FUNCTION}
	};
	const twis_options_t TWIS_OPTIONS = {
		.pba_hz = FPBA_HZ,
		.speed = TWI_SPEED,
		.chip = SLAVE_ADDRESS,
		.smbus = false,
	};
	// Assign I/Os to SPI.
	gpio_enable_module (TWIS_GPIO_MAP,
			sizeof (TWIS_GPIO_MAP) / sizeof (TWIS_GPIO_MAP[0]));
	// Set pointer to user specific application routines
	twis_slave_fct.rx = &twis_slave_rx;
	twis_slave_fct.tx = &twis_slave_tx;
	twis_slave_fct.stop = &twis_slave_stop;
	// Initialize as master.
	twis_slave_init (TWIS, &TWIS_OPTIONS, &twis_slave_fct);
}
//! @}

/*! \brief Main function.
 */
/*! \remarks Main Function
 */
//! @{
int main (void)
{
	// Configure the system clock
	init_sys_clocks ();
	// Init debug serial line
	init_dbg_rs232 (FPBA_HZ);
	// Display a header to user
	print_dbg ("Slave Example\r\n");
	print_dbg ("Slave Started\r\n");
	// Initialize and enable interrupt
	irq_initialize_vectors();
	cpu_irq_enable();
	// Initialize the TWIS Module
	twis_init ();
	while (true);
}
//! @}
