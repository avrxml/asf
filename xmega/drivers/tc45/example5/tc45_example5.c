/**
 * \file
 *
 * \brief AVR XMEGA TC45 Driver Example 5
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
 * pulsed width modulation signals and to switch them on different outputs
 * with the use of Output Matrix function of WeX.
 *
 * The example uses the Timer TCC4 and Timer TCC5:
 * - Enable compare and waveform generation in Dual slope mode
 * - Timer TCC5 2 times faster than TCC4
 * - Enable Compare/Capture modules A/B
 * - WeX is used to output PWM signals from CCA and CCB.
 *   CCA and CCB control the duty-cycle.
 * - CCA and CCB interrupts update the CCA and CCB values which dimming the
 *   duty-cycle of the two signals in an opposite way.
  *
 * The LEDs are connected on Timer PWM output signal and give:
 * - TCC4 CCA : LED fades out
 * - TCC4 CCB : LED fades in
 * - TCC5 CCA : LED fades out two times faster than TCC4 CCA LED
 * - TCC5 CCB : LED fades in two times faster than TCC5 CCB LED
 *
 * The Output Matrix (OTMX) of WeX allows to change output pin of signals.
 * The switch SW0 (PORTD pin 0) is used to change OTMX mode value,
 * and give the following results:
 * - In OTMX 0 mode (at startup):
 *   - PC0 = TCC4 CCA = LED fades out
 *   - PC1 = TCC4 CCB = LED fades in
 *   - PC2 = None
 *   - PC3 = None
 *   - PC4 = TCC5 CCA = LED fades out faster
 *   - PC5 = TCC5 CCB = LED fades in faster
 *   - PC6 = None
 *   - PC7 = None
 * - In OTMX 1 mode:
 *   - PC0 = TCC4 CCA = LED fades out
 *   - PC1 = TCC4 CCB = LED fades in
 *   - PC2 = None
 *   - PC3 = None
 *   - PC4 = TCC5 CCA = LED fades out faster
 *   - PC5 = TCC5 CCB = LED fades in faster
 *   - PC6 = TCC5 CCA = LED fades out faster
 *   - PC7 = TCC5 CCB = LED fades in faster
 * - In OTMX 2 mode:
 *   - PC0 = TCC4 CCA = LED fades out
 *   - PC1 = TCC4 CCB = LED fades in
 *   - PC2 = TCC5 CCA = LED fades out faster
 *   - PC3 = TCC5 CCB = LED fades in faster
 *   - PC4 = TCC4 CCA = LED fades out
 *   - PC5 = TCC4 CCB = LED fades in
 *   - PC6 = TCC5 CCA = LED fades out faster
 *   - PC7 = TCC5 CCB = LED fades in faster
 * - In OTMX 3 mode:
 *   - PC0 = TCC4 CCA = LED fades out
 *   - PC1 = TCC4 CCA = LED fades out
 *   - PC2 = TCC4 CCA = LED fades out
 *   - PC3 = TCC4 CCA = LED fades out
 *   - PC4 = TCC4 CCA = LED fades out
 *   - PC5 = TCC4 CCA = LED fades out
 *   - PC6 = TCC4 CCA = LED fades out
 *   - PC7 = TCC4 CCA = LED fades out
 * - In OTMX 4 mode:
 *   - PC0 = TCC4 CCA = LED fades out
 *   - PC1 = TCC4 CCA = LED fades out
 *   - PC2 = TCC4 CCA = LED fades out
 *   - PC3 = TCC4 CCA = LED fades out
 *   - PC4 = TCC4 CCA = LED fades out
 *   - PC5 = TCC4 CCA = LED fades out
 *   - PC6 = TCC4 CCA = LED fades out
 *   - PC7 = TCC4 CCB = LED fades in
 *
 * \note
 * All AVR XMEGA E devices can be used.
 * The TC45 driver API can be found \ref tc45_group "here".
 *
 * Main files:
 * - tc45.c timer/counter driver implementation
 * - tc45.h timer/counter driver definitions
 * - tc45_example5.c: tc driver example application
 *
 * \section board_setup Board setup
 * For STK600 board:
 * - uses the RC032X routine board with TQFP32 socket
 * - PD0 must be connected to SW0
 * - Port C must be connected to header LEDS
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

/* Timer TCC4 and TCC5 resolution (Hz)
 * Note: This configure a TC prescaler equal to 32 = SYSCLK 32MHz / 10MHz
 */
#define TIMER_TCC4_TCC5_RESOLUTION  10000000

/* Timer TCC4 periode
 * Note: 320Hz = SYSCLK 10MHz / 31250
 */
#define TIMER_TCC4_PERIOD           31250

/* Timer TCC5 periode
 * Note: 160Hz = TCC4 periode / 2
 */
#define TIMER_TCC5_PERIOD           (TIMER_TCC4_PERIOD / 2)

/**
 * \brief Compare/Capture detection interrupt callback function
 *
 * This function is called when an interrupt has occurred on a Compare A
 * channel.
 * It increments the CC detection level and thus modify the duty-cycle.
 */
static void example_cca_tcc4_interrupt_callback(void)
{
	static uint16_t cca_tcc4_pwm_index = (TIMER_TCC4_PERIOD / 2);
	cca_tcc4_pwm_index += 0x10;
	if (cca_tcc4_pwm_index >= (TIMER_TCC4_PERIOD - 17)) {
		cca_tcc4_pwm_index = 17;
	}

	tc45_write_cc_buffer(&TCC4, TC45_CCA, cca_tcc4_pwm_index);
}

