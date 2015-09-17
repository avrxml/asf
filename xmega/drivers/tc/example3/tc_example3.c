/**
 * \file
 *
 * \brief AVR XMEGA TC Driver Example 3
 *
 * Copyright (C) 2010-2015 Atmel Corporation. All rights reserved.
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
 * This simple example shows how to use the \ref tc_group for generation of
 * pulsed width modulation signals.
 *
 * \section files Main files:
 * - tc.c timer/counter driver implementation
 * - tc.h timer/counter driver definitions
 * - tc_example3.c: tc driver example application
 * - conf_tc.h: configuration of the tc driver
 *
 * \section apiinfo tc driver API
 * The tc driver API can be found \ref tc_group "here".
 *
 * \section deviceinfo Device Info
 * All AVR XMEGA devices with tc and awex modules can be used.
 *
 * \section example_description Description of the example
 * The example will configure the Timer E0 (TCE0) in waveform generator mode.
 * AWex is used to output PWM signals from Compare modules CCA and CCB.
 * These complementary signals are output on Port E0 and E1. Deadtimes are
 * inserted. This example is dimming the duty-cycle of the two signals in an
 * opposite way.
 *
 * In addition the example configures the CCA and CCB interrupts to update the
 * duty-cycle
 *
 * Summary of Timer Counter configuration:
 *  - TCE0 : Timer
 *  - Compare/Capture modules A/B : Compare and waveform generation in Dual
 *  slope mode
 *  - AXexE : Deadtime Low and High insertion and override of Port E0/E1
 *
 * This example uses the on-board LEDs to provide user feedback, the output
 * from the LEDs are as follows:
 *  - LED0: fades out
 *  - LED1: fades in

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
#include <conf_example.h>
#include <string.h>
#include <asf.h>

uint16_t cca_pwm_index;
uint16_t ccb_pwm_index;

/**
 * \brief Compare/Capture detection interrupt callback function
 *
 * This function is called when an interrupt has occurred on a Compare A channel.
 * It increments the CC detection level and thus modify the duty-cycle.
 */
static void example_cca_interrupt_callback(void)
{
	cca_pwm_index += 0x01;
	if (cca_pwm_index >= 0xFE) cca_pwm_index=0;
	tc_write_cc_buffer(&TIMER_EXAMPLE, TC_CCA, cca_pwm_index);
}

/**
 * \brief Compare/Capture detection interrupt callback function
 *
 * This function is called when an interrupt has occurred on a Compare B channel.
 * It decrements the CC detection level and thus modify the duty-cycle.
 */
static void example_ccb_interrupt_callback(void)
{
	ccb_pwm_index -= 0x01;
	if (ccb_pwm_index <= 0x01) ccb_pwm_index=0xFF;
	tc_write_cc_buffer(&TIMER_EXAMPLE, TC_CCB, ccb_pwm_index);
}

int main(void)
{
	pmic_init();
	board_init();
	sysclk_init();
	sleepmgr_init();
	cpu_irq_enable();

	/* Enables the Timer defined in conf_example.h : TCE0 in this example */
	tc_enable(&TIMER_EXAMPLE);

	/* Configures the interrupt level of CCA and CCB modules : low */
	tc_set_cca_interrupt_level(&TIMER_EXAMPLE, TC_INT_LVL_LO);
	tc_set_ccb_interrupt_level(&TIMER_EXAMPLE, TC_INT_LVL_LO);

	/* Configures the waveform generator of this Timer mode in NORMAL mode */
	tc_set_wgm(&TIMER_EXAMPLE, TC_WG_NORMAL);

	/* Declares the interrupt functions which will be called when CCA and CCB
	interrupts will occur */
	tc_set_cca_interrupt_callback(&TIMER_EXAMPLE,
			example_cca_interrupt_callback);
	tc_set_ccb_interrupt_callback(&TIMER_EXAMPLE,
			example_ccb_interrupt_callback);

	/* Configures the Timer period*/
	tc_write_period(&TIMER_EXAMPLE, TIMER_EXAMPLE_PERIOD);

	/* Configures the CCA and CCB levels*/
	tc_write_cc(&TIMER_EXAMPLE, TC_CCA, TIMER_EXAMPLE_PERIOD/2);
	tc_write_cc(&TIMER_EXAMPLE, TC_CCB, TIMER_EXAMPLE_PERIOD/2);

	/* Enables the CCA and CCB channels*/
	tc_enable_cc_channels(&TIMER_EXAMPLE,TC_CCAEN);
	tc_enable_cc_channels(&TIMER_EXAMPLE,TC_CCAEN);

	/* Configures the waveform genertaor in Dual Slope mode and Top*/
	tc_set_wgm(&TIMER_EXAMPLE,TC_WG_DS_T);

	/* Enables and configures the deadtime of CCA and CCB outputs*/
	tc_awex_enable_cca_deadtime(&AWEXE);
	tc_awex_enable_ccb_deadtime(&AWEXE);
	tc_awex_set_dti_high(&AWEXE, TIMER_EXAMPLE_PERIOD/6);
	tc_awex_set_dti_low(&AWEXE, TIMER_EXAMPLE_PERIOD/6);

	/* Outputs CCA and CCB on Port E0 and E1*/
	tc_awex_set_output_override(&AWEXE, 0x03);

	tc_set_resolution(&TIMER_EXAMPLE, 10000);

	do {
		/* Go to sleep, everything is handled by interrupts. */
		sleepmgr_enter_sleep();
	} while (1);
}
