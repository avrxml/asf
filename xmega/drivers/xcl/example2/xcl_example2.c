/**
 * \file
 *
 * \brief AVR XMEGA XCL example2
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

/*! \mainpage
 * \section intro Introduction
 * This example demonstrates how to use XMEGA E XCL module with TC.
 *
 * \section files Main Files
 * - xcl_example2.c: the example application.
 * - conf_board.h: board configuration
 * - conf_xcl.h: XCL driver configuration (to enable ISR handling)
 *
 * \section xcl_apiinfo drivers/XCL API
 * The XCL driver API can be found \ref xcl_group "here".
 *
 * \section deviceinfo Device Info
 * All AVR XMEGA E devices can be used. This example has been tested
 * with the following setup:
 *   - STK600 with ATxmega32E5 on STK600-RC032X
 *     LED0 pin should be connected to PA0 pin
 *   - XMEGA E5 Xplained evaluation kit
 *
 * \section exampledescription Description of the example
 * This example will configure the XCL Timer sub-module in a 16bit timer with
 * its underflow interrupt to toggle a LED at 4Hz.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/avr">Atmel AVR</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>

#define OUTPUT_FREQ 4

static void example_xcl_tc16_interrupt_callback(void);

/**
 * \brief XCL TC16 underflow interupt call back
 *
 * This function is called when an underflow interrupt occues on XCL TC16
 * module.
 * This function toggles and LED and clears the realted interrupt flag.
 *
 */
static void example_xcl_tc16_interrupt_callback()
{
	ioport_toggle_pin_level(LED0_GPIO);
	xcl_tc16_clear_underflow_interrupt();
}

/*! \brief Main function.
 */
int main(void)
{
	volatile uint32_t real_resolution;

	/* Initialize the board.
	 * The board-specific conf_board.h file contains the configuration of
	 * the board initialization.
	 */
	board_init();
	sysclk_init();
	sleepmgr_init();
	irq_initialize_vectors();
	cpu_irq_enable();

	/* Configure the XCL module:
	 * - Synchronous mode usage (locks idle sleep mode)
	 * - Configure timer in 16bits mode
	 * - Configure Timer period to underflow at 4Hz
	 * - Enable underflow interupt and map user callback to the underflow
	 *interrupt
	 */
	xcl_enable(XCL_SYNCHRONOUS);
	xcl_port(PD);
	xcl_tc_type(TC16);
	xcl_set_unf_interrupt_callback(example_xcl_tc16_interrupt_callback);
	xcl_tc_set_underflow_interrupt_level(XCL_INT_LVL_LO);
	real_resolution = xcl_tc_set_resolution(7800);
	xcl_tc16_set_period(real_resolution / OUTPUT_FREQ);
	xcl_tc16_set_count(real_resolution / OUTPUT_FREQ);
	xcl_tc16_enable_underflow_interrupt();

	/*
	 * main loop simply enters sleep mode
	 */
	while (true) {
		sleepmgr_enter_sleep();
	}
}
