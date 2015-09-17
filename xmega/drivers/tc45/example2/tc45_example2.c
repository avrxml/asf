/**
 * \file
 *
 * \brief AVR XMEGA TC45 Driver Example 2
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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
 *
 * \section intro Introduction
 * This simple example shows how to use the \ref tc45_group to generate
 * single slope PWM.
 *
 * The example uses and configures two TC modules.
 *  - TCD5 is configured to generate single slope PWM on LED0
 *  - TCC4 is used in generate a 50ms overflow interrupt
 *
 * Each time the 50ms overflow interrupt of TCC4 is executed
 * the TCD5 single slope PWM duty cycle is changed so that it
 * result visible on LEDO light intensity.
 *
 * \note
 * All AVR XMEGA E devices can be used.
 * The TC45 driver API can be found \ref tc45_group "here".
 *
 * Main files:
 *  - tc45.c Timer XMEGA Timer Counter driver implementation
 *  - tc45.h Timer XMEGA Timer Counter driver definitions
 *  - tc45_example2.c example application
 *
 * \section board_setup Board setup
 * For STK600 board:
 * - uses the RC032X routine board with TQFP32 socket
 * - PD4 must be connected to LED0
 *
 * For XMEGA-E5 Xplained board:
 * - plug USB for the power
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>

/**
 * \brief Timer Counter Overflow interrupt callback function
 *
 * This function is called when an overflow interrupt has occurred (50ms) on
 * TCD5 and change PWM duty single generation on TCD5 CCA.
 */
static void example_overflow_interrupt_callback(void)
{
	tc45_write_cc(&TCD5, TC45_CCA,
			tc45_read_cc(&TCD5, TC45_CCA) + 0xA00);
	tc45_clear_overflow(&TCC4);
}

int main(void)
{
	/* Usual initializations */
	board_init();
	sysclk_init();
	sleepmgr_init();
	irq_initialize_vectors();
	cpu_irq_enable();
        
	/*
	 * Configure TCC4 to generate 50ms overflow interrupt
	 * using 500kHz (2us) resolution clock (50ms = 25000 * 2us)
	 */
	/* Unmask clock for TCC4 */
	tc45_enable(&TCC4);
	/* Enable overflow interrupt */
	tc45_set_overflow_interrupt_level(&TCC4, TC45_INT_LVL_LO);
	/* Configure TC in normal mode */
	tc45_set_wgm(&TCC4, TC45_WG_NORMAL);
	/* Configure call back interrupt */
	tc45_set_overflow_interrupt_callback(&TCC4,
			example_overflow_interrupt_callback);
	/* Configure TC period and resolution */
	tc45_write_period(&TCC4, 25000);
	tc45_set_resolution(&TCC4, 500000);

	/*
	 * Configure TCD5 to generate 2ms Single Slope PWM
	 */
	/* Unmask clock for TCD5 */
	tc45_enable(&TCD5);
	/* Configure TC in PWM Single Slope PWM */
	tc45_set_wgm(&TCD5, TC45_WG_SS);
	/* Use the max period (2MHz / FFFFh = 30Hz) */
	tc45_write_period(&TCC4, 0xFFFF);
	/* Initialize and enable Channel A */
	tc45_write_cc(&TCD5, TC45_CCA, 0);
	tc45_enable_cc_channels(&TCD5, TC45_CCACOMP);
	/* Run TC at 2MHz clock resolution (CPU frequency 32MHz) */
	tc45_set_resolution(&TCD5, 20000000);

	do {
		/* Go to sleep, everything is handled by interrupts. */
		sleepmgr_enter_sleep();
	} while (1);
}
