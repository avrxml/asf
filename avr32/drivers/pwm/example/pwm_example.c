/*****************************************************************************
*
* \file
*
* \brief PWM example driver for AVR32 UC3.
*
* This file provides an example for the PWM on AVR32 UC3 devices.
*
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
*****************************************************************************/

/** \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 *defines, enums, and
 * typedefs for the PWM driver. <BR>It also gives an example of the usage of the
 * PWM on UC3 products.
 * This example shows how to configure a PWM and output it on a GPIO.\n
 * <b>Operating mode</b>:\n Check the used PWM pin with an oscilloscope, you
 * should see a PWM frequency of 22.5 Hz with a PWM duty cycle of 3/4.\n
 * - On \b EVK1100, PWM3 is pin number 54 (PB22) with AT32UC3A0512 in QFP144
 *   package. Also, LED7 is connected to PB22. Thus, you should also see LED7
 *   blink.
 * - On \b EVK1101, PWM0 is pin number 13 (PA07) with AT32UC3B0256 in QFP64
 *   package. Also, LED0 is connected to PA07. Thus, you should also see it
 *   blink.
 * - On \b EVK1105, PWM4 is pin number 59 (PB27) with AT32UC3A0512 in QFP144
 *   package. Also, LED0 is connected to PB27. Thus, you should also see it
 *   blink.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR32 devices with a PWM module can be used. This example has been tested
 * with the following setup:\n
 * - EVK1100 evaluation kit
 * - EVK1101 evaluation kit
 * - EVK1105 evaluation kit
 *
 * \section setupinfo Setup Information
 * CPU speed: <i>Internal RC oscillator (about 115200 Hz)</i>.
 *
 * \section contactinfo Contact Info
 * For more info about Atmel AVR32 visit
 * <a href="http://www.atmel.com/products/AVR32/">Atmel AVR32</a>\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include "conf_example.h"

/** \brief Main function. Execution starts here. */
int main(void)
{
	/* PWM controller configuration. */
	pwm_opt_t pwm_opt =
		{
			.diva = AVR32_PWM_DIVA_CLK_OFF,
			.divb = AVR32_PWM_DIVB_CLK_OFF,
			.prea = AVR32_PWM_PREA_MCK,
			.preb = AVR32_PWM_PREB_MCK
		};

	/* PWM channel configuration structure. */
	avr32_pwm_channel_t pwm_channel = { .ccnt = 0 };
	
	/* With these settings, the output waveform period will be:
	 * (115200/256)/20 == 22.5Hz == (MCK/prescaler)/period, with
	 * MCK == 115200Hz, prescaler == 256, period == 20. */
	pwm_channel.cdty = 5; /* Channel duty cycle, should be < CPRD. */
	pwm_channel.cprd = 20; /* Channel period. */
	pwm_channel.cupd = 0; /* Channel update is not used here. */
	pwm_channel.CMR.calg = PWM_MODE_LEFT_ALIGNED; /* Channel mode. */
	pwm_channel.CMR.cpol = PWM_POLARITY_LOW;      /* Channel polarity. */
	pwm_channel.CMR.cpd = PWM_UPDATE_DUTY;        /* Not used the first time. */
	pwm_channel.CMR.cpre = AVR32_PWM_CPRE_MCK_DIV_256; /* Channel prescaler. */	
	
	/* Enable the alternative mode of the output pin to connect it to the PWM
	 * module within the device. */
	gpio_enable_module_pin(EXAMPLE_PWM_PIN, EXAMPLE_PWM_FUNCTION);

	/* Initialize the PWM module. */
	pwm_init(&pwm_opt);

	/* Set channel configuration to channel 0. */
	pwm_channel_init(EXAMPLE_PWM_CHANNEL_ID, &pwm_channel);

	/* Start channel 0. */
	pwm_start_channels(1 << EXAMPLE_PWM_CHANNEL_ID);

	while (1) {
		/* Loop forever - PWM will be generated by the peripheral */
	}
}
