/**
 * \file
 *
 * \brief GPIO example application for AVR UC3 GPIO driver.
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

/** \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the GPIO driver.
 *
 * General Purpose Input/Output manages the I/O pins of the microcontroller.
 * Each I/O line may be dedicated as a general-purpose I/O or be assigned to
 * a function of an embedded peripheral.
 * This assures effective optimization of the pins of a product.
 *
 * The example covers various uses of the GPIO controller and demonstrates
 * different GPIO functionalities using the peripheral bus interface.
 * <b>Example's operating mode: </b>
 * -# Place breakpoint in gpio_tgl_gpio_pin() function in ISR
 * -# Open IO view and watch the value change in PVR and OVR
 *
 * \section files Main Files
 * - gpio.c: GPIO driver;
 * - gpio.h: GPIO driver header file;
 * - conf_example.h: GPIO configuration file
 * - conf_clock.h: Clock Configuration for example
 * - gpio_simulator_example.c: GPIO example application
 *   for use with the simulator.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32.
 *
 * \section deviceinfo Device Info
 * All AVR Simulator supported AVR32 devices with a GPIO module can be used.
 * This example has been tested with the following setup:
 *   - AVR Studio 5.0 AVR Simulator
 *
 * \section setupinfo Setup Information
 * CPU speed: <i> Internal RC oscillator (about 115200 Hz) </i>.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include "gpio.h"
#include <board.h>
#include <sysclk.h>
#include "intc.h"
#include "conf_clock.h"
#include "conf_example.h"

/**
 * \brief GPIO interrupt handler
 */
#if __GNUC__
__attribute__((__interrupt__))
#endif
static void gpio_pin_change_interrupt_handler(void)
{
	// Clear the pin change interrupt flag
	gpio_clear_pin_interrupt_flag(GPIO_PIN_EXAMPLE_2);
	// Toggle GPIO_PIN_EXAMPLE_3
	gpio_tgl_gpio_pin(GPIO_PIN_EXAMPLE_3);
}

/**
 * \brief Application main loop.
 */
int main(void)
{
	uint32_t state = 0;
	/**
	 * \note the call to sysclk_init() will disable all non-vital
	 * peripheral clocks, except for the peripheral clocks explicitly
	 * enabled in conf_clock.h.
	 */
	sysclk_init();

	/**
	 * Enable the clock to the selected example GPIO peripheral module.
	 */
	sysclk_enable_pba_module(SYSCLK_GPIO);

	// Pull-up is enabled for all used pins
	gpio_enable_pin_pull_up(GPIO_PIN_EXAMPLE_1);
	gpio_enable_pin_pull_up(GPIO_PIN_EXAMPLE_2);
	gpio_enable_pin_pull_up(GPIO_PIN_EXAMPLE_3);


	// Disable all interrupts
	cpu_irq_disable();
	INTC_init_interrupts();
	// Register GPIO Pin Change Interrupt
	INTC_register_interrupt(&gpio_pin_change_interrupt_handler,
			AVR32_GPIO_IRQ_0, AVR32_INTC_INT0);
	// Enable pin change interrupt for GPIO_PIN_EXAMPLE_2
	gpio_enable_pin_interrupt(GPIO_PIN_EXAMPLE_2, GPIO_PIN_CHANGE);
	// Enable all interrupts
	cpu_irq_enable();

	// Based on the variable, GPIO_PIN_EXAMPLE_1 will be changed.
	while (1) {
		switch (state) {
		case 0:
			// Access with GPIO driver gpio.c with clear and set access.
			gpio_clr_gpio_pin(GPIO_PIN_EXAMPLE_1);
			state++;
			break;

		case 1:
			gpio_set_gpio_pin(GPIO_PIN_EXAMPLE_1);
			state++;
			break;

		case 2:
			// Note that it is also possible to use the GPIO toggle feature.
			gpio_tgl_gpio_pin(GPIO_PIN_EXAMPLE_1);
			state++;
			break;

		default:
			gpio_tgl_gpio_pin(GPIO_PIN_EXAMPLE_1);
			state = 0;
			break;
		}

		// Check GPIO_PIN_EXAMPLE_1 value to modify GPIO_PIN_EXAMPLE_2.
		if (gpio_get_pin_value(GPIO_PIN_EXAMPLE_1) == 0)
			gpio_clr_gpio_pin(GPIO_PIN_EXAMPLE_2);
		else
			gpio_set_gpio_pin(GPIO_PIN_EXAMPLE_2);
	}
	while (true) {
		cpu_relax();
	}
}
