/**
 * \file
 *
 * \brief Example of flash access using the FLASHCDW driver.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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

/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the FLASHCDW software driver.
 *
 * It also comes bundled with an example. This example demonstrates flash read /
 * write data accesses, using a flash block as an NVRAM, located either in the
 * flash array or in the User page.
 *
 * Operating mode of the example:
 *   -# After reset, the NVRAM variables are displayed on the USART link.
 *   -# The NVRAM is cleared (all bytes are set to 0x00).
 *   -# All NVRAM variables are written with incrementing nibbles, starting from
 *      0x0.
 *   -# The user can reset or power-cycle the board to check the
 *      non-volatileness of the NVRAM.
 *
 * \section files Main Files
 *   - flashcdw.c: FLASHCDW driver;
 *   - flashcdw.h: FLASHCDW driver header file;
 *   - flash_example.c: flash access example application.
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with a FLASHCDW module can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 *   - AT32UC3L-EK evaluation kit; STK600+RCUC3L0 routing card ; STK600+RCUC3D routing card
 *   - CPU clock: 12 MHz;
 *   - USART3 (on AT32UC3L-EK) abstracted with a USB CDC connection to a PC;
 *     STK600 usart port for the STK600+RCUC3L0 setup :
 *       - connect STK600.PE2 to STK600.RS232 SPARE.TXD
 *       - connect STK600.PE3 to STK600.RS232 SPARE.RXD
 *     STK600 usart port for the STK600+RCUC3D setup :
 *       - connect STK600.PC7 to STK600.RS232 SPARE.TXD
 *       - connect STK600.PD0 to STK600.RS232 SPARE.RXD
 *   - PC terminal settings:
 *     - 57600 bps,
 *     - 8 data bits,
 *     - no parity bit,
 *     - 1 stop bit,
 *     - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include "compiler.h"
#include "print_funcs.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "flashcdw.h"

#if BOARD == UC3L_EK
#  define EXAMPLE_TARGET_DFLL_FREQ_HZ   96000000  // DFLL target frequency, in Hz
#  define EXAMPLE_TARGET_MCUCLK_FREQ_HZ 12000000  // MCU clock target frequency, in Hz
#  define EXAMPLE_TARGET_PBACLK_FREQ_HZ 12000000  // PBA clock target frequency, in Hz
	/*! \name Parameters to pcl_configure_clocks().
	 */
	//! @{
	static scif_gclk_opt_t gc_dfllif_ref_opt = { SCIF_GCCTRL_SLOWCLOCK, 0, false};
	static pcl_freq_param_t pcl_dfll_freq_param = {
		.main_clk_src  = PCL_MC_DFLL0,
		.cpu_f         = EXAMPLE_TARGET_MCUCLK_FREQ_HZ,
		.pba_f         = EXAMPLE_TARGET_PBACLK_FREQ_HZ,
		.pbb_f         = EXAMPLE_TARGET_PBACLK_FREQ_HZ,
		.dfll_f        = EXAMPLE_TARGET_DFLL_FREQ_HZ,
		.pextra_params = &gc_dfllif_ref_opt
	};
	//! @}
#else
# define EXAMPLE_TARGET_PBACLK_FREQ_HZ FOSC0  // PBA clock target frequency, in Hz
#endif

//! Structure type containing variables to store in NVRAM using a specific
//! memory map.
typedef const struct {
	uint8_t  var8;
	uint16_t var16;
	uint8_t  var8_3[3];
	uint32_t var32;
} nvram_data_t;


//! NVRAM data structure located in the flash array.
#if __GNUC__
__attribute__((__section__(".flash_nvram")))
#endif
static nvram_data_t flash_nvram_data
#if __ICCAVR32__
@ "FLASH_NVRAM"
#endif
;

//! NVRAM data structure located in the User page.
#if defined(__GNUC__)
__attribute__((__section__(".userpage")))
#elif defined(__ICCAVR32__)
__no_init
// This code is added so that this example can be programmed through
// batchisp and a bootloader. Else, IAR will set this variable as
// loadable and batchisp will err because this variable is out of the
// flash memory range (it's in the user page).
// GCC will init this variable at run time not during the programming
// of the application.
#endif
static nvram_data_t user_nvram_data
#if defined(__ICCAVR32__)
@ "USERDATA32_C"
#endif
;


/*! \brief Prints the variables stored in NVRAM.
 *
 * \param nvram_data  Pointer to the NVRAM data structure to print.
 */
