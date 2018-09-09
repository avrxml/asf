/**
 * \file
 *
 * \brief AVR XMEGA XCL example2
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
 * <A href="http://www.microchip.com/design-centers/8-bit">Atmel AVR</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
