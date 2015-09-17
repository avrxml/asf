/**
 * \file
 *
 * \brief Main functions
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include "conf_app_display.h"
#include "app_cpu_load.h"
#include "app_sampling.h"

/* ! Link on Atmel icon */
extern struct gfx_mono_bitmap bitmap_atmel;

static void main_introduction(void);
static void main_introduction_delay(uint16_t delay_ms);
static bool main_introduction_is_exist(void);

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate   = 38400,
		.charlength = USART_CHSIZE_8BIT_gc,
		.paritytype = USART_PMODE_DISABLED_gc,
		.stopbits   = false, /* No extra stopbit */
	};

	irq_initialize_vectors();
	cpu_irq_enable();

	/* Initialize ASF services */
	sleepmgr_init();
	sysclk_init();
	board_init();
	gfx_mono_init();
	rtc_init();
	/* UART link with board controller */
	stdio_serial_init(&USARTD0, &usart_serial_options);

	main_introduction();
	/* Initialize tasks */
	app_cpu_load_init();
	app_sampling_init();

	/* Initialization TC to generate an interrupt (CPU wakeup) each 100ms */
	tc45_enable(&TCD5);
	tc45_write_period(&TCD5, 100 * (32000 / 1024));
	tc45_write_clock_source(&TCD5, TC45_CLKSEL_DIV1024_gc);
	tc45_set_cca_interrupt_level(&TCD5, PMIC_LVL_LOW);

	/* The main loop */
	while (true) {
		/* Enter in sleep mode */
		app_cpu_load_enter_sleep();
		sleepmgr_enter_sleep();

		/* Execute tasks */
		app_sampling_task();
		app_cpu_load_task();
	}
}

/**
 * \brief Shows a brief explanation of how to use the demonstration using the
 * on-board display.
 * SW1 pressed will skip this explanation.
 */
static void main_introduction(void)
{
	LED_On(LED0_GPIO);  /* Keep power LED on */

	/* Display Atmel logo */
	if (!main_introduction_is_exist()) {
		gfx_mono_generic_put_bitmap(&bitmap_atmel, 10, 0);
		gfx_mono_draw_string(DISPLAY_INTRO_MSG_EXIT,
				0, 25, &sysfont);
	}

	main_introduction_delay(DISPLAY_INTRO_ATMEL_DELAY);
	/* Display message */
	if (!main_introduction_is_exist()) {
		gfx_mono_init();
		gfx_mono_draw_string(DISPLAY_INTRO_MSG_A,
				0, 10 * 0, &sysfont);
		gfx_mono_draw_string(DISPLAY_INTRO_MSG_B,
				0, 10 * 1, &sysfont);
		gfx_mono_draw_string(DISPLAY_INTRO_MSG_C,
				0, 10 * 2, &sysfont);
	}

	main_introduction_delay(DISPLAY_INTRO_HELP_DELAY);
	/* Display help */
	if (!main_introduction_is_exist()) {
		gfx_mono_init();
		gfx_mono_draw_string(DISPLAY_INTRO_HELP_A,
				0, 10 * 0, &sysfont);
		gfx_mono_draw_string(DISPLAY_INTRO_HELP_B,
				0, 10 * 1, &sysfont);
	}

	main_introduction_delay(DISPLAY_INTRO_HELP_DELAY);

	/* Clear display */
	gfx_mono_init();
}

/**
 * \brief Delay function which use a Timer Counter and can be aborted
 * SW1 pressed stop delay.
 *
 * \param delay_ms  delay in ms
 */
static void main_introduction_delay(uint16_t delay_ms)
{
	/* Initialization TC to manage a delay between each slides */
	tc45_enable(&TCD5);
	/* 32MHz / 1024 */
	tc45_write_clock_source(&TCD5, TC45_CLKSEL_DIV1024_gc);
	tc45_set_direction(&TCD5, TC45_UP);
	while (delay_ms) {
		tc45_write_count(&TCD5, 0);
		uint16_t delay_step = delay_ms;
		if (delay_step > 2000) {
			delay_step = 2000;
		}

		while (tc45_read_count(&TCD5) <
				((32000lu * delay_step) / 1024lu)) {
			if (main_introduction_is_exist()) {
				break;
			}
		}
		delay_ms -= delay_step;
	}
	tc45_disable(&TCD5);
}

/**
 * \brief Returns the SW1 state
 *
 * \return Exit is requested if true
 */
static bool main_introduction_is_exist(void)
{
	if (ioport_pin_is_low(GPIO_PUSH_BUTTON_1)) {
		return true;
	}

	return false;
}

/**
 * \mainpage ASF Demo application for XMEGA-E5 Xplained
 *
 * \section intro Introduction
 * The Atmel XMEGA-E5 Xplained evaluation kit demo software allows to
 * demonstrate some of the Atmel AVR ATxmega32E5 device capabilities.
 * The demo samples the on-board light sensor connected to ADC and sends the 
 * converted values to the OLED display and UART. 
 * The demo is controlled through mechanical switches.
 *
 * The application note associated is the AN-2657
 * "XMEGA-E5 Xplained Software Guide".
 *
 * \section desc Description
 * At power up, demo shows introduction screens, with some help about how to use
 * the buttons. Then, the application performs continuous sensor data sampling
 * scheduled with the real time counter (RTC). Sensor values
 * are displayed on the OLED and sent to UART (38400 baud).
 * The UART is connected to a board controller which includes a USB
 * device Virtual COM port.
 *
 * The application benefits of the Low Power XMEGA architecture, and the CPU
 * enters in sleep mode whenever possible. Another task running in parallel
 * allows to monitor the CPU load.
 * The software includes 2 tasks:
 * - \ref sampling_task_group, managing the acquisition of the temperature and
 * light sensors
 * - \ref cpu_load_task_group, displaying real time CPU activity
 * (e.g. CPU time in active mode)
 *
 * \subsection cdc USB Communication Device Class
 * On board controller, the CDC interface uses the native driver from UNIX O.S.,
 * but requires a specific one on Windows O.S..
 *
 * Once the CDC driver is installed, the Virtual COM port can be open
 * through a terminal. The Virtual COM port is connected to a true RS232
 * COM port bridge, thus the baud rate must 38400 with no parity.
 *
 */
