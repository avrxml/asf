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
 * This simple example shows how to use the \ref pwm_group to dim LEDs.
 *
 * \section files Main Files
 * - pwm.c PWM service implementation
 * - pwm.h PWM service definitions
 * - example1.c example application
 *
 * \section device_info Device Info
 * This example is made specifically for XMEGA-A1 Xplained. However, any XMEGA
 * board with LEDs connected to PE0-PE6 may be used.
 *
 * \section exampledescription Description of the example
 * This example sets up 4 PWM channels and outputs them to 4 different LEDs,
 * giving the impression the LEDs are dimmed.
 * All 4 PWM channels are set up with the same frequency, but uses different
 * duty cycles to give different level of dimming.
 * LED3 is set up to light constantly (since the LEDs are active low, this
 * means 0% duty cycle)
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

/**
 * \brief Example 1 main application routine
 */
int main( void )
{
	/* The 4 PWM config structs */
	struct pwm_config pwm_cfg[4];

	/* Initialize sysclock */;
	sysclk_init();

	/* Set up all 4 PWM channels with 500 Hz frequency. We want to use TCE0
	   since this is where our LEDs are connected (PE0-PE3) */
	pwm_init(&pwm_cfg[0], PWM_TCE0, PWM_CH_A, 500); /* LED0 / PE0 */
	pwm_init(&pwm_cfg[1], PWM_TCE0, PWM_CH_B, 500); /* LED1 / PE1 */
	pwm_init(&pwm_cfg[2], PWM_TCE0, PWM_CH_C, 500); /* LED2 / PE2 */
	pwm_init(&pwm_cfg[3], PWM_TCE0, PWM_CH_D, 500); /* LED3 / PE3 */

	/* Start all 4 PWM channels with different duty cycles
	  (which means different LED intensity).
	  Since the LEDs are active low, 0% duty cycle means constantly lit */
	pwm_start(&pwm_cfg[0], 97);
	pwm_start(&pwm_cfg[1], 85);
	pwm_start(&pwm_cfg[2], 60);
	pwm_start(&pwm_cfg[3], 0);

	while(1) {
		/* Nothing to do */
	}
}
