/**
 * \file
 *
 * \brief PWM example for XMEGA
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
 * <a href="http://www.microchip.com/">atmel.com</a>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
