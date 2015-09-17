/**
 * \file
 *
 * \brief Example of an interrupt-driven USART communication.
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

/**
 * \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the interrupt controller driver. It also
 * comes bundled with a basic application as an example of usage.
 *
 * This example demonstrates how to use the INTC driver for the USART RXRDY
 * interrupt.
 *
 * Operating mode: each character typed on the PC terminal is echoed back.
 *
 * \note The IAR built-in event handling mechanism manages interrupt groups but
 * not lines. This example shows how to register an interrupt through the INTC
 * driver, which manages lines. If interrupt request lines do not have to be
 * handled separately within groups, then the IAR built-in event handling
 * mechanism can be used in this way:
 *   - remove the INTC driver files (intc.c and exception.s82) from the IAR
 *     project;
 *   - remove calls to the INTC driver functions (\ref INTC_init_interrupts and
 *     \ref INTC_register_interrupt);
 *   - add <tt>\#pragma handler = \e AVR32_xxxx_IRQ_GROUP, \e int_level</tt>
 *     above the \c __interrupt keyword;
 *   - the IAR project options can be changed to catch unhandled events under
 *     the \e Runtime tab of the <i>General Options</i> item.
 *
 * \warning The IAR built-in event handling mechanism
 *          (<tt>\#pragma exception</tt> and <tt>\#pragma handler</tt>) MUST NOT
 *          be used along with the INTC driver within a project.
 *
 * \section files Main Files
 *   - intc.c: interrupt controller driver;
 *   - intc.h: interrupt controller driver header file;
 *   - exception.S: exception and interrupt handlers assembly file for GCC;
 *   - interrupt_usart_example.c: interrupt controller example application.
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with an INTC and a USART module can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 *   - EVK1100, EVK1101, UC3C-EK, EVK1104, EVK1105, AT32UC3L-EK evaluation kits;
 *     STK600+RCUC3L routing card
 *   - CPU clock:
 *        -- 12 MHz : EVK1100, EVK1101, EVK1104, EVK1105, AT32UC3L-EK evaluation
 *                    kits; STK600+RCUC3L routing card; STK600+RCUCD routing
 *                    card;
 *        -- 16 MHz : UC3C_EK
 *   - USART0 (on EVK1100) or USART1 (on EVK1101 or STK1000 or STK600+RCUCD )
 *     connected to a PC serial port via a standard RS232 DB9 cable, or USART0
 *     (on EVK1105) or USART1 (on EVK1104) or USART2 (on AT32UC3C-EK) or USART3
 *     (on AT32UC3L-EK) abstracted with a USB CDC connection to a PC;
 *   - PC terminal settings:
 *     - 57600 bps (on EVK1100, EVK1101, AT32UC3C-EK, EVK1104, EVK1105,
         1AT32UC3L-EK, STK600+RCUCD) or 115200 bps (on STK1000),
 *     - 8 data bits,
 *     - no parity bit,
 *     - 1 stop bit,
 *     - no flow control.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "print_funcs.h"
#include "intc.h"
#if defined (__GNUC__)
#  if   defined (__AVR32_AP7000__)
#    include "pm_at32ap7000.h"
#  else
#    include "power_clocks_lib.h"
#  endif
#elif defined (__ICCAVR32__) || defined (__AAVR32__)
#  if  defined (__AT32AP7000__)
#    include "pm_at32ap7000.h"
#  else
#    include "power_clocks_lib.h"
#  endif
#endif
#include "gpio.h"
#include "usart.h"
#include "conf_example.h"



#if BOARD == STK1000
#  define EXAMPLE_USART                 (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART1_RXD_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART1_RXD_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN          AVR32_USART1_TXD_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART1_TXD_0_FUNCTION
#  define EXAMPLE_USART_IRQ             AVR32_USART1_IRQ
#  define EXAMPLE_USART_BAUDRATE        115200
// PBA clock target frequency, in Hz
#  define EXAMPLE_TARGET_PBACLK_FREQ_HZ FOSC0
#elif BOARD == NGW100
#  define EXAMPLE_USART                 (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART1_RXD_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART1_RXD_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN          AVR32_USART1_TXD_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART1_TXD_0_FUNCTION
#  define EXAMPLE_USART_IRQ             AVR32_USART1_IRQ
#  define EXAMPLE_USART_BAUDRATE        115200
// PBA clock target frequency, in Hz
#  define EXAMPLE_TARGET_PBACLK_FREQ_HZ FOSC0
#endif



/**
 * \brief The USART interrupt handler.
 *
 * \note The `__attribute__((__interrupt__))' (under GNU GCC for AVR32) and
 *       `__interrupt' (under IAR Embedded Workbench for Atmel AVR32) C function
 *       attributes are used to manage the `rete' instruction.
 */
