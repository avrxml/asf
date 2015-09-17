/**
 * \file
 *
 * \brief XMEGA-A3BU Xplained demo application
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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
 * This is the application that is shipped with the XMEGA-A3BU Xplained
 * kits. It consists of a simple menu system and applications demonstrating
 * the features on the kit.
 *
 * The following application are available in the main menu:
 * - "Temperature",  Display the temperature measured by the connected NTC
 *                   sensor.
 * - "Lightsensor",  Display measurements from the connected lightsensor
 * - "Production Date", Display the time elapsed since the part left the
 *                      factory.
 * - "Date & time" Display the current date & time
 * - "Set Timezone", Select your timezone +- UTC
 * - "Toggle backlight", Toggle the LCD backlight
 *
 * \section files Main files:
 * - main.c  Main application
 * - bitmaps.c Bitmaps used in the application
 * - keyboard.c Keyboard driver
 * - ntc_sensor.c Temperature sensor application
 * - lightsensor.c Lightsensor application
 * - production_date.c Time since production application
 * - date_time.c  Date and time from RTC32
 * - timezone.c handle timezones stored in EEPROM and selecting a new one
 * - cdc.c USB CDC class abstraction used for keyboard input
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
 
#include <asf.h>
#include "adc_sensors.h"
#include "date_time.h"
#include "lightsensor.h"
#include "keyboard.h"
#include "ntc_sensor.h"
#include "production_date.h"
#include "timezone.h"
#include "touch_api.h"
#include "cdc.h"
#include "conf_application.h"

/* Main menu: strings are stored in FLASH,
 * the string pointer table and menu struct are stored in RAM.
 */
PROGMEM_DECLARE(char const, main_menu_title[]) = "Main Menu";
PROGMEM_DECLARE(char const, main_menu_1[]) = "Temperature";
PROGMEM_DECLARE(char const, main_menu_2[]) = "Lightsensor";
PROGMEM_DECLARE(char const, main_menu_3[]) = "Production Date";
PROGMEM_DECLARE(char const, main_menu_4[]) = "Date & Time";
PROGMEM_DECLARE(char const, main_menu_5[]) = "Toggle Backlight";

PROGMEM_STRING_T main_menu_strings[] = {
	main_menu_1,
	main_menu_2,
	main_menu_3,
	main_menu_4,
	main_menu_5,
};

struct gfx_mono_menu main_menu = {
	// Title
	main_menu_title,
	// Array with menu strings
	main_menu_strings,
	// Number of menu elements
	5,
	// Initial selection
	0
};

void button_splash(void);

/**
 * \brief Show button names on display
 */
void button_splash(void)
{
	struct keyboard_event input;

	// Clear screen
	gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);

	gfx_mono_draw_filled_circle(10, 10, 4, GFX_PIXEL_SET, GFX_WHOLE);
	gfx_mono_draw_filled_circle(10, 22, 4, GFX_PIXEL_SET, GFX_WHOLE);
	gfx_mono_draw_filled_circle(117, 10, 4, GFX_PIXEL_SET, GFX_WHOLE);
	gfx_mono_draw_filled_circle(117, 22, 4, GFX_PIXEL_SET, GFX_WHOLE);

	gfx_mono_draw_string("Up", 90, 8, &sysfont);
	gfx_mono_draw_string("Down", 80, 20, &sysfont);
	gfx_mono_draw_string("Enter", 20, 8, &sysfont);
	gfx_mono_draw_string("Back", 20, 20, &sysfont);

	// Any key will exit the loop
	while (true) {
		keyboard_get_key_state(&input);
		if (input.type == KEYBOARD_RELEASE) {
			break;
		}
	}
}


/**
 * \brief Main function.
 *
 * Initializes the board and reads out the production date stored in EEPROM and
 * set timezone from EEPROM if it is set. If it is not set it will open the
 * timezone selector to select the local timezone. It then runs the menu system
 * in an infinite while loop.
 */
int main(void)
{
	uint8_t menu_status;
	struct keyboard_event input;
	uint32_t rtc_timestamp;

	sysclk_init();
	board_init();
	pmic_init();
	gfx_mono_init();
	touch_init();
	adc_sensors_init();

	// Enable display backlight
	gpio_set_pin_high(NHD_C12832A1Z_BACKLIGHT);

	// Workaround for known issue: Enable RTC32 sysclk
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_RTC);
	while (RTC32.SYNCCTRL & RTC32_SYNCBUSY_bm) {
		// Wait for RTC32 sysclk to become stable
	}

	// If we have battery power and RTC is running, don't initialize RTC32
	if (rtc_vbat_system_check(false) != VBAT_STATUS_OK) {
		rtc_init();

		// Set current time to after production date
		rtc_timestamp = production_date_get_timestamp() + 1;
		rtc_set_time(rtc_timestamp);
	}

	// Get current time
	rtc_timestamp = rtc_get_time();
	// Make sure RTC has not been set to a too early date .
	if (rtc_timestamp < FIRST_POSSIBLE_TIMESTAMP) {
		// Set time to 01.01.2011 00:00:00
		rtc_set_time(FIRST_POSSIBLE_TIMESTAMP);
	}

	// Initialize USB CDC class
	cdc_start();

	cpu_irq_enable();

	// Display a splash screen showing button functions
	button_splash();

	// Set timezone from EEPROM or to a default value
	timezone_init();

	/* Main loop. Read keyboard status and pass input to menu system.
	 * When an element has been selected in the menu, it will return the
	 * index of the element that should be run. This can be an application
	 * or another menu.
	 */
	while (true) {

		// (re)initialize menu system
		gfx_mono_menu_init(&main_menu);

		do {
			do {
				keyboard_get_key_state(&input);
			// Wait for key release
			} while (input.type != KEYBOARD_RELEASE);

			// Send key to menu system
			menu_status = gfx_mono_menu_process_key(&main_menu, input.keycode);
		// Wait for something useful to happen in the menu system
		} while (menu_status == GFX_MONO_MENU_EVENT_IDLE);

		switch (menu_status) {
		case 0:
			ntc_sensor_application();
			break;
		case 1:
			lightsensor_application();
			break;
		case 2:
			production_date_application();
			break;
		case 3:
			date_time_application();
			break;
		case 4:
			// Toggle LCD Backlight
			gpio_toggle_pin(NHD_C12832A1Z_BACKLIGHT);
			break;
		case GFX_MONO_MENU_EVENT_EXIT:
			// Fall trough to default and show button splash
		default:
			button_splash();
			break;
		};
	}
}
