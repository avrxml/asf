/*****************************************************************************
 *
 * \file
 *
 * \brief SMC example application for AVR32 UC3.
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
 ******************************************************************************/

/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the SMC on EBI driver.
 *
 * This example demonstrates how to use the SMC on EBI driver with the
 * onboard SMC. It initializes the SMC, do a write access and a read back
 *
 * To use this code in your own application, make a pointer to the base address
 * of the SMC and access it like you would normally access a memory-mapped
 * device.
 *
 * The file "smc_peripheral.h" contains all declarations for Data Bus Width and timing constraints
 * Please refer to the datasheet for specific SMC module of AVR32 UC3 devices constraints.
 *
 * \section files Main Files
 * - smc.c: SMC on EBI driver;
 * - smc.h: SMC on EBI driver header file;
 * - conf_smc.h: SMC Configuration Example;
 * - conf_ebi.h: EBI Configuration Example;
 * - smc_peripheral.h: SMC Peripheral Example;
 * - smc_example.c: SMC on EBI example application.
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with an SRAM connected through the SMC can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - EVK1100 evaluation kit with additional SRAM connected to the SMC module,
 * - CPU clock: 12 MHz;
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.microchip.com/design-centers/32-bit /">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include "compiler.h"
#include "board.h"
#include "print_funcs.h"
#include "pm.h"
#include "gpio.h"
#include "smc.h"

//!  SRAM base address
#define SRAM              ((void *)AVR32_EBI_CS1_ADDRESS)

//! SRAM size.
#define SRAM_SIZE         (1 << smc_get_cs_size(1))


/*! \brief This example shows how to access an external RAM connected to the SMC module.
 */
int main(void)
{
	// Get base address of SRAM module
	volatile uint32_t *sram = SRAM;

	// Switch to external oscillator 0.
	pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

	// Initialize debug serial line
	init_dbg_rs232(FOSC0);

	// Display a header to user
	print_dbg("\x1B[2J\x1B[H\r\nSMC Example\r\n");

	print_dbg("Board running at ");
	print_dbg_ulong(FOSC0 / 1000000);
	print_dbg(" MHz\r\n");

	print_dbg("Initializing SRAM...");

	// Initialize the external SRAM chip.
	smc_init(FOSC0);
	print_dbg("done\r\n\r\n");

	print_dbg("Testing SRAM...\r\n");

	// Test each address location inside the chip with a write/readback
	uint32_t total_tests  = 0;
	uint32_t total_errors = 0;

	for (uint32_t total_tests = 0; total_tests < SRAM_SIZE; total_tests++) {
		sram[total_tests] = total_tests;

		if (total_tests != sram[total_tests]) {
			total_errors++;

			print_dbg("Error at 0x");
			print_dbg_hex((uint32_t)&sram[total_tests]);
			print_dbg("\r\n");
		}
    }

	if (total_errors == 0) {
		print_dbg("SRAM test successfully completed\r\n");
	}
	else {
		print_dbg("SRAM test completed with ");
		print_dbg_ulong(total_errors);
		print_dbg(" errors out of ");
		print_dbg_ulong(total_tests);
		print_dbg(" tests\r\n");
	}

	while (true);

	return 0;
}
