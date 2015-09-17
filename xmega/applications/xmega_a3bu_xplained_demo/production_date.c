/**
 * \file
 *
 * \brief Display time since production date
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
#include "nvm.h"
#include "rtc32.h"
#include "calendar.h"
#include "sysfont.h"
#include "production_date.h"
#include "conf_application.h"
#include "nvm.h"

/**
 * \brief Production date get timestamp
 *
 * This function reads the production date time stamp stored in the
 * user signature row addresses PRODUCTION_DATE_ADDRESS to
 * PRODUCTION_DATE_ADDRESS + 3 (4 bytes) and returns a 32 bit value in the
 * UNIX time format.
 *
 * \return Production date timestamp
 */
uint32_t production_date_get_timestamp(void)
{
	// Read timestamp from signature
	uint32_t production_date_timestamp =                             \
		(uint32_t) nvm_read_user_signature_row(                      \
				PRODUCTION_DATE_ADDRESS)          << 24 |            \
		(uint32_t) nvm_read_user_signature_row(                      \
				PRODUCTION_DATE_ADDRESS + 1)      << 16 |            \
		(uint32_t) nvm_read_user_signature_row(                      \
				PRODUCTION_DATE_ADDRESS + 2)      <<  8 |            \
		(uint32_t) nvm_read_user_signature_row(                      \
				PRODUCTION_DATE_ADDRESS + 3);

	// Check if timestamp is invalid
	if ((production_date_timestamp == 0xFFFFFFFF) ||
			(production_date_timestamp < FIRST_POSSIBLE_TIMESTAMP)) {
		production_date_timestamp = FIRST_POSSIBLE_TIMESTAMP;
	}

	return production_date_timestamp;
}

/**
 * \brief Production date application
 *
 * This application will display the time since the XMEGA-A3BU Xplained board
 * has been produced. During production the production time is stored in EEPROM
 * and the RTC is initialized to the same time. The RTC is then left running
 * from the battery backup. This application reads out the production date from
 * EEPROM and shows the difference between the current time and the production
 * date; in other words the time the RTC timer has been running on battery
 * since production.
 *
 * \note If the EEPROM is erased and no production date can be set, it will
 * default to time 01.01.1970, the start of the UNIX time epoch.
 */
void production_date_application(void)
{
	struct keyboard_event input_key;
	uint32_t past_timestamp = 0xFFFFFFFF;
	uint32_t rtc_timestamp;
	struct calendar_date rtc_date;
	struct calendar_date date_diff;
	struct calendar_date production_date;
	uint32_t production_date_timestamp;
	uint16_t months;
	uint8_t width;
	uint8_t height;
	uint8_t offset;
	char string_buf[22];

	// Clear screen
	gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);

	// Draw application title
	gfx_mono_draw_string("Time since production", 0, 0, &sysfont);

	// Get production timestamp
	production_date_timestamp = production_date_get_timestamp();

	// Convert timestamp to date struct
	calendar_timestamp_to_date(production_date_timestamp, &production_date);

	// Exit the application if "back" key is pressed
	while (true) {
		keyboard_get_key_state(&input_key);
		if ((input_key.keycode == KEYBOARD_BACK) &&
				(input_key.type == KEYBOARD_RELEASE)) {
			break;
		}

		// Get current time from RTC32
		rtc_timestamp = rtc_get_time();

		if (rtc_timestamp == past_timestamp) {
			// Same time as last time, no need for update
			continue;
		}

		past_timestamp = rtc_timestamp;

		// Convert timestamp to date struct
		calendar_timestamp_to_date(rtc_timestamp, &rtc_date);

		// Find the difference between the current date and production date
		calendar_time_between_dates(&rtc_date, &production_date, &date_diff);

		// Use months + year*12 as we are a line short to have both year and month
		months = (date_diff.year * 12) + date_diff.month;

		// Center month string on screen. Align day string with month string
		snprintf(string_buf, sizeof(string_buf), "%2d Months", months);
		gfx_mono_get_string_bounding_box(string_buf, &sysfont, &width, &height);
		offset = (GFX_MONO_LCD_WIDTH - width) / 2;
		gfx_mono_draw_string(string_buf, offset, 8, &sysfont);

		snprintf(string_buf, sizeof(string_buf), "%2d Days", date_diff.date);
		gfx_mono_draw_string(string_buf, offset, 16, &sysfont);

		// Display hour, minute, second
		snprintf(string_buf, sizeof(string_buf),"%.2d:%.2d:%.2d", date_diff.hour,
				date_diff.minute, date_diff.second);
		gfx_mono_get_string_bounding_box(string_buf, &sysfont, &width, &height);
		offset = (GFX_MONO_LCD_WIDTH - width) / 2;
		gfx_mono_draw_string(string_buf, offset, 24, &sysfont);
	}
}
