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
 * This example demonstrates how to use XMEGA E XCL module with both LUT and TC.
 *
 * \section files Main Files
 * - xcl_example3.c: the example application.
 * - conf_board.h: board configuration
 *
 * \section xcl_apiinfo drivers/XCL API
 * The XCL driver API can be found \ref xcl_group "here".
 *
 * \section deviceinfo Device Info
 * All AVR XMEGA E devices can be used. This example has been tested
 * with the following setup:
 *   - STK600 with ATxmega32E5 on STK600-RC032X.
 *     LED0 pin should be connected to PD2 pin
 *     LED1 pin should be connected to PD3 pin
 *     LED2 pin should be connected to PD4 pin
 *
 * \section exampledescription Description of the example
 * This examples demonstrates how to use both Timer and LUT sub-modules together
 * within the XCL module. The purpose of this configuration is to generate
 * a waveform output that is the logic XOR of two generated PWM waveforms.
 *
 * This example configures XCL TC sub-module in two 8bits BTCO/1 timers to
 * generate single slope PWM output on their compare output pins (PD2 and PD3).
 * Those PWM output are internaly connected to LUT0/1 inputs configured as a
 * logic XOR function. The resulting waveform is connected to PD4 LUT output.
 *
 * \image html xcl_example3.png
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
	 * - Synchronous mode usage (locks Idle sleep mode)
	 * - Configure XCL to use XMEGA port D
	 */
	xcl_enable(XCL_SYNCHRONOUS);
	xcl_port(PD);

	/* Configure the XCL LUT sub-module:
	 * - Configure LUT in 1 LUTs with 3 differents inputs
	 * - LUT IN0 input on pin 2 of port D
	 * - LUT IN1 input on XCL LUT OUT1
	 * - LUT OUT0 output on pin 4 of port D
	 * - LUT IN2 input on XCL LUT OUT0 (but ignored)
	 * - LUT IN3 input on pin 3 of port D
	 * - LUT OUT1 output keeps internal
	 * - No time Delay for both LUT0 and LUT1
	 * - LUT0 performs XOR operation
	 * - LUT1 copies IN3 input ignoring IN2
	 */
	xcl_lut_type(LUT_1LUT3IN);
	xcl_lut_in0(LUT_IN_PINL);
	xcl_lut_in1(LUT_IN_XCL);
	xcl_lut_in2(LUT_IN_XCL);
	xcl_lut_in3(LUT_IN_PINL);
	xcl_lut0_output(LUT0_OUT_PIN4);
	xcl_lut_config_delay(DLY11, LUT_DLY_DISABLE, LUT_DLY_DISABLE);
	xcl_lut0_truth(XOR);
	xcl_lut1_truth(IN3);

	/* Configure the XCL Timer sub-module:
	 * - Configure timer in 2 separate 8bits BTCO/1 timers mode
	 * - Configure Timers in Single Slope PWM mode
	 * - Enable both CC0 and CC1 pins outputs
	 */
	xcl_tc_type(BTC01);
	xcl_tc_mode(PWM);
	xcl_enable_oc0();
	xcl_enable_oc1();
	xcl_btc0_set_compare(0x55);
	xcl_btc1_set_compare(0xAA);
	/* A resolution of 60 is selected to obtain a slow blink
	 * which will be visible for user
	 */
	real_resolution = xcl_tc_set_resolution(60);

	/*
	 * main loop simply enters sleep mode
	 */
	while (true) {
		sleepmgr_enter_sleep();
	}
}
