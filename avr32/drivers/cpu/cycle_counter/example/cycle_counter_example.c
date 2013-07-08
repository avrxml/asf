/*****************************************************************************
 *
 * \file
 *
 * \brief COUNT & COMPARE usage example.
 *
 * Example of COUNT & COMPARE registers usage, using the USART software driver
 * (for printing ASCII msgs), the GPIO software driver (to map the USART on I/O pins),
 * the INTC software driver (for interrupt management).
 *
 * Copyright (c) 2009 - 2011 Atmel Corporation. All rights reserved.
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
 * This documents gives an example of the usage of the CPU Cycle counter. The cycle counter is a COUNT register,
 * that increments once every clock. The count register can be used together with
 * the COMPARE register to create a timer with interrupt functionality.
 * The COMPARE register holds a value that the COUNT register is compared against.
 * When the COMPARE and COUNT registers match, a compare interrupt request is
 * generated and COUNT is reset to 0.
 *
 * \section example-description Example's Operating Mode
 * This example shows how to use the COUNT register together with the COMPARE register
 * to generate an interrupt periodically. Here is the operating mode of the example:
 * - At the beginning of the code, we check that initial default values of the COUNT
 * and COMPARE registers are correct.
 * - Then, the COUNT & COMPARE interrupt mechanism is tested with a short delay. Messages
 * are displayed on USART1. This delay is equal to (1/fCPU)*NB_CLOCK_CYCLE_DELAY_SHORT
 * in case NB_CLOCK_CYCLE_DELAY_SHORT value is reloaded (83.3ms) or (1/fCPU)*NB_CLOCK_CYCLE_DELAY_LONG
 * in case NB_CLOCK_CYCLE_DELAY_LONG value is reloaded (1.67s)
 * - Then the program infinitely loops, using the COUNT & COMPARE interrupt mechanism
 * with a longer delay. Messages are displayed on USART1 and one of Led 1 through Led4
 * is on upon each COUNT & COMPARE match (Led1 -> Led2 -> Led3 -> Led4 -> Led1 ...and so on).
 *
  \section files Main Files
 * - cycle_count_example.c : cycle counter example
 * - cycle_counter.h: cycle counter driver interface
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - EVK1100, EVK1101, EVK1104, EVK1105, AT32UC3C-EK or AT32UC3L-EK evaluation kits; STK600+RCUC3L routing card;
 *    STK600+RCUCD routing card;
 * - CPU clock:
 *             -- EVK1100, EVK1101, EVK1104, EVK1105, AT32UC3L-EK, STK600+RCUC3L :  12 MHz;
 *             -- AT32UC3C-EK : 16 MHz
 * - Serial Link :
 *      -- USART1 (on EVK1100 or EVK1101) connected to a PC serial port via a standard
 *         RS232 DB9 cable, or USART0 (on EVK1105) or USART1 (on EVK1104)
 *      -- USART2 (on AT32UC3C-EK) abstracted with a USB CDC connection to a PC
 *      -- USART3 (on AT32UC3L-EK) abstracted with a USB CDC connection to a PC;
 *      -- STK600 usart port for the STK600+RCUC3L setup (connect STK600.PE2 to
 *         STK600.RS232 SPARE.TXD and STK600.PE3 to STK600.RS232 SPARE.RXD)
 *      -- STK600 usart port for the STK600+RCUC3D setup (connect STK600.PC7 to
 *         STK600.RS232 SPARE.TXD and STK600.PD0 to STK600.RS232 SPARE.RXD)
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


#if defined (__GNUC__)
#  include "intc.h"
#endif

#define _ASSERT_ENABLE_

#include "compiler.h"
#include "print_funcs.h"
#include "board.h"
#include "cycle_counter.h"
#include "power_clocks_lib.h"

#if BOARD == UC3L_EK
#  define EXAMPLE_TARGET_DFLL_FREQ_HZ   96000000  // DFLL target frequency, in Hz
#  define EXAMPLE_TARGET_MCUCLK_FREQ_HZ 12000000  // MCU clock target frequency, in Hz
#  define EXAMPLE_TARGET_PBACLK_FREQ_HZ 12000000  // PBA clock target frequency, in Hz
#endif

#define NB_CLOCK_CYCLE_DELAY_SHORT    1000000   // 83.3 ms if fCPU==12MHz
#define NB_CLOCK_CYCLE_DELAY_LONG    20000000   // 1.67 s if fCPU==12MHz

// Counter of COUNT/COMPARE matches.
static volatile uint32_t total_compare_irqs = 0;

// COUNT/COMPARE match interrupt handler and main function synchronizer.
static volatile bool compare_isr_fired = true;

// COUNT/COMPARE match interrupt handler
#if defined(__GNUC__)
// GCC-specific syntax to declare an interrupt handler. The interrupt handler
// registration is done in the main function using the INTC software driver module.
__attribute__((__interrupt__))
#elif defined(__ICCAVR32__)
// IAR-specific syntax to declare and register an interrupt handler.
// Register to the interrupt group 0(cf Section "Interrupt Request Signal Map"
// in the datasheet) with interrupt level 0.
#if ((defined(__AT32UC3L016__)  \
    || defined(__AT32UC3L032__) \
    || defined(__AT32UC3L064__) \
    || UC3D                     \
    || UC3C ))
#pragma handler = AVR32_CORE_IRQ_GROUP0, 0
#else
#pragma handler = AVR32_CORE_IRQ_GROUP, 0
#endif
__interrupt
#endif
static void compare_irq_handler(void)
{
	// Count the number of times this IRQ handler is called.
	total_compare_irqs++;

	// Inform the main program that it may display a msg saying
	// that the COUNT&COMPARE interrupt occurred.
	compare_isr_fired = true;

	// Clear the pending interrupt(writing a value to the COMPARE register clears
	// any pending compare interrupt requests). Schedule the COUNT&COMPARE match
	// interrupt to happen every NB_CLOCK_CYCLE_DELAY_LONG cycles.
	Set_sys_compare(NB_CLOCK_CYCLE_DELAY_LONG);
}


#if BOARD == UC3L_EK
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
#endif


/* Main function */
int main(void)
{
	uint32_t target_compare_val;
	uint32_t current_compare_val;
	uint32_t current_sys_clock_count;
	uint8_t  active_led_map = 0x01;

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

	// Init DEBUG module
	init_dbg_rs232(EXAMPLE_TARGET_PBACLK_FREQ_HZ);
#else
	// Configure Osc0 in crystal mode (i.e. use of an external crystal source, with
	// frequency FOSC0) with an appropriate startup time then switch the main clock
	// source to Osc0.
	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);
	// Init DEBUG module
	init_dbg_rs232(FOSC0);
