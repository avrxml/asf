/**
 * \file
 *
 * \brief set timezone offset in hours and minutes
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

#include "nvm.h"
#include "gfx_mono_menu.h"
#include "keyboard.h"
#include "timezone.h"

#define TZ_HOURS_EEPROM_ADDR   4
#define TZ_MINUTES_EEPROM_ADDR 5

static int8_t tz_hours;
static int8_t tz_minutes;

PROGMEM_DECLARE(char const, tz_hours_title[]) = "Timezone hours UTC";
PROGMEM_DECLARE(char const, tz_n12[]) = "-12: Baker Island";
PROGMEM_DECLARE(char const, tz_n11[]) = "-11: Kingman Reef";
PROGMEM_DECLARE(char const, tz_n10[]) = "-10: Hawaii";
PROGMEM_DECLARE(char const, tz_n09[]) = " -9: Alaska";
PROGMEM_DECLARE(char const, tz_n08[]) = " -8: Pacific";
PROGMEM_DECLARE(char const, tz_n07[]) = " -7: Mountain";
PROGMEM_DECLARE(char const, tz_n06[]) = " -6: Central";
PROGMEM_DECLARE(char const, tz_n05[]) = " -5: Eastern";
PROGMEM_DECLARE(char const, tz_n04[]) = " -4: ECT";
PROGMEM_DECLARE(char const, tz_n03[]) = " -3: Greenland";
PROGMEM_DECLARE(char const, tz_n02[]) = " -2: South Georgia";
PROGMEM_DECLARE(char const, tz_n01[]) = " -1: Praia";
PROGMEM_DECLARE(char const, tz_00[]) =  "UTC: UK (GMT)";
PROGMEM_DECLARE(char const, tz_01[]) =  " +1: Oslo, Berlin";
PROGMEM_DECLARE(char const, tz_02[]) =  " +2: Istanbul";
PROGMEM_DECLARE(char const, tz_03[]) =  " +3: Nairobi";
PROGMEM_DECLARE(char const, tz_04[]) =  " +4: Moscow";
PROGMEM_DECLARE(char const, tz_05[]) =  " +5: Pakistan";
PROGMEM_DECLARE(char const, tz_06[]) =  " +6: Bangladesh";
PROGMEM_DECLARE(char const, tz_07[]) =  " +7: Thailand";
PROGMEM_DECLARE(char const, tz_08[]) =  " +8: Hong Kong";
PROGMEM_DECLARE(char const, tz_09[]) =  " +9: Tokyo";
PROGMEM_DECLARE(char const, tz_10[]) =  "+10: AEST";
PROGMEM_DECLARE(char const, tz_11[]) =  "+11: Solomon Island";
PROGMEM_DECLARE(char const, tz_12[]) =  "+12: Fiji";

PROGMEM_STRING_T tz_hours_strings[] = {
	tz_n12,
	tz_n11,
	tz_n10,
	tz_n09,
	tz_n08,
	tz_n07,
	tz_n06,
	tz_n05,
	tz_n04,
	tz_n03,
	tz_n02,
	tz_n01,
	tz_00,
	tz_01,
	tz_02,
	tz_03,
	tz_04,
	tz_05,
	tz_06,
	tz_07,
	tz_08,
	tz_09,
	tz_10,
	tz_11,
	tz_12
};

struct gfx_mono_menu tz_hours_menu = {
	// Title
	tz_hours_title,
	// Array with menu strings
	tz_hours_strings,
	// Number of menu elements
	25,
	// Initial selection on 0 hours
	12
};

PROGMEM_DECLARE(char const, tz_minutes_title[]) = "Timezone minutes:";
PROGMEM_DECLARE(char const, tz_min_00[]) = " 0 min";
PROGMEM_DECLARE(char const, tz_min_15[]) = "+15 min";
PROGMEM_DECLARE(char const, tz_min_30[]) = "+30 min";
PROGMEM_DECLARE(char const, tz_min_45[]) = "+45 min";

PROGMEM_STRING_T tz_minutes_strings[] = {
	tz_min_00,
	tz_min_15,
	tz_min_30,
	tz_min_45
};

struct gfx_mono_menu tz_minutes_menu = {
	// Title
   tz_minutes_title,
	// Array with menu strings
	tz_minutes_strings,
	// Number of menu elements
	4,
	// Initial selection on 0 minutes
	0
};

/**
 * \brief Convert from tz_minute_menu index to actual minutes
 *
 * \param tz_minutes_index   index in tz_minutes menu
 *
 * \retval actual number of minutes timezone offset
 */
int8_t tz_index_to_minutes(uint8_t tz_minutes_index)
{
	switch(tz_minutes_index) {
		case 0:
			return 0;
		case 1:
			return 15;
		case 2:
			return 30;
		case 3:
			return 45;
		default:
			// unknown index; return 0 minutes offset
			return 0;
	}
}