static void print_nvram_variables(nvram_data_t *nvram_data)
{
	print_dbg("var8:\t0x");
	print_dbg_char_hex(nvram_data->var8);

	print_dbg("\r\nvar16:\t0x");
	print_dbg_short_hex(nvram_data->var16);

	print_dbg("\r\nvar8_3:\t0x");
	print_dbg_char_hex(nvram_data->var8_3[0]);
	print_dbg_char_hex(nvram_data->var8_3[1]);
	print_dbg_char_hex(nvram_data->var8_3[2]);

	print_dbg("\r\nvar32:\t0x");
	print_dbg_hex(nvram_data->var32);

	print_dbg("\r\n");
}


/*! \brief This is an example demonstrating flash read / write data accesses
 *         using the FLASHCDW driver.
 *
 * \param caption     Caption to print before running the example.
 * \param nvram_data  Pointer to the NVRAM data structure to use in the example.
 */
static void flash_rw_example(const char *caption, nvram_data_t *nvram_data)
{
	static const uint8_t write_data[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};

	print_dbg(caption);

	print_dbg("Initial values of NVRAM variables:\r\n");
	print_nvram_variables(nvram_data);

	print_dbg("\r\nClearing NVRAM variables...");
	flashcdw_memset((void *)nvram_data, 0x00, 8, sizeof(*nvram_data), true);
	print_dbg("\r\nNVRAM variables cleared:\r\n");
	print_nvram_variables(nvram_data);

	print_dbg("\r\nWriting new values to NVRAM variables...");
	flashcdw_memcpy((void *)&nvram_data->var8,   &write_data, sizeof(nvram_data->var8),   true);
	flashcdw_memcpy((void *)&nvram_data->var16,  &write_data, sizeof(nvram_data->var16),  true);
	flashcdw_memcpy((void *)&nvram_data->var8_3, &write_data, sizeof(nvram_data->var8_3), true);
	flashcdw_memcpy((void *)&nvram_data->var32,  &write_data, sizeof(nvram_data->var32),  true);
	print_dbg("\r\nNVRAM variables written:\r\n");
	print_nvram_variables(nvram_data);
}


/*! \brief Main function running the example on both the flash array and the
 *         User page.
 */
int main(void)
{
#if BOARD == UC3L_EK
	// Note: on the AT32UC3L-EK board, there is no crystal/external clock connected
	// to the OSC0 pinout XIN0/XOUT0. We shall then program the DFLL and switch the
	// main clock source to the DFLL.
	pcl_configure_clocks(&pcl_dfll_freq_param);
	// Note: since it is dynamically computing the appropriate field values of the
	// configuration registers from the parameters structure, this function is not
	// optimal in terms of code size. For a code size optimal solution, it is better
	// to create a new function from pcl_configure_clocks_dfll0() and modify it
	// to use preprocessor computation from pre-defined target frequencies.
#else
	// Configure Osc0 in crystal mode (i.e. use of an external crystal source, with
	// frequency FOSC0) with an appropriate startup time then switch the main clock
	// source to Osc0.
	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);
#endif

	// Initialize the debug USART module.
	init_dbg_rs232(EXAMPLE_TARGET_PBACLK_FREQ_HZ);

	// Apply the example to the flash array.
	flash_rw_example("\x0C=== Using a piece of the flash array as NVRAM ===\r\n",
			&flash_nvram_data);

	// Apply the example to the User page.
	flash_rw_example("\r\n\r\n=== Using a piece of the User page as NVRAM ===\r\n",
			&user_nvram_data);

	//*** Sleep mode
	// This program won't be doing anything else from now on, so it might as well
	// sleep.
	// Modules communicating with external circuits should normally be disabled
	// before entering a sleep mode that will stop the module operation.
	// For this application, we must disable the USART module that the DEBUG
	// software module is using.
	pcl_disable_module(DBG_USART_CLOCK_MASK);

	// Since we're going into a sleep mode deeper than IDLE, all HSB masters must
	// be stopped before entering the sleep mode.
	pcl_disable_module(AVR32_PDCA_CLK_HSB);
	pcl_disable_module(AVR32_PDCA_CLK_PBA);

	// If there is a chance that any PB write operations are incomplete, the CPU
	// should perform a read operation from any register on the PB bus before
	// executing the sleep instruction.
	AVR32_INTC.ipr[0];  // Dummy read

	// Go to STATIC sleep mode.
	SLEEP(AVR32_PM_SMODE_STATIC);

	while (true);
}
