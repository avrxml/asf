/**
 * \file
 *
 * \brief AVR XMEGA XCL example1
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
 * This example demonstrates how to use XMEGA E XCL module with LUT sub-module.
 * It performs a logic XOR operation between the 2 external input pins.
 *
 * \section files Main Files
 * - xcl_example1.c: the example application.
 * - conf_board.h: board configuration
 *
 * \section xcl_apiinfo drivers/XCL API
 * The XCL driver API can be found \ref xcl_group "here".
 *
 * \section deviceinfo Device Info
 * All AVR XMEGA E devices can be used. This example has been tested
 * with the following setup:
 *   - STK600 with ATxmega32E5 on STK600-RC032X
 *     LED0 pin should be connected to PD4 pin
 *     SW0 pin should be connected to PD2 pin
 *     SW1 pin should be connected to PD0 pin
 *   - XMEGA E5 Xplained evaluation kit
 *     No configuration required, SW0 and SW1 are already connected
 *     on PD0 and PD2, and LED0 on PD4.
 *
 * \section exampledescription Description
 * This example configures XCL LUT0 sub-module to perform a logic XOR operation
 * between its two inputs pins IN0/IN1 map on the low nible of PORTD
 * (respectively PD2, PD0). The LUTO output pin is mapped to PORTD pin 4.
 * As this XCL configuration is purely asynchronous, the example enters Power
 * Down sleep mode, but the XOR operation between the two pins is still
 * functionnal.
 *
 * As a result, LED0 will light when only one SW0 or SW1 push button is pressed.
 *
 * \image html xcl_example1.png
 *
 * \section stk600_setup Setup for STK600:
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

/*! \brief Main function.
 */
int main(void)
{
	/* Initialize the board.
	 * The board-specific conf_board.h file contains the configuration of
	 * the board initialization.
	 */
	board_init();
	sysclk_init();
	sleepmgr_init();

	/* Configure the XCL module:
	 * - Asynchronous mode usage (locks Power Down sleep mode)
	 * - Configure XCL to use XMEGA port D
	 * - Configure LUT in 2 separe LUTs with 2 differents inputs
	 * - LUT IN0 input on pin 2 or port D
	 * - LUT IN1 input on pin 0 or port D
	 * - LUT OUT0 output on pin 4 or port D
	 * - No time Delay for both LUT0 and LUT1
	 * - LUT0 performs XOR operation
	 */
	xcl_enable(XCL_ASYNCHRONOUS);
	xcl_port(PD);
	xcl_lut_type(LUT_2LUT2IN);
	xcl_lut_in0(LUT_IN_PINL);
	xcl_lut_in1(LUT_IN_PINL);
	xcl_lut0_output(LUT0_OUT_PIN4);
	xcl_lut_config_delay(DLY11, LUT_DLY_DISABLE, LUT_DLY_DISABLE);
	xcl_lut0_truth(XOR);

	/*
	 * main loop simply enters sleep mode
	 */
	while (true) {
		sleepmgr_enter_sleep();
	}
}
