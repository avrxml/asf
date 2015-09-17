/**
 * \file
 *
 * \brief AVR XMEGA TC45 Driver Example 3
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
 * This simple example shows how to use the \ref tc45_group for generation of
 * pulsed width modulation signals.
 *
 * The example configures the Timer C4 (TCC4) in waveform generator mode.
 * Waveform Extension (WeX) feature is used to output PWM signals from Compare
 * modules CCA and CCB. These complementary signals are output on Port C0/C1
 * and C2/C3. Deadtimes are inserted.
 *
 * This example is dimming the duty-cycle of the two signals in an opposite way.
 * In addition the example configures the CCA and CCB interrupts to update the
 * duty-cycle.
 *
 * Summary of Timer Counter configuration:
 *  - TCC4, Timer
 *  - Compare/Capture modules A/B, Compare and waveform generation in Dual
 *  slope mode
 *  - WeX, Deadtime Low and High insertion and override of Port C0/C1 and C2/C3.
 *
 * This example uses the on-board LEDs to provide user feedback, the output
 * from the LEDs are as follows:
 *  - LED0: fades out
 *  - LED1: fades in
 *  - LED2: fades in
 *  - LED3: fades out
 *
 * \note
 * All AVR XMEGA E devices can be used.
 * The TC45 driver API can be found \ref tc45_group "here".
 *
 * Main files:
 * - tc45.c timer/counter driver implementation
 * - tc45.h timer/counter driver definitions
 * - tc45_example3.c: tc driver example application
 *
 * \section board_setup Board setup
 * For STK600 board:
 * - uses the RC032X routine board with TQFP32 socket
 * - PC0 must be connected to LED0
 * - PC1 must be connected to LED1
 * - PC2 must be connected to LED2
 * - PC3 must be connected to LED3
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

/* Timer TCC4 resolution (Hz)
 * Note: This configure a TC prescaler equal to 32 = SYSCLK 32MHz / 10MHz
 */
#define TIMER_TCC4_RESOLUTION  10000000

/* Timer TCC4 periode
 * Note: 320Hz = SYSCLK 10MHz / 31250
 */
#define TIMER_TCC4_PERIOD      31250


/**
 * \brief Compare/Capture detection interrupt callback function
 *
 * This function is called when an interrupt has occurred on a Compare A
 * channel.
 * It increments the CC detection level and thus modify the duty-cycle.
 */
static void example_cca_interrupt_callback(void)
{
	static uint16_t cca_pwm_index = (TIMER_TCC4_PERIOD / 2);
	cca_pwm_index += 0x10;
	if (cca_pwm_index >= (TIMER_TCC4_PERIOD - 17)) {
		cca_pwm_index = 17;
	}

	tc45_write_cc_buffer(&TCC4, TC45_CCA, cca_pwm_index);
}

/**
 * \brief Compare/Capture detection interrupt callback function
 *
 * This function is called when an interrupt has occurred on a Compare B
 * channel.
 * It decrements the CC detection level and thus modify the duty-cycle.
 */
static void example_ccb_interrupt_callback(void)
{
	static uint16_t ccb_pwm_index = (TIMER_TCC4_PERIOD / 2);
	ccb_pwm_index -= 0x10;
	if (ccb_pwm_index <= 17) {
		ccb_pwm_index = (TIMER_TCC4_PERIOD - 17);
	}

	tc45_write_cc_buffer(&TCC4, TC45_CCB, ccb_pwm_index);
}

int main(void)
{
	/* Usual initializations */
	board_init();
	sysclk_init();
	sleepmgr_init();
	irq_initialize_vectors();
	cpu_irq_enable();

	/* Enables the Timer */
	tc45_enable(&TCC4);

	/* Configures the interrupt level of CCA and CCB modules : low */
	tc45_set_cca_interrupt_level(&TCC4, TC45_INT_LVL_LO);
	tc45_set_ccb_interrupt_level(&TCC4, TC45_INT_LVL_LO);

	/* Configures the waveform generator of this Timer mode in NORMAL mode */
	tc45_set_wgm(&TCC4, TC45_WG_NORMAL);

	/* Declares the interrupt functions which will be called when CCA and
	 * CCB interrupts will occur */
	tc45_set_cca_interrupt_callback(&TCC4,
			example_cca_interrupt_callback);
	tc45_set_ccb_interrupt_callback(&TCC4,
			example_ccb_interrupt_callback);

	/* Configures the Timer period */
	tc45_write_period(&TCC4, TIMER_TCC4_PERIOD);

	/* Configures the CCA and CCB levels */
	tc45_write_cc(&TCC4, TC45_CCA, TIMER_TCC4_PERIOD / 2);
	tc45_write_cc(&TCC4, TC45_CCB, TIMER_TCC4_PERIOD / 2);

	/* Enables the CCA and CCB channels */
	tc45_enable_cc_channels(&TCC4, TC45_CCACOMP);
	tc45_enable_cc_channels(&TCC4, TC45_CCBCOMP);

	/* Configures the waveform generator in Dual Slope mode and Top */
	tc45_set_wgm(&TCC4, TC45_WG_DS_T);

	/* Enables and configures the deadtime of CCA and CCB outputs */
	tc45_wex_set_dti_high(&WEXC, TIMER_TCC4_PERIOD / 6);
	tc45_wex_set_dti_low(&WEXC, TIMER_TCC4_PERIOD / 6);
	tc45_wex_enable_cca_deadtime(&WEXC);
	tc45_wex_enable_ccb_deadtime(&WEXC);

	/* Configure resolution and start Timer */
	tc45_set_resolution(&TCC4, TIMER_TCC4_RESOLUTION);

	do {
		/* Go to sleep, everything is handled by interrupts. */
		sleepmgr_enter_sleep();
	} while (1);
}
