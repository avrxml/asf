/**
 * \file
 *
 * \brief AVR XMEGA TC45 Driver Example 3
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
 * <A href="http://www.microchip.com/">Microchip</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