/**
 * \brief get the current timezone offset (hours)
 *
 * \retval timezone offset (hours)
 */
int8_t timezone_get_hours(void)
{
	return tz_hours;
}

/**
 * \brief get the current timezone offset (minutes)
 *
 * \retval timezone offset (minutes)
 */
int8_t timezone_get_minutes(void)
{
	return tz_minutes;
}

/**
 * \brief get and set timezone stored in EEPROM
 *
 * This function will check if a timezone has been stored in EEPROM.
 * If this is the case it will be set, and used by the date&time application.
 * If it is not set, the "select timezone" application will be started to set
 * it
 */
void timezone_init(void)
{
	uint8_t temp;
	uint8_t invalid_tz = false;

	// Read timezone (hours) from EEPROM
	temp = nvm_eeprom_read_byte(TZ_HOURS_EEPROM_ADDR);

	// convert tz_hours_menu index to actual timezone offset
	tz_hours = ((int8_t)temp) - 12;

	// Check if the timezone is valid
	if((tz_hours <= 12) && (tz_hours >= -12)) {
		// Set timezone menu selection to the selected timezone
		tz_hours_menu.current_selection = temp;
	} else {
		/*
		 * Timezone is outside the range; reset to default and set
		 * invalid.
		 */
		tz_hours = 0;
		tz_hours_menu.current_selection = 12;
		invalid_tz = true;
	}

	temp = nvm_eeprom_read_byte(TZ_MINUTES_EEPROM_ADDR);

	// Check if we have a valid timezone_minutes_menu index
	if(temp < 4) {
		// Convert to actual minutes from stored menu index
		tz_minutes = tz_index_to_minutes(temp);
		// Set the selection in tz_minutes_menu to selected timezone
		tz_minutes_menu.current_selection = temp;
	} else {
		// Outside the range, reset to default and set invalid
		tz_minutes = 0;
		tz_minutes_menu.current_selection = 0;
		invalid_tz = true;
	}

	// If hours or minutes timezone offset was invalid
	if(invalid_tz == true) {
		/*
		 * One (or both) of the stored settings where invalid. start
		 * timezone selection.
		 */
		timezone_select_application();
	}
}

/**
 * \brief Select timezone application
 *
 * This application will show a menu with available timezone offset and set
 * the selected timezone to be used by the date&time application. If the
 * timezone setting differs from the already stored setting in EEPROM, the
 * EEPROM value will be updated with the new selection.
 */
void timezone_select_application(void)
{
	uint8_t readback = 0;
	uint8_t menu_status;
	struct keyboard_event input;

	// Get timezone offset (hours). Do not accept the "back" button
	gfx_mono_menu_init(&tz_hours_menu);

	// Wait for something useful to happen in the menu system
	do {
		// Wait for key release
		do {
			keyboard_get_key_state(&input);
		} while (input.type != KEYBOARD_RELEASE);

		// Send key to menu system
		menu_status = gfx_mono_menu_process_key(&tz_hours_menu,
				input.keycode);
	} while ((menu_status == GFX_MONO_MENU_EVENT_IDLE) ||
			(menu_status == GFX_MONO_MENU_EVENT_EXIT));

	// convert tz_hours_menu index to actual offset
	tz_hours = menu_status - 12;

	// Update EEPROM if setting differs from the one stored in EEPROM
	readback = nvm_eeprom_read_byte(TZ_HOURS_EEPROM_ADDR);
	if(menu_status != readback) {
		nvm_eeprom_write_byte(TZ_HOURS_EEPROM_ADDR, menu_status);
	}

	// Get timezone offset (minutes). Do not accept the "back" button
	gfx_mono_menu_init(&tz_minutes_menu);
	// Wait for something useful to happen in the menu system
	do {
		// wait for key release
		do {
			keyboard_get_key_state(&input);
		} while (input.type != KEYBOARD_RELEASE);

		// send key to menu system
		menu_status = gfx_mono_menu_process_key(&tz_minutes_menu,
				input.keycode);
	} while ((menu_status == GFX_MONO_MENU_EVENT_IDLE) ||
			(menu_status == GFX_MONO_MENU_EVENT_EXIT));

	// Convert tz_minutes_menu index to actual minutes
	tz_minutes = tz_index_to_minutes(menu_status);

	// Update EEPROM if setting differs from the one stored in EEPROM
	readback = nvm_eeprom_read_byte(TZ_MINUTES_EEPROM_ADDR);
	if(menu_status != readback) {
		nvm_eeprom_write_byte(TZ_MINUTES_EEPROM_ADDR, menu_status);
	}
}
