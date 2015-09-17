/**
 * \file
 *
 * \brief AVR XMEGA TC45 Driver Example 4
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
 * This example shows how to use the Capture feature of the \ref tc_group
 * module with event system.
 *
 * The example configures the TCD5 modules in frequency capture mode.
 * The event system is configured to trigger a capture when switch
 * 0 is pressed and released.
 *
 * The captured value which is the timing between press and release,
 * is displayed on the leds. The number of LED ON increases according to
 * measured timing.
 *
 * \note
 * All AVR XMEGA E devices can be used.
 * The TC45 driver API can be found \ref tc45_group "here".
 *
 * Main files:
 * - tc45.c Timer XMEGA Timer Counter driver implementation
 * - tc45.h Timer XMEGA Timer Counter driver definitions
 * - tc_example4.c example application
 *
 * \section board_setup Board setup
 * For STK600 board:
 * - uses the RC032X routine board with TQFP32 socket
 * - PA0 must be connected to SW0
 * - Port D must be connected to header LEDS
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

/* Fix header file about Prescaler Filter for XMEGA E */
#undef EVSYS_PRESCFILT_CH04_gc
#define EVSYS_PRESCFILT_CH04_gc (1<<4)

/* Interrupt callback function declaration */
void cca_callback(void);

/**
 * \brief Callback function for TC_CCA interrupt
 *
 * This function reads the result from tc frequency capture and displays the
 * lower 8 bits of the result on leds.
 */
void cca_callback(void)
{
	uint16_t timing_ms, cca_capture;
	uint8_t level;
	
	/* Store the compare channel result */
	cca_capture = tc45_read_cc(&TCD5, TC45_CCA);

	if (tc45_is_overflow(&TCD5)) {
		tc45_clear_overflow(&TCD5);
		/* Turn ON all LEDs */
		PORTD.OUT = 0x00;
		return;
	}
	/* Compute timing in ms */
	timing_ms = ((uint32_t)cca_capture * 1000)
			/ tc45_get_resolution(&TCD5);
	/* Turn ON the LEDs according to captured value */
	if (timing_ms < 250) {
		level = 0;
	} else if (timing_ms < 500) {
		level = 1;
	} else if (timing_ms < 1000) {
		level = 2;
	} else {
		level = (timing_ms / 1000) + 2;
	}
	PORTD.OUT = ~(((uint16_t)1 << level) - 1);
}

/**
 * \brief Main application routine
 */
int main(void)
{
	/* Usual initializations */
	board_init();
	sysclk_init();
	sleepmgr_init();
	irq_initialize_vectors();
	cpu_irq_enable();

	/* Enable and turn off all leds */
	PORTD.DIRSET = 0xFF;
	PORTD.OUT = 0xFF;

	/* Enable trigger on both edges for PORT A pin 0 (Switch 0) */
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTA,0),
			IOPORT_DIR_INPUT | IOPORT_PULL_UP | IOPORT_SENSE_BOTHEDGES | IOPORT_INV_ENABLED);

	/* Enable clock to event sys */
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EVSYS);
	/* Setup PORT A pin 0 (sw0) as input to event system channel 0 */
	EVSYS.CH0MUX = EVSYS_CHMUX_PORTA_PIN0_gc;
	/* Enable prescaler of filter */
	EVSYS.DFCTRL = EVSYS_PRESCFILT_CH04_gc	| EVSYS_PRESCALER_CLKPER_32768_gc;
	EVSYS.CH0CTRL = EVSYS_DIGFILT_2SAMPLES_gc;
	
	/* Enable clock to timer */
	tc45_enable(&TCD5);

	/* Set maximum period */
	tc45_write_period(&TCD5, 0xffff);

	/* Enable capture channels */
	tc45_enable_cc_channels(&TCD5, TC45_CCACAPT);

	/* Clear timer interrupts */
	tc45_clear_cc_interrupt(&TCD5, TC45_CCA);

	/* Do capture on event channel 0 (sw0) */
	tc45_set_input_capture(&TCD5, TC45_EVSEL_CH0_gc,
			TC45_EVACT_PWF_gc);

	/* Register callback for interrupt */
	tc45_set_cca_interrupt_callback(&TCD5, &cca_callback);

	/* Enable CCA interrupt at level low */
	tc45_set_cca_interrupt_level(&TCD5, TC45_INT_LVL_LO);

	/* Set resolution to minimum and start timer */
	tc45_set_resolution(&TCD5, 1);

	/* Enable global interrupts */
	cpu_irq_enable();

	while (1) {
		/* Go to sleep, everything is handled by interrupts. */
		sleepmgr_enter_sleep();
	}
}
