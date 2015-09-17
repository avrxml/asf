/**
 * \file
 *
 * \brief CPU Cycle Counter Example application for AVR UC3.
 *
 * Copyright (C) 2013-2015 Atmel Corporation. All rights reserved.
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
 */

/**
 * \mainpage
 * \section intro Introduction
 *
 * This document gives an example of the usage of the CPU Cycle counter. The
 * cycle counter is a COUNT register, that increments once every clock. The
 * count register can be used together with the COMPARE register to create a
 * timer with interrupt functionality. The COMPARE register holds a value that
 * the COUNT register is compared against. When the COMPARE and COUNT registers
 * match, a compare interrupt request is generated and COUNT is reset to 0.
 *
 * \section example-description Example Description
 * This example shows how to use the COUNT register together with the COMPARE
 * register to generate an interrupt periodically.
 * Here is the operating mode of the example:
 * - At the beginning of the code, we check that initial default values of the
 * COUNT and COMPARE registers are correct.
 * - Then, COMPARE register is loaded with a delay specified by
 * delay_clock_cycles. This delay is (1 / fCPU) * delay_clock_cycles.
 * - Then the program infinitely loops, using the COUNT and COMPARE interrupt
 * with the above delay. Upon each COUNT and COMPARE match, GPIO line is
 * toggled.
 *
 * \section files Main Files
 * - cycle_counter_example1.c : cycle counter example
 * - cycle_counter.h: cycle counter driver interface
 * - conf_board.h: Simulator configuration for example
 * - conf_clock.h: Clock configuration for example
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - AVR Simulator
 * - CPU clock: <i> Internal RC oscillator (about 115200 Hz) </i>.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <stdint.h>
#include <stdbool.h>
#include <asf.h>

#define _ASSERT_ENABLE_

/** \name Example configuration
 * @{
 */

/**
 * \def CONFIG_SYSCLK_SOURCE
 * \brief Clock source to use
 */
/**
 * \def EXAMPLE_DELAY_MS
 * \brief Delay period between COMPARE interrupts given in milliseconds.
 */

/** @} */

/** Toggle PA03 GPIO in the example upon COMPARE interrupts. */
#define EXAMPLE_TOGGLE_PIN      AVR32_PIN_PA03
/** Example delay period between COMPARE interrupts given in milliseconds. */
#define EXAMPLE_DELAY_MS        100

/** Counter to store the number for COMPARE interrupts. */
static volatile uint32_t        number_of_compares = 0;
/** Number of clock cycles representing the \ref EXAMPLE_DELAY_MS. */
static volatile uint32_t        delay_clock_cycles;
/** Flag to indicate that the GPIO line should be toggled. */
static volatile bool            toggle_gpio = false;

/**
 * \brief COUNT/COMPARE match interrupt handler
 *
 * Interrupt handler to set GPIO toggle flag on COUNT COMPARE match and reloads
 * the COMPARE register.
 */
#if ((defined(__AT32UC3L016__) \
    || defined(__AT32UC3L032__) \
    || defined(__AT32UC3L064__) \
    || UC3C ))
ISR(compare_irq_handler, AVR32_CORE_IRQ_GROUP0, 0)
#else
ISR(compare_irq_handler, AVR32_CORE_IRQ_GROUP, 0)
#endif
{
	/* Count the number of times this IRQ handler has been called */
	number_of_compares++;

	toggle_gpio = true;

	/*
	 * Clear any pending interrupt by writing to the COMPARE register, in
	 * the same go also schedule the next COUNT and COMPARE match
	 * interrupt.
	 */
	Set_sys_compare(Get_sys_count() + delay_clock_cycles);
}

/**
 * \brief Application main loop.
 *
 * Main function sets the COMPARE register and enables the interrupt for
 * COMPARE match. On every COMPARE match interrupt the GPIO line is toggled.
 *
 * \note Use No compiler optimization to visualize the variable updates
 * using Watch Window
 */
int main(void)
{
	uint32_t compare_value;
	uint32_t temp;

	/*
	 * The call to sysclk_init() will disable all non-vital
	 * peripheral clocks, except for the peripheral clocks explicitly
	 * enabled in conf_clock.h.
	 */
	sysclk_init();

	/*
	 * The COMPARE register should initially be equal to 0 (default
	 * value upon reset), indicating that the compare and exception
	 * generation feature is currently disabled.
	 */
	temp = Get_sys_compare();
	Assert(temp == 0);

	/*
	 * The COUNT register increments since reset, hence it should not
	 * be zero.
	 */
	temp = Get_sys_count();
	Assert(temp);

	/*
	 * Disable all interrupts while configuring the interrupt controller to
	 * avoid potential spurious interrupts and undefined behavior.
	 */
	cpu_irq_disable();

	/* Intialize the CPU IRQ vector table */
	irq_initialize_vectors();

	/* Register the compare interrupt handler to the interrupt controller.*/
	irq_register_handler(&compare_irq_handler, AVR32_CORE_COMPARE_IRQ, 0);

	/*
	 * Calculate the number of clock cycles required for the
	 * EXAMPLE_DELAY_MS ms delay (CPU frequency * delay_required).
	 */
	delay_clock_cycles = cpu_ms_2_cy(EXAMPLE_DELAY_MS, sysclk_get_cpu_hz());

	cpu_irq_enable();

	/*
	 * Schedule the COUNT and COMPARE match interrupt in delay_clock_cycles
	 * clock cycles from current COUNT value.
	 */
	temp = Get_sys_count();
	compare_value = temp + delay_clock_cycles;

	/*
	 * If the next COMPARE value is set to 0 the COUNT and COMPARE match
	 * interrupt will be disabled, therefor set the next COMPARE to 1 in
	 * this specific case.
	 */
	if (!compare_value) {
		 compare_value = 1;
	}
	Set_sys_compare(compare_value);

	while (true) {
		/*
		 * When toggle_gpio is set, the GPIO line is toggled and the
		 * flag is cleared.
		 */
		if (toggle_gpio) {
			toggle_gpio = false;

			/*
			 * TODO: place a breakpoint here and watch the
			 * number_of_compares variable. The number_of_compares
			 * variable will increase for every break, since it is
			 * incremented in the interrupt handler.
			 */
			gpio_tgl_gpio_pin(EXAMPLE_TOGGLE_PIN);
		}
	}
}
