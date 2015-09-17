/*****************************************************************************
 *
 * \file
 *
 * \brief HMATRIX example for AVR UC3.
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
 *****************************************************************************/

/*! \mainpage
 * \section intro Introduction
 *
 * This example demonstrates how to use the HMATRIX module.
 * This example show number of cycle save between two configuration.
 * In the present example, function is called NB_TOGGLE time and
 *    the number of cycle is measured.
 *  The first test is done with Slave Default Master Type configured with default value.
 *  The second test is done with Slave Default Master Type configured with Last Default Master.
 *
 *  The number of cycle is save for both tests. As expected the number of cycle is lower for test2.
 *
 * \section files Main Files
 * - hmatrix_example.c: HMATRIX example application.
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR UC3. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR UC3 devices with an HMATRIX and a USART module can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 *
 * with the following setup:
 * - EVK1100 evaluation kit with an AT32UC3A0512 MCU.
 * - EVK1101 evaluation kit with an AT32UC3B0256 MCU.
 * - EVK1104 evaluation kit with an AT32UC3A3256 MCU.
 * - EVK1105 evaluation kit with an AT32UC3A0512 MCU.
 * - UC3L_EK evaluation kit with an AT32UC3L064 MCU.
 * - UC3C_EK evaluation kit with an AT32UC3C0512C MCU.
 * - STK600+RCUC3D routing card with the following configuration :
 *     - connect STK600.PA4 to STK600.LEDS.LED0
 * - CPU clock:
 *     -- 12 MHz : EVK1100, EVK1101, EVK1104, EVK1105, UC3L_EK, STK600+RCUC3D evaluation kit;
 *     -- 16 MHz : UC3C_EK evaluation kit;
 * - USART :
 *     -- USART2 (on EVK1100, EVK1101) connected to a PC serial port ;
 *     -- RS232 (on STK600) connected to a PC serial port ;
 *     -- USART2 (on EVK1104, EVK1105, UC3C_EK, UC3L_EK)  connected to a PC
 *           serial port abstracted with a USB CDC connection to a PC;
 *     -- STK600.RS232 (on STK600+RCUC3D) connected to a PC serial port ;

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

#include <string.h>

#include "board.h"
#include "print_funcs.h"
#include "gpio.h"
#include "power_clocks_lib.h"
#include "usart.h"

/*! \name USART Settings
 */
