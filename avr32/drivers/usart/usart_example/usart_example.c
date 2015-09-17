/*****************************************************************************
 *
 * \file
 *
 * \brief USART example application for AVR UC3 USART driver.
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
 ******************************************************************************/

/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the USART software driver.\n It also comes
 * bundled with an example. This example is a basic Hello-World example.\n
 * <b>Example's operating mode: </b>
 * -# A message is displayed on the PC terminal ("Hello, this is AT32UC3 saying hello! (press enter)")
 * -# You may then type any character other than CR(Carriage Return) and it will
 * be echoed back to the PC terminal.
 * -# If you type a CR, "'CRLF'Goodbye." is echoed back to the PC terminal and
 * the application ends.
 *
 * \section files Main Files
 * - usart.c: USART driver;
 * - usart.h: USART driver header file;
 * - usart_example.c: USART example application.
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with a USART module can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - EVK1100, EVK1101, UC3C_EK, EVK1104, EVK1105 evaluation kits; STK600+RCUC3L routing card; AT32UC3L-EK; STK600+RCUC3L4 routing card
 * - CPU clock:
 *        -- 12 MHz : EVK1100, EVK1101, EVK1104, EVK1105, AT32UC3L-EK evaluation kits;
 *                    STK600+RCUC3L routing card; STK600+RCUC3D routing card; STK600+RCUC3L4 routing card;
 *        -- 16 MHz : AT32UC3C-EK
 * - USART1 (on EVK1100 or EVK1101) connected to a PC serial port via a standard
 *   RS232 DB9 cable, or USART0 (on EVK1105) or USART2 (on AT32UC3C-EK) or USART1 (on EVK1104)
 *   or USART3 (on AT32UC3L-EK) abstracted with a USB CDC connection to a PC;
 *   STK600 usart port for :
 *      - STK600+RCUC3L or STK600+RCUC3L4 setup (connect STK600.PE2 to STK600.RS232 SPARE.TXD and STK600.PE3 to STK600.RS232 SPARE.RXD)
 *      - STK600+RCUC3D setup (connect STK600.PC7 to STK600.RS232 SPARE.TXD and STK600.PD0 to STK600.RS232 SPARE.RXD)
 * - PC terminal settings:
 *   - 57600 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/uc3">Atmel AVR UC3</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "usart.h"

/*! \name USART Settings
 */
//! @{

#  define EXAMPLE_TARGET_PBACLK_FREQ_HZ FOSC0  // PBA clock target frequency, in Hz

