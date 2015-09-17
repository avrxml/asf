/**
 * \file
 *
 * \brief Display current date and time
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "stdio.h"
#include "keyboard.h"
#include "rtc32.h"
#include "calendar.h"
#include "sysfont.h"
#include "timezone.h"
#include "date_time.h"
#include "gfx_mono_menu.h"
#include "gfx_mono_spinctrl.h"

// Menus
PROGMEM_DECLARE(char const, datetime_menu_title[]) = "Date&Time";
PROGMEM_DECLARE(char const, datetime_menu_1[]) = "Show Date&Time";
PROGMEM_DECLARE(char const, datetime_menu_2[]) = "Set Date";
PROGMEM_DECLARE(char const, datetime_menu_3[]) = "Set Time";
PROGMEM_DECLARE(char const, datetime_menu_4[]) = "Set Timezone";

PROGMEM_STRING_T datetime_menu_strings[] = {
	datetime_menu_1,
	datetime_menu_2,
	datetime_menu_3,
	datetime_menu_4,
};

struct gfx_mono_menu datetime_menu = {
	// Title
	datetime_menu_title,
	// Array with menu strings
	datetime_menu_strings,
	// Number of menu elements
	4,
	// Initial selection
	0
};


PROGMEM_DECLARE(char const, datetime_date_spinner_string_year[]) = "Year";
PROGMEM_DECLARE(char const, datetime_date_spinner_string_month[]) = "Month";
PROGMEM_DECLARE(char const, datetime_date_spinner_string_day[]) = "Day";
PROGMEM_DECLARE(char const, datetime_date_spinner_string_hour[]) = "Hour";
PROGMEM_DECLARE(char const, datetime_date_spinner_string_minute[]) = "Minute";

//! Weekday shortnames
char* day[] = {
	"Sun",
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat"
};

PROGMEM_DECLARE(char const, datetime_invalid_date_string[]) = "Date invalid";

/**
 * \brief Display current date and time in the selected timezone
 */
static void display_date_time_application(void)
{
	struct calendar_date date;
	struct keyboard_event input_key;
	uint8_t width;
	uint8_t height;
	uint8_t offset;
	int8_t tz_hours_temp;
	int8_t tz_minutes_temp;
	uint32_t past_timestamp = 0xFFFFFFFF;
	uint32_t rtc_timestamp = 0;
	char string_buf[32] = "Date & Time";

	//clear screen
	gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);

	// Get current timestamp from the RTC32
	rtc_timestamp = rtc_get_time();

	// Get timezone settings
	tz_hours_temp = timezone_get_hours();
	tz_minutes_temp = timezone_get_minutes();

	// Print "Date & Time" at first line of the display
	gfx_mono_get_string_bounding_box(string_buf, &sysfont, &width, &height);
	offset = (GFX_MONO_LCD_WIDTH - width) / 2;
	gfx_mono_draw_string(string_buf, offset, 0, &sysfont);

	// Convert RTC time to a date struct containing the current date and time
	calendar_timestamp_to_date_tz(rtc_timestamp, tz_hours_temp,
			tz_minutes_temp, &date);

	while (true) {
		// Return from application if "back" key is pressed and released
		keyboard_get_key_state(&input_key);
		if ((input_key.keycode == KEYBOARD_BACK) &&
				(input_key.type == KEYBOARD_RELEASE)) {
			break;
		}

		rtc_timestamp = rtc_get_time();

		// Update printed time if the time has changed
		if(rtc_timestamp != past_timestamp) {
			calendar_add_second_to_date(&date);

			// Print current date centered on line 3
			snprintf(string_buf, sizeof(string_buf),"%s %.2d.%.2d.%d",
					day[date.dayofweek],
			date.date + 1, date.month + 1, date.year);
			gfx_mono_get_string_bounding_box(string_buf, &sysfont, &width,
					&height);
			offset = (GFX_MONO_LCD_WIDTH - width) / 2;
			gfx_mono_draw_string(string_buf, offset, 12, &sysfont);

			snprintf(string_buf, sizeof(string_buf),
					"%.2d:%.2d:%.2d", date.hour,
					date.minute, date.second);

			// Center text
			gfx_mono_get_string_bounding_box(string_buf,
					&sysfont,&width, &height);
			offset = (GFX_MONO_LCD_WIDTH - width) / 2;
			gfx_mono_draw_string(string_buf, offset, 20, &sysfont);

			past_timestamp = rtc_timestamp;
		}
	}
}