//! @{
#if BOARD == EVK1100
#  define EXAMPLE_USART                 (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN          AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART1_TXD_0_0_FUNCTION
#  define FPBA                          FOSC0
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART1_CLK_PBA
#elif BOARD == EVK1101
#  define EXAMPLE_USART                 (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN          AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART1_TXD_0_0_FUNCTION
#  define FPBA                          FOSC0
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART1_CLK_PBA
#elif BOARD == UC3C_EK
#  define EXAMPLE_USART                 (&AVR32_USART2)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART2_RXD_0_1_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART2_RXD_0_1_FUNCTION
#  define EXAMPLE_USART_TX_PIN          AVR32_USART2_TXD_0_1_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART2_TXD_0_1_FUNCTION
#  define FPBA                          FOSC0
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART2_CLK_PBA
#elif BOARD == EVK1104
#  define EXAMPLE_USART                 (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN          AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART1_TXD_0_0_FUNCTION
#  define FPBA                          FOSC0
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART1_CLK_PBA
#elif BOARD == EVK1105
#  define EXAMPLE_USART                 (&AVR32_USART0)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART0_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART0_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN          AVR32_USART0_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART0_TXD_0_0_FUNCTION
#  define FPBA                          FOSC0
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART0_CLK_PBA
#elif BOARD == STK600_RCUC3L0
#  define EXAMPLE_USART                 (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART1_RXD_0_1_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART1_RXD_0_1_FUNCTION
// For the RX pin, connect STK600.PORTE.PE3 to STK600.RS232 SPARE.RXD
#  define EXAMPLE_USART_TX_PIN          AVR32_USART1_TXD_0_1_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART1_TXD_0_1_FUNCTION
// For the TX pin, connect STK600.PORTE.PE2 to STK600.RS232 SPARE.TXD
#  define FPBA                          FOSC0
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART1_CLK_PBA
#elif BOARD == STK600_RCUC3D
#  define EXAMPLE_USART                 (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART1_RXD_0_0_FUNCTION
// For the RX pin, connect STK600.PORTE.PD0 to STK600.RS232 SPARE.RXD
#  define EXAMPLE_USART_TX_PIN          AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART1_TXD_0_0_FUNCTION
// For the TX pin, connect STK600.PORTE.PC7 to STK600.RS232 SPARE.TXD
#  define FPBA                          FOSC0
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART1_CLK_PBA
#elif BOARD == UC3L_EK
#  define EXAMPLE_USART                 (&AVR32_USART3)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART3_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART3_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN          AVR32_USART3_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART3_TXD_0_0_FUNCTION
#  define EXAMPLE_TARGET_DFLL_FREQ_HZ   96000000  // DFLL target frequency, in Hz
#  define EXAMPLE_TARGET_MCUCLK_FREQ_HZ 12000000  // MCU clock target frequency, in Hz
#  define EXAMPLE_TARGET_PBACLK_FREQ_HZ 12000000  // PBA clock target frequency, in Hz
#  define FPBA                          EXAMPLE_TARGET_PBACLK_FREQ_HZ
    /*! \name Parameters to pcl_configure_clocks().
     */
    //! @{
    static scif_gclk_opt_t gc_dfllif_ref_opt = { SCIF_GCCTRL_SLOWCLOCK, 0, false };
    static pcl_freq_param_t pcl_dfll_freq_param =
    {
      .main_clk_src = PCL_MC_DFLL0,
      .cpu_f        = EXAMPLE_TARGET_MCUCLK_FREQ_HZ,
      .pba_f        = EXAMPLE_TARGET_PBACLK_FREQ_HZ,
      .pbb_f        = EXAMPLE_TARGET_PBACLK_FREQ_HZ,
      .dfll_f       = EXAMPLE_TARGET_DFLL_FREQ_HZ,
      .pextra_params = &gc_dfllif_ref_opt
    };
    //! @}
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART3_CLK_PBA
#elif BOARD == UC3_A3_XPLAINED
#  define EXAMPLE_USART                 (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN          AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART1_TXD_0_0_FUNCTION
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART1_CLK_PBA
#  define FPBA                          FOSC0
#endif

#if !defined(EXAMPLE_USART)             || \
    !defined(EXAMPLE_USART_RX_PIN)      || \
    !defined(EXAMPLE_USART_RX_FUNCTION) || \
    !defined(EXAMPLE_USART_TX_PIN)      || \
    !defined(EXAMPLE_USART_TX_FUNCTION)
#  error The USART preprocessor configuration to use in this example is missing.
#endif

// Number of loop in the example
#define NB_TOGGLE 512

int32_t toggle_led(uint32_t number_of_toggles);

/*! \brief Toggle LED
 */
int32_t toggle_led(uint32_t number_of_toggles)
{
	volatile uint32_t start_count, end_count;
	int32_t result = 0;

	start_count = Get_system_register(AVR32_COUNT);

	for (uint32_t i = 0; i < number_of_toggles; i++) {
		LED_Toggle(LED0);
	}

	end_count = Get_system_register(AVR32_COUNT);
	result = end_count - start_count;

	print(EXAMPLE_USART, "       - Number of cycles: ");
	print_ulong(EXAMPLE_USART, result);
	print(EXAMPLE_USART, "\r\n");

	return result;
}


/*! \brief Initializes the HSB bus matrix.
 */
static void configure_hmatrix(uint32_t mode)
{
	// Configure all Slave in Last Default Master
#if (defined AVR32_HMATRIX)
	for(uint32_t i = 0; i < AVR32_HMATRIX_SLAVE_NUM; i++) {
		AVR32_HMATRIX.SCFG[i].defmstr_type = mode;
	}
#endif
#if (defined AVR32_HMATRIXB)
	for(uint32_t i = 0;i < AVR32_HMATRIXB_SLAVE_NUM; i++) {
		AVR32_HMATRIXB.SCFG[i].defmstr_type = mode;
	}
#endif
}

/*! \brief The main function.
 */
