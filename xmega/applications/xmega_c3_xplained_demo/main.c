/**
 * \file
 *
 * \brief Main functions
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include "conf_app_display.h"
#include "app_cpu_load.h"
#include "app_sampling.h"
#include "app_touch.h"
#include "app_microsd.h"
#include "app_usb.h"

/* ! Link on Atmel icon */
extern struct gfx_mono_bitmap bitmap_atmel;

static void main_introduction(void);
static void main_introduction_delay(uint16_t delay_ms);
static bool main_introduction_is_exist(void);

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
	irq_initialize_vectors();
	cpu_irq_enable();

	/* Initialize ASF services */
	sleepmgr_init();
	sysclk_init();
	board_init();
	gfx_mono_init();
	sd_mmc_init();
	rtc_init();
	stdio_usb_init(); /* Initialize STDIO and start USB */
	udc_stop(); /* Stop USB by default */

	main_introduction();

	/* Initialize tasks */
	app_touch_init();
	app_cpu_load_init();
	app_sampling_init();

	/* The main loop */
	while (true) {
		/* Enter in sleep mode */
		app_cpu_load_enter_sleep();
		sleepmgr_enter_sleep();

		/* Execute tasks */
		app_usb_task();
		app_microsd_task();
		app_sampling_task();
		app_touch_task();
		app_cpu_load_task();
	}
}

/**
 * \brief Shows on display an explanation of how to use the demonstration
 * SW1 pressed skips this explanation.
 */
static void main_introduction(void)
{
	LED_On(LED3_GPIO);  /* Keep power LED on */

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
		gfx_mono_draw_string(DISPLAY_INTRO_HELP_C,
				0, 10 * 2, &sysfont);
	}

	main_introduction_delay(DISPLAY_INTRO_HELP_DELAY);
	/* Display help */
	if (!main_introduction_is_exist()) {
		gfx_mono_init();
		gfx_mono_draw_string(DISPLAY_INTRO_INF_A,
				0, 10 * 0, &sysfont);
		gfx_mono_draw_string(DISPLAY_INTRO_INF_B,
				0, 10 * 1, &sysfont);
		gfx_mono_draw_string(DISPLAY_INTRO_INF_C,
				0, 10 * 2, &sysfont);
	}

	main_introduction_delay(DISPLAY_INTRO_HELP_DELAY);

	/* Clean display */
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
	/* Initialization TC to manage a delay between each slide */
	tc_enable(&TCC1);
	tc_write_clock_source(&TCC1, TC_CLKSEL_DIV1024_gc); /* 24MHz / 1024 */
	tc_set_direction(&TCC1, TC_UP);
	while (delay_ms) {
		tc_write_count(&TCC1, 0);
		uint16_t delay_step = delay_ms;
		if (delay_step > 2800) {
			delay_step = 2500;
		}

		while (tc_read_count(&TCC1) <
				((24000lu * delay_step) / 1024lu)) {
			if (main_introduction_is_exist()) {
				break;
			}
		}
		delay_ms -= delay_step;
	}
	tc_disable(&TCC1);
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
 * \mainpage ASF Demo application for XMEGA-C3 Xplained
 *
 * \section intro Introduction
 * The Atmel AVR XMEGA-C3 Xplained evaluation kit demo software is created to
 * showcase the Atmel AVR ATxmega384C3 device. The demo samples the available
 * sensors connected on ADC and sent the values through OLED display, USB or
 * microSD card. The demo is controlled through mechanical switches and touch
 * buttons.
 * The associated application note is the AVR1940.
 * The AVR1925 application note describes the XMEGA-C3 Xplained hardware in
 * details.
 *
 * \section desc Description
 * At power up, demo shows introduction screens, with explanation how to use
 * the buttons. Then, the application starts with continuous temperature and
 * light sensors acquisition task scheduled with the real time counter (RTC).
 * Sensor values are displayed on the OLED. Depending on user choice on
 * mechanical buttons, these values can be either sent to the USB device
 * Virtual COM port or stored on the microSD card.
 *
 * The application benefits of the Low Power XMEGA architecture and the CPU
 * enters in sleep mode whenever possible. Thus, a task runs in parallel of
 * the others, to display CPU load. This CPU load actually corresponds to the
 * CPU time spent in active mode.
 *
 * The software includes 5 tasks:
 * - \ref sampling_task_group, managing the acquisition of the temperature and
 * light sensors
 * - \ref usb_task_group, managing the connectivity with PC
 * - \ref microsd_task_group, managing data storage and accessibility
 * - \ref touch_task_group, scheduling the QTouch sensor measures
 * - \ref cpu_load_task_group, displaying real time CPU activity
 * (e.g. CPU time in active mode)
 *
 * \image html tasks.png
 *
 * \section usb USB feature
 * When powering the kit through the USB connector, the XMEGA-C3 Xplained
 * starts without any USB PC enumeration. USB support is handled by embedded
 * firmware when SW0 is pressed.
 *
 * The USB feature provides a USB device composite including:
 * - a USB Mass Storage Class (MSC)
 * - a USB Communication Device Class (CDC)
 *
 * \subsection msc USB Mass Storage Class
 * The MSC interface uses the native driver from USB host O.S. and does not
 * require a specific setup. Thus, the microSD card is mounted as an external
 * disk in O.S.
 *
 * To avoid a File System corruption, the data logging is stopped on microSD
 * while the USB is enabled. All sensor data logged on the microSD card are
 * stored in a file named dat_log_c3_xplained_xx.txt where xx will be
 * incremented from 00 for each new session of sensors data storage on the card.
 *
 * \subsection cdc USB Communication Device Class
 * The CDC interface uses the native driver from UNIX O.S., but requires a
 * specific one on Windows O.S. When the USB feature is disabled, the Windows
 * driver file is created by demo application on the microSD card.
 * After USB MSC startup or through another SD card reader, the
 * atmel_devices_cdc.inf file can be selected to install the new USB CDC
 * interface.
 *
 * After having installed the CDC driver, the Virtual COM port can be open
 * through a terminal. The Virtual COM port is not connected to a true RS232
 * COM port, thus the baud rate and parity parameters can be ignored.
 *
 */
