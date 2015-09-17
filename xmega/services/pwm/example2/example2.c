/**
 * \file
 *
 * \brief PWM example for XMEGA
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
 * This example shows how to use the \ref pwm_group with interrupts.
 *
 * \section files Main Files
 * - pwm.c PWM service implementation
 * - pwm.h PWM service definitions
 * - example2.c example application
 *
 * \section device_info Device Info
 * This example is made specifically for XMEGA-A1 Xplained. However, any XMEGA
 * board with LEDs connected to PE0 and PE4 may be used.
 *
 * \section exampledescription Description of the example
 * This example uses two PWM channels on two different LEDs. An interrupt is
 * triggered on each PWM TC overrun, and the interrupt handler will increase
 * the duty cycle of the PWM channels, thus fading the LEDs out (since the LEDs
 * are active low, increased duty cycle means more dimmed light).
 * The two PWM channels are running two different frequencies to make the
 * fading speed different.
 *
 * \section dependencies Dependencies
 * This example depends on the following module:
 * - \ref pwm_group
 *
 * \section apiinfo PWM Service API
 * The PWM service API can be found \ref pwm_group "here"
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, please visit
 * <a href="http://www.atmel.com/">atmel.com</a>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>

struct pwm_config pwm_1_cfg;
struct pwm_config pwm_2_cfg;

volatile uint8_t duty_cycle_percent_1 = 0;
volatile uint8_t duty_cycle_percent_2 = 0;

/**
 * \brief PWM channel 1 interrupt callback function
 */
static void pwm_callback_1 (void)
{
	/* Increase (and wrap at 100) the duty cycle */
	if (duty_cycle_percent_1++ >= 100) {
		duty_cycle_percent_1 = 0;
	}
	/* Set new duty cycle value */
	pwm_set_duty_cycle_percent(&pwm_1_cfg, duty_cycle_percent_1);
}

/**
 * \brief PWM channel 2 interrupt callback function
 */
static void pwm_callback_2 (void)
{
	/* Increase (and wrap at 100) the duty cycle */
	if (duty_cycle_percent_2++ >= 100) {
		duty_cycle_percent_2 = 0;
	}
	/* Set new duty cycle value */
	pwm_set_duty_cycle_percent(&pwm_2_cfg, duty_cycle_percent_2);
}

/**
 * \brief Example 2 main application routine
 */
int main( void )
{
	/* Initialize interrupt controller, board and sysclock */
	pmic_init();
	sysclk_init();

	/* Enable global interrupts */
	cpu_irq_enable();

	/*
	  Set up first PWM channel
	*/

	/* Set PWM to TC E0, channel A (PE0 = LED0), 75 Hz */
	pwm_init(&pwm_1_cfg, PWM_TCE0, PWM_CH_A, 75);

	/* Set callback function on TC overflow */
	pwm_overflow_int_callback(&pwm_1_cfg, pwm_callback_1);

	/*
	  Set up second PWM channel
	*/

	/* Set PWM to TC E1, channel A (PE4 = LED4), 250 Hz */
	pwm_init(&pwm_2_cfg, PWM_TCE1, PWM_CH_A, 250);

	/* Set callback function on TC overflow */
	pwm_overflow_int_callback(&pwm_2_cfg, pwm_callback_2);

	/*
	  Start PWM
	*/

	pwm_start(&pwm_1_cfg, duty_cycle_percent_1);
	pwm_start(&pwm_2_cfg, duty_cycle_percent_2);

	while(1) {
		/* Do nothing. Everything is handled by interrupts. */
	}
}