/**
 * \brief Set the date using a spinner widget.
 *
 * This function will set show a spinner widget that lets the user select the
 * current date. This function will leave the clock unchanged, and only change
 * the date part of the RTC32 timestamp.
 */
static void set_date_application(void)
{
	struct calendar_date date;
	struct calendar_date new_date;
	struct keyboard_event input;
	uint32_t current_timestamp;
	uint32_t new_timestamp;
	uint8_t tz_hours;
	uint8_t tz_minutes;
	struct gfx_mono_spinctrl year_spinner;
	struct gfx_mono_spinctrl month_spinner;
	struct gfx_mono_spinctrl day_spinner;
	struct gfx_mono_spinctrl_spincollection date_spinners;
	int16_t spinner_results[3];
	uint8_t spinner_status;

	// Prepare the spinner widget for date selection
	gfx_mono_spinctrl_init(&year_spinner, SPINTYPE_INTEGER,
			datetime_date_spinner_string_year, NULL, 2011, 2105, 0);
	gfx_mono_spinctrl_init(&month_spinner, SPINTYPE_INTEGER,
			datetime_date_spinner_string_month, NULL, 1, 12, 0);
	gfx_mono_spinctrl_init(&day_spinner, SPINTYPE_INTEGER,
			datetime_date_spinner_string_day, NULL, 1, 31, 0);

	// Create date spincollector
	gfx_mono_spinctrl_spincollection_init(&date_spinners);
	gfx_mono_spinctrl_spincollection_add_spinner(&year_spinner,
			&date_spinners);
	gfx_mono_spinctrl_spincollection_add_spinner(&month_spinner,
			&date_spinners);
	gfx_mono_spinctrl_spincollection_add_spinner(&day_spinner,
			&date_spinners);

	// Get timezone settings
	tz_hours = timezone_get_hours();
	tz_minutes = timezone_get_minutes();

	// Get current time
	current_timestamp = rtc_get_time();

	// Convert the current timestamp to a datestruct
	calendar_timestamp_to_date_tz(current_timestamp, tz_hours, tz_minutes,
		&date);

	// Set the spinner selection to the current date
	day_spinner.integer_data = date.date + 1;
	month_spinner.integer_data = date.month + 1;
	year_spinner.integer_data = date.year;

	// show the date selection spinner to get a new date
	gfx_mono_spinctrl_spincollection_show(&date_spinners);

	do {
		do {
			keyboard_get_key_state(&input);
			// Wait for key release
		} while (input.type != KEYBOARD_RELEASE);
		// Send key to spinnercollection
		spinner_status = gfx_mono_spinctrl_spincollection_process_key(
				&date_spinners, input.keycode, spinner_results);
	} while (spinner_status != GFX_MONO_SPINCTRL_EVENT_FINISH);

	/* The result are stored in the same order that they were added
	 * we subtract one from month and day, as they are indexed from 0 in the
	 * date struct but we present them as starting from 1 in the spinner.
	 */
	new_date.year = spinner_results[0];
	new_date.month = spinner_results[1] - 1;
	new_date.date = spinner_results[2] - 1;

	// Verify date set
	new_date.hour = 0;
	new_date.minute = 0;
	new_date.second = 0;
	if (calendar_date_to_timestamp_tz(&new_date, tz_hours, tz_minutes) == 0) {
		//Notify the user that the date was invalid and wait for key
		gfx_mono_draw_filled_rect(0, 0, GFX_MONO_LCD_WIDTH,
				GFX_MONO_LCD_HEIGHT, GFX_PIXEL_CLR);
		gfx_mono_draw_progmem_string(
				(char PROGMEM_PTR_T)datetime_invalid_date_string,
				10, 12, &sysfont);

		while (true) {
			keyboard_get_key_state(&input);
			if (input.type == KEYBOARD_RELEASE) {
				break;
			}
		}
		return;
	}

	// Date OK, change.

	// Get current time
	current_timestamp = rtc_get_time();
	// Convert the current timestamp to a datestruct
	calendar_timestamp_to_date_tz(current_timestamp, tz_hours, tz_minutes,
		&date);
	// Set new date in struct
	date.year = new_date.year;
	date.month = new_date.month;
	date.date = new_date.date;

	// Convert back to timestamp
	new_timestamp =
			calendar_date_to_timestamp_tz(&date, tz_hours, tz_minutes);
	// Set new timestamp
	rtc_set_time(new_timestamp);
}