#if BOARD == EVK1100
#  define EXAMPLE_USART                 (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN          AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART1_TXD_0_0_FUNCTION
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART1_CLK_PBA
#  define EXAMPLE_PDCA_CLOCK_HSB        AVR32_PDCA_CLK_HSB
#  define EXAMPLE_PDCA_CLOCK_PB         AVR32_PDCA_CLK_PBA
#elif BOARD == EVK1101
#  define EXAMPLE_USART                 (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN          AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART1_TXD_0_0_FUNCTION
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART1_CLK_PBA
#  define EXAMPLE_PDCA_CLOCK_HSB        AVR32_PDCA_CLK_HSB
#  define EXAMPLE_PDCA_CLOCK_PB         AVR32_PDCA_CLK_PBA
#elif BOARD == UC3C_EK
#  define EXAMPLE_USART                 (&AVR32_USART2)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART2_RXD_0_1_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART2_RXD_0_1_FUNCTION
#  define EXAMPLE_USART_TX_PIN          AVR32_USART2_TXD_0_1_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART2_TXD_0_1_FUNCTION
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART2_CLK_PBA
#  define EXAMPLE_PDCA_CLOCK_HSB        AVR32_PDCA_CLK_HSB
#  if UC3C_REVC
#    define EXAMPLE_PDCA_CLOCK_PB       AVR32_PDCA_CLK_PBB
#  else
#    define EXAMPLE_PDCA_CLOCK_PB       AVR32_PDCA_CLK_PBC
#  endif
#elif BOARD == EVK1104
#  define EXAMPLE_USART                 (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN          AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART1_TXD_0_0_FUNCTION
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART1_CLK_PBA
#  define EXAMPLE_PDCA_CLOCK_HSB        AVR32_PDCA_CLK_HSB
#  define EXAMPLE_PDCA_CLOCK_PB         AVR32_PDCA_CLK_PBA
#elif BOARD == EVK1105
#  define EXAMPLE_USART                 (&AVR32_USART0)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART0_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART0_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN          AVR32_USART0_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART0_TXD_0_0_FUNCTION
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART0_CLK_PBA
#  define EXAMPLE_PDCA_CLOCK_HSB        AVR32_PDCA_CLK_HSB
#  define EXAMPLE_PDCA_CLOCK_PB         AVR32_PDCA_CLK_PBA
#elif (BOARD == STK600_RCUC3L0 || BOARD == STK600_RCUC3L4)
#  define EXAMPLE_USART                 (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART1_RXD_0_1_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART1_RXD_0_1_FUNCTION
// For the RX pin, connect STK600.PORTE.PE3 to STK600.RS232 SPARE.RXD
#  define EXAMPLE_USART_TX_PIN          AVR32_USART1_TXD_0_1_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART1_TXD_0_1_FUNCTION
// For the TX pin, connect STK600.PORTE.PE2 to STK600.RS232 SPARE.TXD
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART1_CLK_PBA
#  define EXAMPLE_TARGET_DFLL_FREQ_HZ   96000000  // DFLL target frequency, in Hz
#  define EXAMPLE_TARGET_MCUCLK_FREQ_HZ 12000000  // MCU clock target frequency, in Hz
#  undef  EXAMPLE_TARGET_PBACLK_FREQ_HZ
#  define EXAMPLE_TARGET_PBACLK_FREQ_HZ 12000000  // PBA clock target frequency, in Hz
#  define EXAMPLE_PDCA_CLOCK_HSB        AVR32_PDCA_CLK_HSB
#  define EXAMPLE_PDCA_CLOCK_PB         AVR32_PDCA_CLK_PBA
#elif BOARD == UC3L_EK
#  define EXAMPLE_USART                 (&AVR32_USART3)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART3_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART3_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN          AVR32_USART3_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART3_TXD_0_0_FUNCTION
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART3_CLK_PBA
#  define EXAMPLE_TARGET_DFLL_FREQ_HZ   96000000  // DFLL target frequency, in Hz
#  define EXAMPLE_TARGET_MCUCLK_FREQ_HZ 12000000  // MCU clock target frequency, in Hz
#  undef  EXAMPLE_TARGET_PBACLK_FREQ_HZ
#  define EXAMPLE_TARGET_PBACLK_FREQ_HZ 12000000  // PBA clock target frequency, in Hz
#  define EXAMPLE_PDCA_CLOCK_HSB        AVR32_PDCA_CLK_HSB
#  define EXAMPLE_PDCA_CLOCK_PB         AVR32_PDCA_CLK_PBA
#elif BOARD == STK600_RCUC3D
#  define EXAMPLE_USART                 (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART1_RXD_0_0_FUNCTION
// For the RX pin, connect STK600.PORTD.PD0 to STK600.RS232 SPARE.RXD
#  define EXAMPLE_USART_TX_PIN          AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART1_TXD_0_0_FUNCTION
// For the TX pin, connect STK600.PORTC.PC7 to STK600.RS232 SPARE.TXD
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART1_CLK_PBA
#  define EXAMPLE_PDCA_CLOCK_HSB        AVR32_PDCA_CLK_HSB
#  define EXAMPLE_PDCA_CLOCK_PB         AVR32_PDCA_CLK_PBA
#elif BOARD == UC3_A3_XPLAINED
#  define EXAMPLE_USART                 (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN          AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART1_TXD_0_0_FUNCTION
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART1_CLK_PBA
#  define EXAMPLE_PDCA_CLOCK_HSB        AVR32_PDCA_CLK_HSB
#  define EXAMPLE_PDCA_CLOCK_PB         AVR32_PDCA_CLK_PBA
#elif BOARD == UC3_L0_XPLAINED
#  define EXAMPLE_USART                 (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN          AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART1_TXD_0_0_FUNCTION
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART1_CLK_PBA
#  define EXAMPLE_TARGET_DFLL_FREQ_HZ   96000000  // DFLL target frequency, in Hz
#  define EXAMPLE_TARGET_MCUCLK_FREQ_HZ 12000000  // MCU clock target frequency, in Hz
#  undef  EXAMPLE_TARGET_PBACLK_FREQ_HZ
#  define EXAMPLE_TARGET_PBACLK_FREQ_HZ 12000000  // PBA clock target frequency, in Hz
#  define EXAMPLE_PDCA_CLOCK_HSB        AVR32_PDCA_CLK_HSB
#  define EXAMPLE_PDCA_CLOCK_PB         AVR32_PDCA_CLK_PBA
#endif

