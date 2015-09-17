/*****************************************************************************
 *
 * \file
 *
 * \brief External Interrupt Controller Example 1
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

/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the EIC driver. This documentation is also
 * bundled with an example.
 *
 * This example shows how to use an external interrupt to wake up the
 * cpu when in sleep mode (static). \n
 * <b>Operating mode:</b>
 * -# At the beginning, the CPU is set in the <b>Static</b> sleep mode.
 * -# To leave the sleep mode, the user needs :
 *    - Press the Joystick on EVK1100
 *    - Press the pushbutton PB0 on EVK1101
 *    - Set PA20 low on EVK1104
 *    - Set PA29 low (press button PB1) on UC3C_EK
 *    - Set PA09 low on AT32UC3L-EK.
 *    - Press SWITCHES.SW0 (setup can be found in conf_example1)
 * In normal mode (i.e. out of the sleep mode), the led LED0 toggles for a short while;
 * the CPU is then switched back again into the <b>Static</b>
 * sleep mode (back to step 1)...etc
 *
 * \section files Main Files
 * - eic.c: EIC driver;
 * - eic.h: EIC driver header file;
 * - eic_example1.c: EIC example application.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR32 devices with a EIC module can be used. This example has been tested
 * with the following setup:<BR>
 * <ul>
 *  <li>EVK1100 evaluation kit
 *  <li>EVK1101 evaluation kit
 *  <li>EVK1104 evaluation kit
 *  <li>UC3C_EK evaluation kit
 *  <li>STK600+RCUCD routing card;
 *  </ul>
 *
 * \section setupinfo Setup Information
 * CPU speed: <i> Switch to oscillator external FOSC0. </i>
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <avr32/io.h>
#include "compiler.h"
#include "gpio.h"
#include "power_clocks_lib.h"
#include "board.h"
#include "conf_example1.h"
#include "eic.h"


/** \brief Toggle LED 0 for a short while.
 *
 */
static void toggle_led(void)
{
	volatile uint32_t i;
	volatile uint32_t j = 30;
	do {
		j--;
		i = 1000;
		while (i--);

		gpio_tgl_gpio_pin(LED0_GPIO); // Toggle the LED0.
	} while (j);
}


/*
 * \brief main function : do init and loop to wake up CPU through EIC controller
 */
int main(void)
{
	// Structure holding the configuration parameters
	// of the EIC module.
	eic_options_t eic_options;

	// Activate LED0 pin in GPIO output mode and switch LED0 off.
	gpio_set_gpio_pin(LED0_GPIO);

	// Enable level-triggered interrupt.
	eic_options.eic_mode   = EIC_MODE_LEVEL_TRIGGERED;
	// Interrupt will trigger on low-level.
	eic_options.eic_level  = EIC_LEVEL_LOW_LEVEL;
	// Enable filter.
	eic_options.eic_filter = EIC_FILTER_ENABLED;
	// For Wake Up mode, initialize in asynchronous mode
	eic_options.eic_async  = EIC_ASYNCH_MODE;
	// Choose External Interrupt Controller Line
	eic_options.eic_line   = EXT_INT_EXAMPLE_LINE;

	// Map the interrupt line to the GPIO pin with the right peripheral function.
	gpio_enable_module_pin(EXT_INT_EXAMPLE_PIN_LINE, EXT_INT_EXAMPLE_FUNCTION_LINE);
 	/*
	 * Enable the internal pull-up resistor on that pin (because the EIC is
	 * configured such that the interrupt will trigger on low-level, see
	 * eic_options.eic_level).
	 */
	gpio_enable_pin_pull_up(EXT_INT_EXAMPLE_PIN_LINE);

	// Init the EIC controller with the options
	eic_init(&AVR32_EIC, &eic_options,1);

	// Enable External Interrupt Controller Line
	eic_enable_line(&AVR32_EIC, EXT_INT_EXAMPLE_LINE);
	/*
	 * Switch the CPU to static sleep mode.
	 * When the CPU is idle, it is possible to switch off the CPU clock and optionally other
	 * clock domains to save power. This is activated by the sleep instruction, which takes the sleep
	 * mode index number as argument. SLEEP function is defined in \DRIVERS\PM\pm.h.
	 * In static mode, all oscillators, including 32KHz and RC oscillator are stopped.
	 * Bandgap voltage reference BOD detector is turned off.
	 */
	SLEEP(AVR32_PM_SMODE_STATIC);
	/*
	 * Cpu now is in static sleep mode. When the wake-up external interrupt occurs,
	 * the CPU resumes execution here and enter the while(1) loop.
	 */
	while (true)
	{
		// Toggle LED0 for a short while
		toggle_led();

		// Interrupt Line must be cleared to enable next SLEEP action
		eic_clear_interrupt_line(&AVR32_EIC, EXT_INT_EXAMPLE_LINE);

		// re-enter sleep mode.
		SLEEP(AVR32_PM_SMODE_STATIC);
		/*
		 * Cpu now is in static sleep mode. When the wake-up external interrupt occurs,
		 * the CPU resumes execution back from the top of the while loop.
		 */
	}
}