/**
 * \brief Set the time using a spinner widget
 */
static void set_time_application(void)
{
	struct keyboard_event input;
	struct calendar_date date;
	uint32_t timestamp;
	uint8_t tz_hours;
	uint8_t tz_minutes;
	struct gfx_mono_spinctrl hour_spinner;
	struct gfx_mono_spinctrl minute_spinner;
	struct gfx_mono_spinctrl_spincollection time_spinners;
	uint8_t spinner_status;
	int16_t spinner_results[2];

	// Prepare the spinner widget for time selection
	gfx_mono_spinctrl_init(&hour_spinner, SPINTYPE_INTEGER,
			datetime_date_spinner_string_hour, NULL, 0, 23, 0);
	gfx_mono_spinctrl_init(&minute_spinner, SPINTYPE_INTEGER,
			datetime_date_spinner_string_minute, NULL, 0, 59, 0);

	// Create time spincollector
	gfx_mono_spinctrl_spincollection_init(&time_spinners);
	gfx_mono_spinctrl_spincollection_add_spinner(&hour_spinner,
			&time_spinners);
	gfx_mono_spinctrl_spincollection_add_spinner(&minute_spinner,
			&time_spinners);

	// Get timezone settings
	tz_hours = timezone_get_hours();
	tz_minutes = timezone_get_minutes();

	timestamp = rtc_get_time();

	calendar_timestamp_to_date_tz(timestamp, tz_hours, tz_minutes, &date);

	// Set spinners to current time as initial position
	hour_spinner.integer_data = date.hour;
	minute_spinner.integer_data = date.minute;

	gfx_mono_spinctrl_spincollection_show(&time_spinners);

	do {
		do {
			keyboard_get_key_state(&input);
			// Wait for key release
		} while (input.type != KEYBOARD_RELEASE);
		// Send key to spinnercollection
		spinner_status = gfx_mono_spinctrl_spincollection_process_key(
				&time_spinners, input.keycode, spinner_results);
	} while (spinner_status != GFX_MONO_SPINCTRL_EVENT_FINISH);

	date.hour = spinner_results[0];
	date.minute = spinner_results[1];

	timestamp = calendar_date_to_timestamp_tz(&date, tz_hours, tz_minutes);

	if(timestamp != 0) {
		rtc_set_time(timestamp);
	}
}

/**
 * \brief Show a menu presenting the date and time applications
 */
void date_time_application(void)
{
	uint8_t menu_status;
	struct keyboard_event input;

	while(true) {
		gfx_mono_menu_init(&datetime_menu);
		do {
			do {
				keyboard_get_key_state(&input);
				// Wait for key release
			} while (input.type != KEYBOARD_RELEASE);

			// Send key to menu system
			menu_status = gfx_mono_menu_process_key(&datetime_menu,
					input.keycode);
			// Wait for something useful to happen in the menu system
		} while (menu_status == GFX_MONO_MENU_EVENT_IDLE);

		switch(menu_status) {
		case 0:
			display_date_time_application();
			break;
		case 1:
			set_date_application();
			break;
		case 2:
			set_time_application();
			break;
		case 3:
			timezone_select_application();
			break;
		default:
			// Return to the main menu on unknown element or back key
			return;
		}
	}
}