#if !defined(EXAMPLE_USART)             || \
    !defined(EXAMPLE_USART_RX_PIN)      || \
    !defined(EXAMPLE_USART_RX_FUNCTION) || \
    !defined(EXAMPLE_USART_TX_PIN)      || \
    !defined(EXAMPLE_USART_TX_FUNCTION)
#  error The USART configuration to use in this example is missing.
#endif

//! @}

#if UC3L
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
#endif

/*! \brief This is an example demonstrating the USART RS232 TX and RX
 *         functionalities using the USART driver.
 */
int main(void)
{
#if (BOARD == UC3L_EK) || (BOARD == UC3_L0_XPLAINED) || (BOARD == STK600_RCUC3L4)
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

	static const gpio_map_t USART_GPIO_MAP =
	{
		{EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
		{EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION}
	};

	// USART options.
	static const usart_options_t USART_OPTIONS =
	{
		.baudrate     = 57600,
		.charlength   = 8,
		.paritytype   = USART_NO_PARITY,
		.stopbits     = USART_1_STOPBIT,
		.channelmode  = USART_NORMAL_CHMODE
	};

	// Assign GPIO to USART.
	gpio_enable_module(USART_GPIO_MAP,
			sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

	// Initialize USART in RS232 mode.
	usart_init_rs232(EXAMPLE_USART, &USART_OPTIONS, EXAMPLE_TARGET_PBACLK_FREQ_HZ);

	// Hello world!
	usart_write_line(EXAMPLE_USART, "Hello, this is the AVR UC3 MCU saying hello! (press enter)\r\n");

	// Press enter to continue.
	while (usart_get_echo_line(EXAMPLE_USART) == USART_FAILURE);  // Get and echo characters until end of line.

	usart_write_line(EXAMPLE_USART, "Goodbye.\r\n");

	//*** Sleep mode
	// This program won't be doing anything else from now on, so it might as well
	// sleep.
	// Modules communicating with external circuits should normally be disabled
	// before entering a sleep mode that will stop the module operation.
	// Make sure the USART dumps the last message completely before turning it off.
	while(!usart_tx_empty(EXAMPLE_USART));
	pcl_disable_module(EXAMPLE_USART_CLOCK_MASK);

	// Since we're going into a sleep mode deeper than IDLE, all HSB masters must
	// be stopped before entering the sleep mode.
	pcl_disable_module(EXAMPLE_PDCA_CLOCK_HSB);
	pcl_disable_module(EXAMPLE_PDCA_CLOCK_PB);

	// If there is a chance that any PB write operations are incomplete, the CPU
	// should perform a read operation from any register on the PB bus before
	// executing the sleep instruction.
	AVR32_INTC.ipr[0];  // Dummy read

	// Go to STATIC sleep mode.
	SLEEP(AVR32_PM_SMODE_STATIC);

	while (true);
}