#endif

	print_dbg("---------------------------------------------\r\n");

	// Read COMPARE register.
	// NOTE: it should be equal to 0 (default value upon reset) => The compare
	// and exception generation feature is thus currently disabled.
	current_compare_val = Get_sys_compare();
	Assert(!current_compare_val);

	// Read COUNT register.
	// NOTE: the COUNT register increments since reset => it should be != 0.
	current_sys_clock_count = Get_sys_count();
	Assert(current_sys_clock_count);

#if defined (__GNUC__)
	// Disable all interrupts.
	Disable_global_interrupt();

	INTC_init_interrupts();

	// Register the compare interrupt handler to the interrupt controller.
	// compare_irq_handler is the interrupt handler to register.
	// AVR32_CORE_COMPARE_IRQ is the IRQ of the interrupt handler to register.
	// AVR32_INTC_INT0 is the interrupt priority level to assign to the group of this IRQ.
	// void INTC_register_interrupt(__int_handler handler, unsigned int irq, unsigned int int_level);
	INTC_register_interrupt(&compare_irq_handler, AVR32_CORE_COMPARE_IRQ, AVR32_INTC_INT0);
#endif
	// Enable all interrupts.
	Enable_global_interrupt();

	// Schedule the COUNT&COMPARE match interrupt in NB_CLOCK_CYCLE_DELAY_SHORT
	// clock cycles from now.
	current_sys_clock_count = Get_sys_count();
	target_compare_val = current_sys_clock_count + NB_CLOCK_CYCLE_DELAY_SHORT; // WARNING: MUST FIT IN 32bits.

	// Start compare interrupt. If target_compare_val ends up to be 0, make it 1 so that the COMPARE
	// and exception generation feature does not get disabled.
	Set_sys_compare(!target_compare_val ? 1 : target_compare_val);

	// Check if the previous write in the COMPARE register succeeded.
	current_compare_val = Get_sys_compare();
	Assert(target_compare_val == current_compare_val);

	//  The previous COMPARE write succeeded.
	// Loop until the COUNT&COMPARE match triggers.
	while (!total_compare_irqs) {
		current_sys_clock_count = Get_sys_count();

		if (current_sys_clock_count < target_compare_val)
			print_dbg("Count has not net reached compare value\r\n");
		else if (current_sys_clock_count > target_compare_val)
			// This should never happen if COMPARE is not zero.
			print_dbg("Count is greater than compare value\r\n");
		else
			print_dbg("Count is equal to compare value\r\n");
		// NOTE: since the COUNT register is reset to zero upon COUNT/COMPARE match,
		// the printed messages here are not "accurate".
	}

	while (true) {
		if (compare_isr_fired) {
			// Indicate that the ISR trigger flag has been read
			compare_isr_fired = false;

			// Turn the current LED on only and move to next LED.
			LED_Display_Field(LED0 | LED1 | LED2 | LED3, active_led_map);
			active_led_map = max((active_led_map << 1) & 0x0F, 0x01);

			// Print some info on the debug port.
			print_dbg("\r\nCompare interrupt triggered: #");
			print_dbg_ulong(total_compare_irqs);
		}
	}
}