#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined(__ICCAVR32__)
__interrupt
#endif
static void usart_int_handler(void)
{
	int c;

	/*
	 * In the code line below, the interrupt priority level does not need to
	 * be explicitly masked as it is already because we are within the
	 * interrupt handler.
	 * The USART Rx interrupt flag is cleared by side effect when reading
	 * the received character.
	 * Waiting until the interrupt has actually been cleared is here useless
	 * as the call to usart_write_char will take enough time for this before
	 * the interrupt handler is left and the interrupt priority level is
	 * unmasked by the CPU.
	 */
	usart_read_char(EXAMPLE_USART, &c);

	// Print the received character to USART. This is a simple echo.
	usart_write_char(EXAMPLE_USART, c);
}


/**
 * \brief The main function.
 *
 * It sets up the USART module on EXAMPLE_USART. The terminal settings are 57600
 * 8N1.
 * Then it sets up the interrupt handler and waits for a USART interrupt to
 * trigger.
 */
int main(void)
{
	static const gpio_map_t USART_GPIO_MAP =
	{
		{EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
		{EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION}
	};

	// USART options.
	static const usart_options_t USART_OPTIONS =
	{
		.baudrate     = EXAMPLE_USART_BAUDRATE,
		.charlength   = 8,
		.paritytype   = USART_NO_PARITY,
		.stopbits     = USART_1_STOPBIT,
		.channelmode  = USART_NORMAL_CHMODE
	};

#if BOARD == EVK1100 || BOARD == EVK1101 || BOARD == UC3C_EK \
	|| BOARD == EVK1104 || BOARD == EVK1105 || BOARD == STK600_RCUC3L0 \
	|| BOARD == STK600_RCUC3D
	/*
	 * Configure Osc0 in crystal mode (i.e. use of an external crystal
	 * source, with frequency FOSC0) with an appropriate startup time then
	 * switch the main clock source to Osc0.
	 */
	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

#elif BOARD == STK1000
	pm_reset();
#elif BOARD == UC3L_EK
	/*
	 * Note: on the AT32UC3L-EK board, there is no crystal/external clock
	 * connected to the OSC0 pinout XIN0/XOUT0. We shall then program the
	 * DFLL and switch the main clock source to the DFLL.
	 */
	pcl_configure_clocks(&pcl_dfll_freq_param);
	/*
	 * Note: since it is dynamically computing the appropriate field values
	 * of the configuration registers from the parameters structure, this
	 * function is not optimal in terms of code size. For a code size
	 * optimal solution, it is better to create a new function from
	 * pcl_configure_clocks_dfll0() and modify it to use preprocessor
	 * computation from pre-defined target frequencies.
	 */
#endif

	// Assign GPIO to USART.
	gpio_enable_module(USART_GPIO_MAP,
		sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

	// Initialize USART in RS232 mode.
	usart_init_rs232(EXAMPLE_USART, &USART_OPTIONS,
		EXAMPLE_TARGET_PBACLK_FREQ_HZ);
	print(EXAMPLE_USART, ".: Using interrupts with the USART :.\r\n\r\n");

	// Disable all interrupts.
	Disable_global_interrupt();

	// Initialize interrupt vectors.
	INTC_init_interrupts();

	/*
	 * Register the USART interrupt handler to the interrupt controller.
	 * usart_int_handler is the interrupt handler to register.
	 * EXAMPLE_USART_IRQ is the IRQ of the interrupt handler to register.
	 * AVR32_INTC_INT0 is the interrupt priority level to assign to the
	 * group of this IRQ.
	 */
	INTC_register_interrupt(&usart_int_handler, EXAMPLE_USART_IRQ,
		AVR32_INTC_INT0);

	// Enable USART Rx interrupt.
	EXAMPLE_USART->ier = AVR32_USART_IER_RXRDY_MASK;
	print(EXAMPLE_USART, "Type a character to use the interrupt handler."
		"\r\nIt will show up on your screen.\r\n\r\n");

	// Enable all interrupts.
	Enable_global_interrupt();

	/**
	 * We have nothing left to do in the main, so we may switch to a device
	 * sleep mode: we just need to be sure that the USART module will be
	 * still be active in the chosen sleep mode. The sleep mode to use is
	 * the FROZEN sleep mode: in this mode the PB clocks are still active
	 * (so the USART module which is on the Peripheral Bus will still be
	 * active while the CPU and HSB will be stopped).
	 * --
	 * Modules communicating with external circuits should normally be
	 * disabled before entering a sleep mode that will stop the module
	 * operation: this is not the case for the FROZEN sleep mode.
	 * --
	 * When the USART interrupt occurs, this will wake the CPU up which will
	 * then execute the interrupt handler code then come back to the
	 * while(1) loop below to execute the sleep instruction again.
	 */

	while(1)
	{
		/*
		 * If there is a chance that any PB write operations are
		 * incomplete, the CPU should perform a read operation from any
		 * register on the PB bus before executing the sleep
		 * instruction.
		 */
		AVR32_INTC.ipr[0];  // Dummy read

		// Go to FROZEN sleep mode.
		SLEEP(AVR32_PM_SMODE_FROZEN);
		/*
		 * When the device wakes up due to an interrupt, once the
		 * interrupt has been serviced, go back into FROZEN sleep mode.
		 */
	}
}