int main(void)
{
	int32_t result;

#if UC3L /* UC3L series only */
	// Note: on the AT32UC3L-EK board, there is no crystal/external clock connected
	// to the OSC0 pinout XIN0/XOUT0. We shall then program the DFLL and switch the
	// main clock source to the DFLL.
	pcl_configure_clocks(&pcl_dfll_freq_param);
	// Note: since it is dynamically computing the appropriate field values of the
	// configuration registers from the parameters structure, this function is not
	// optimal in terms of code size. For a code size optimal solution, it is better
	// to create a new function from pcl_configure_clocks_dfll0() and modify it
	// to use preprocessor computation from pre-defined target frequencies.
#else // else for all other series
	// Configure Osc0 in crystal mode (i.e. use of an external crystal source, with
	// frequency FOSC0) with an appropriate startup time then switch the main clock
	// source to Osc0.
	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);
#endif

	const gpio_map_t usart_gpio_map =
	{
		{EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
		{EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION}
	};

	// Assign GPIO to USART.
	gpio_enable_module(usart_gpio_map,
			sizeof(usart_gpio_map) / sizeof(usart_gpio_map[0]));

	static const usart_options_t usart_options =
	{
		.baudrate     = 57600,
		.charlength   = 8,
		.paritytype   = USART_NO_PARITY,
		.stopbits     = USART_1_STOPBIT,
		.channelmode  = 0
	};

	// Initialize USART in RS232 mode.
	usart_init_rs232(EXAMPLE_USART, &usart_options, FPBA);

	usart_write_line(EXAMPLE_USART, "\x1B[2J\x1B[H\r\nATMEL\r\n");
	usart_write_line(EXAMPLE_USART, "AVR UC3 - HMATRIX example\r\n\r\n");

	// First test with AVR32_HMATRIX_DEFMSTR_TYPE_NO_DEFAULT
	print(EXAMPLE_USART, "- Test 1 ----------------------------------------\r\n");
	print(EXAMPLE_USART, "------ All Slave Default Master Types are:  ------\r\n");
	print(EXAMPLE_USART, "       - No Default Master\r\n");
	print(EXAMPLE_USART, "       - leds Toggle: ");
	print_ulong(EXAMPLE_USART,  NB_TOGGLE);
	print(EXAMPLE_USART, " times\r\n");

#if defined(AVR32_HMATRIX)
	configure_hmatrix(AVR32_HMATRIX_DEFMSTR_TYPE_NO_DEFAULT);
#elif defined(AVR32_HMATRIXB)
	configure_hmatrix(AVR32_HMATRIXB_DEFMSTR_TYPE_NO_DEFAULT);
#endif

	result = toggle_led(NB_TOGGLE);

	// Second test with AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT
	print(EXAMPLE_USART, "- Test 2 ----------------------------------------\r\n");
	print(EXAMPLE_USART, "------ All Slave Default Master Types are:  ------\r\n");
	print(EXAMPLE_USART, "       - Last Default Master\r\n");
	print(EXAMPLE_USART, "       - No Default Master\r\n");
	print(EXAMPLE_USART, "       - leds Toggle: ");
	print_ulong(EXAMPLE_USART,  NB_TOGGLE);
	print(EXAMPLE_USART, " times\r\n");

#if defined(AVR32_HMATRIX)
	configure_hmatrix(AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT);
#elif defined(AVR32_HMATRIXB)
	configure_hmatrix(AVR32_HMATRIXB_DEFMSTR_TYPE_LAST_DEFAULT);
#endif

	result -= toggle_led(NB_TOGGLE);

	print(EXAMPLE_USART, "--------------------------------------------------\r\n");

	print_ulong(EXAMPLE_USART, result);
	print(EXAMPLE_USART, " Cycles saved between test 1 and test 2\r\nDone!");

	//*** Sleep mode
	// This program won't be doing anything else from now on, so it might as well
	// sleep.
	// Modules communicating with external circuits should normally be disabled
	// before entering a sleep mode that will stop the module operation.
	// Make sure the USART dumps the last message completely before turning it off.
	while(!usart_tx_empty(EXAMPLE_USART));
	pcl_disable_module(EXAMPLE_USART_CLOCK_MASK);

	// Since we're going into a sleep mode deeper than IDLE, all HSB masters must
	// be stopped before entering the sleep mode: none is acting currently in this
	// example so nothing to do for this example.

	// If there is a chance that any PB write operations are incomplete, the CPU
	// should perform a read operation from any register on the PB bus before
	// executing the sleep instruction.
	AVR32_INTC.ipr[0];  // Dummy read

	// Go to STATIC sleep mode.
	SLEEP(AVR32_PM_SMODE_STATIC);

	while (true);
}

