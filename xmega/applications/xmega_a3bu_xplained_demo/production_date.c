/**
 * \file
 *
 * \brief Display time since production date
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