static void example_cca_tcc5_interrupt_callback(void)
{
	static uint16_t cca_tcc5_pwm_index = (TIMER_TCC5_PERIOD / 2);
	cca_tcc5_pwm_index += 0x10;
	if (cca_tcc5_pwm_index >= (TIMER_TCC5_PERIOD - 17)) {
		cca_tcc5_pwm_index = 17;
	}

	tc45_write_cc_buffer(&TCC5, TC45_CCA, cca_tcc5_pwm_index);
}

/**
 * \brief Compare/Capture detection interrupt callback function
 *
 * This function is called when an interrupt has occurred on a Compare B
 * channel.
 * It decrements the CC detection level and thus modify the duty-cycle.
 */
static void example_ccb_tcc4_interrupt_callback(void)
{
	static uint16_t ccb_tcc4_pwm_index = (TIMER_TCC4_PERIOD / 2);
	ccb_tcc4_pwm_index -= 0x10;
	if (ccb_tcc4_pwm_index <= 17) {
		ccb_tcc4_pwm_index = (TIMER_TCC4_PERIOD - 17);
	}

	tc45_write_cc_buffer(&TCC4, TC45_CCB, ccb_tcc4_pwm_index);
}

static void example_ccb_tcc5_interrupt_callback(void)
{
	static uint16_t ccb_tcc5_pwm_index = (TIMER_TCC5_PERIOD / 2);
	ccb_tcc5_pwm_index -= 0x10;
	if (ccb_tcc5_pwm_index <= 17) {
		ccb_tcc5_pwm_index = (TIMER_TCC5_PERIOD - 17);
	}

	tc45_write_cc_buffer(&TCC5, TC45_CCB, ccb_tcc5_pwm_index);
}

int main(void)
{
	uint8_t otmx_mode_index = 0;
	bool btn_released = true;

	/* Usual initializations */
	board_init();
	sysclk_init();
	sleepmgr_init();
	irq_initialize_vectors();
	cpu_irq_enable();

	/* Enables the Timers defined in conf_example.h : TCC4 (1) and TCC5 (2)
	 *in this example */
	tc45_enable(&TCC4);
	tc45_enable(&TCC5);

	/* Configures the interrupt level of CCA and CCB modules of the 2
	 *Timers: low */
	tc45_set_cca_interrupt_level(&TCC4, TC45_INT_LVL_LO);
	tc45_set_ccb_interrupt_level(&TCC4, TC45_INT_LVL_LO);
	tc45_set_cca_interrupt_level(&TCC5, TC45_INT_LVL_LO);
	tc45_set_ccb_interrupt_level(&TCC5, TC45_INT_LVL_LO);

	/* Declares the interrupt functions which will be called when CCA and
	 * CCB
	 * interrupts will occur */
	tc45_set_cca_interrupt_callback(&TCC4,
			example_cca_tcc4_interrupt_callback);
	tc45_set_ccb_interrupt_callback(&TCC4,
			example_ccb_tcc4_interrupt_callback);
	tc45_set_cca_interrupt_callback(&TCC5,
			example_cca_tcc5_interrupt_callback);
	tc45_set_ccb_interrupt_callback(&TCC5,
			example_ccb_tcc5_interrupt_callback);

	/* Configures the Timer periods*/
	tc45_write_period(&TCC4, TIMER_TCC4_PERIOD);
	tc45_write_period(&TCC5, TIMER_TCC5_PERIOD);

	/* Configures the CCA and CCB levels*/
	tc45_write_cc(&TCC4, TC45_CCA, TIMER_TCC4_PERIOD / 2);
	tc45_write_cc(&TCC4, TC45_CCB, TIMER_TCC4_PERIOD / 2);
	tc45_write_cc(&TCC5, TC45_CCA, TIMER_TCC5_PERIOD / 4);
	tc45_write_cc(&TCC5, TC45_CCB, TIMER_TCC5_PERIOD / 4);

	/* Enables the CCA and CCB channels*/
	tc45_enable_cc_channels(&TCC4, TC45_CCACOMP);
	tc45_enable_cc_channels(&TCC4, TC45_CCBCOMP);
	tc45_enable_cc_channels(&TCC5, TC45_CCACOMP);
	tc45_enable_cc_channels(&TCC5, TC45_CCBCOMP);

	/* Configures the waveform genertaor in Dual Slope mode and Top*/
	tc45_set_wgm(&TCC4, TC45_WG_DS_T);
	tc45_set_wgm(&TCC5, TC45_WG_DS_T);

	tc45_set_resolution(&TCC4, TIMER_TCC4_TCC5_RESOLUTION);
	tc45_set_resolution(&TCC5, TIMER_TCC4_TCC5_RESOLUTION);

	 while (1) {
		/* Go to sleep, everything is handled by interrupts. */
		sleepmgr_enter_sleep();

		/* Configures the Output Matrix mode */
		if (gpio_pin_is_high(GPIO_PUSH_BUTTON_0)) {
			/* button released */
			btn_released = true;
			continue;
		}

		/* button pressed */
		if (!btn_released) {
			/* Wait release of button */
			continue;
		}
		btn_released = false;

		/* Change OTMX mode */
		otmx_mode_index++;
		switch (otmx_mode_index) {
		case 0:
			tc45_wex_set_otmx(&WEXC, WEX_OTMX_DEFAULT);
			break;

		case 1:
			tc45_wex_set_otmx(&WEXC, WEX_OTMX_1);
			break;

		case 2:
			tc45_wex_set_otmx(&WEXC, WEX_OTMX_2);
			break;

		case 3:
			tc45_wex_set_otmx(&WEXC, WEX_OTMX_3);
			break;

		case 4:
			tc45_wex_set_otmx(&WEXC, WEX_OTMX_4);
			break;

		default:
			otmx_mode_index = -1;
			break;
		}
	}
}

