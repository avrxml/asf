/**
 * \file
 *
 * \brief megaAVR STK600 GPIO Example
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
 * \section intro Introduction
 * This example demonstrates GPIO port access, pin access, pin interrupt and
 * pin pull up configuration.
 *
 * \section files Files:
 * - mega_gpio_example.c: megaAVR GPIO example application
 * - conf_example.h: Example configuration
 *
 * \section setup Setup
 * This example is made for STK600 with two 10-pin cables in the following
 * strap configuration:
 * - PORTB <-> SWITCHES
 * - PORTD <-> LEDS
 *
 * \section description Description
 * The example will utilize different registers for setting and reading port
 * and pin GPIOs. These GPIOs are connected to switches and LEDs on the STK600.
 *
 * In the end of the example pin change interrupts are enabled and can be
 * tested with STK600 SW0. This will toggle LED0 from the interrupt handler.
 * Not all megaAVR devices supports pin change interrupts, so for those this
 * is disabled.
 *
 * For the best demonstration it's recommended to run this in a debug session
 * and single step through the different operations.
 *
 * \section compinfo Compilation Info
 * This software was written for the <A href="http://gcc.gnu.org/">GNU GCC</A>
 * for AVR. \n
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include "compiler.h"
#include <avr/interrupt.h>
#include "conf_example.h"

// Only use Pin Change Interrupt handler for devices supporting this.
#ifdef EXAMPLE_PCINT_vect

/**
 * \brief Pin change interrupt handler for PB0
 *
 * Create breakpoint here and then run. Press SW0 in order to test
 * this.
 */
ISR(EXAMPLE_PCINT_vect)
{
	/* Toggle PD0 (LED0)
	 *
	 * Note that it will toggle both on button press and release.
	 */
	PIND = (1 << PIND0);
}

#endif

/**
 * \brief Main routine
 */
int main(void)
{
	// Variable to put switch input into
	uint8_t val;

	/* Use port D for output to control the STK600 LEDs.
	 *
	 * First set the data direction register to 0xff in order to set all
	 * port pins to output function.
	 */
	DDRD = 0xff;

	// Set output levels high. Will turn off STK600 LEDs.
	PORTD = 0xff;

	// Set output levels low. Will turn on STK600 LEDs.
	PORTD = 0;


	/* For input we use port B in order to read input from the STK600
	 * switches.
	 *
	 * We don't need to set these pins to input after reset since they
	 * are default set as inputs after reset.
	 *
	 * But a typical requirement when reading switches are pull-ups. STK600
	 * don't require these, but we enable the internal pull-ups just to be
	 * on the safe side.
	 */
	PORTB = 0xff;

	/* Read STK600 switches on port B inputs into a variable.
	 *
	 * Press any of the STK600 switches in order to see a difference here.
	 * A press will be seen as bit value 0 and not press is 1 due to
	 * pull-ups.
	 */
	val = PINB;


	/* It's also possible to control a single or a set of pins on a port.
	 *
	 * Setting pin PD0 high in order to turn off LED0.
	 */
	PORTD |= (1 << PORTD0);

	// Setting pin PD1 low in order to turn on LED1.
	PORTD &= ~(1 << PORTD1);

	// Toggle both pin PD0 and PD1, which toggles LED0 and LED1.
#if defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) \
		|| defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)
	/* For older megaAVR devices read-modify-write PORT register.
	 * This isn't safe for interrupts.
	 */
	PORTD ^= (1 << PIND0) | (1 << PIND1);
#else
	// Use PIN register to toggle on newer megaAVR devices
	PIND = (1 << PIND0) | (1 << PIND1);
#endif

	// Only use Pin Change Interrupt handler for devices supporting this.
#ifdef EXAMPLE_PCICR
	/* Enable pin change interrupt for PB0 which is controlled by SW0
	 *
	 * First we need to enable pin change interrupt for the wanted port.
	 */
	EXAMPLE_PCICR = (1 << EXAMPLE_PCIE);

	// Then we need to set the pin change port mask to get the bit we want.
	EXAMPLE_PCMSK = (1 << PCINT0);
#endif

	// Enable interrupts
	sei();

	/* Busy loop, and a breakpoint can be used in the interrupt handler to
	 * see interrupts being triggered by SW0.
	 */
	while (true);
}
