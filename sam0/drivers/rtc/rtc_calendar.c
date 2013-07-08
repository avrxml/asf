/**
 * \file
 *
 * \brief SAM D20 RTC Driver (Calendar Mode)
 *
 * Copyright (C) 2012-2013 Atmel Corporation. All rights reserved.
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
#include "rtc_calendar.h"
#include <gclk.h>

#if !defined(__DOXYGEN__)
volatile struct _rtc_device _rtc_dev;
#endif

/**
 * \brief Resets the RTC module
 * Resets the RTC module to hardware defaults.
 */
void rtc_calendar_reset(void)
{
	/* Initialize module pointer. */
	Rtc *const rtc_module = RTC;

	/* Disable module before reset. */
	rtc_calendar_disable();

#if RTC_CALENDAR_ASYNC == true
	_rtc_dev.registered_callback = 0;
	_rtc_dev.enabled_callback    = 0;
#endif

	while (rtc_calendar_is_syncing()) {
		/* Wait for synchronization */
	}

	/* Initiate software reset. */
	rtc_module->MODE2.CTRL.reg |= RTC_MODE2_CTRL_SWRST;
}

/**
 * \internal Convert time structure to register_value.
 */
static uint32_t _rtc_calendar_time_to_register_value(
		const struct rtc_calendar_time *const time)
{
	/* Initialize return value. */
	uint32_t register_value;

	/* Set year value into register_value minus initial year. */
	register_value = (time->year - _rtc_dev.year_init_value) <<
			RTC_MODE2_CLOCK_YEAR_Pos;

	/* Set month value into register_value. */
	register_value |= (time->month << RTC_MODE2_CLOCK_MONTH_Pos);

	/* Set day value into register_value. */
	register_value |= (time->day << RTC_MODE2_CLOCK_DAY_Pos);

	/* Set 24 hour value into register_value. */
	register_value |= (time->hour << RTC_MODE2_CLOCK_HOUR_Pos);

	/* Check if 24 h clock and set pm flag. */
	if (!(_rtc_dev.clock_24h) && (time->pm)) {
		/* Set pm flag. */
		register_value |= RTC_MODE2_CLOCK_HOUR_PM;
	}

	/* Set minute value into register_value. */
	register_value |= (time->minute << RTC_MODE2_CLOCK_MINUTE_Pos);

	/* Set second value into register_value. */
	register_value |= (time->second << RTC_MODE2_CLOCK_SECOND_Pos);

	return register_value;
}

/**
 * \internal Convert register_value to time structure.
 */
static void _rtc_calendar_register_value_to_time(
		const uint32_t register_value,
		struct rtc_calendar_time *const time)
{
	/* Set year plus value of initial year. */
	time->year = ((register_value & RTC_MODE2_CLOCK_YEAR_Msk) >>
			RTC_MODE2_CLOCK_YEAR_Pos) + _rtc_dev.year_init_value;

	/* Set month value into time struct. */
	time->month = ((register_value & RTC_MODE2_CLOCK_MONTH_Msk) >>
			RTC_MODE2_CLOCK_MONTH_Pos);

	/* Set day value into time struct. */
	time->day = ((register_value & RTC_MODE2_CLOCK_DAY_Msk) >>
			RTC_MODE2_CLOCK_DAY_Pos);

	if (_rtc_dev.clock_24h) {
		/* Set hour in 24h mode. */
		time->hour = ((register_value & RTC_MODE2_CLOCK_HOUR_Msk) >>
				RTC_MODE2_CLOCK_HOUR_Pos);
	} else {
		/* Set hour in 12h mode. */
		time->hour = ((register_value &
				(RTC_MODE2_CLOCK_HOUR_Msk & ~RTC_MODE2_CLOCK_HOUR_PM)) >>
				RTC_MODE2_CLOCK_HOUR_Pos);

		/* Set pm flag */
		time->pm = ((register_value & RTC_MODE2_CLOCK_HOUR_PM) != 0);
	}

	/* Set minute value into time struct. */
	time->minute = ((register_value & RTC_MODE2_CLOCK_MINUTE_Msk) >>
			RTC_MODE2_CLOCK_MINUTE_Pos);

	/* Set second value into time struct. */
	time->second = ((register_value & RTC_MODE2_CLOCK_SECOND_Msk) >>
			RTC_MODE2_CLOCK_SECOND_Pos);
}

/**
 * \internal Applies the given configuration.
 *
 * Set the configurations given from the configuration structure to the
 * hardware module.
 *
 * \param[in] config  Pointer to the configuration structure.
 */
static void _rtc_calendar_set_config(
		const struct rtc_calendar_config *const config)
{
	/* Initialize module pointer. */
	Rtc *const rtc_module = RTC;

	/* Set up temporary register value. */
	uint16_t tmp_reg;

	/* Set to calendar mode and set the prescaler. */
	tmp_reg = RTC_MODE2_CTRL_MODE(2) | config->prescaler;

	/* Check clock mode. */
	if (!(config->clock_24h)) {
		/* Set clock mode 12h. */
		tmp_reg |= RTC_MODE2_CTRL_CLKREP;
	}

	/* Check for clear on compare match. */
	if (config->clear_on_match) {
		/* Set clear on compare match. */
		tmp_reg |= RTC_MODE2_CTRL_MATCHCLR;
	}

	/* Set temporary value to register. */
	rtc_module->MODE2.CTRL.reg = tmp_reg;

	/* Check to set continuously clock read update mode. */
	if (config->continuously_update) {
		/* Set continuously mode. */
		rtc_module->MODE2.READREQ.reg |= RTC_READREQ_RCONT;
	}

	/* Set alarm time registers. */
	for (uint8_t i = 0; i < RTC_NUM_OF_ALARMS; i++) {
		rtc_calendar_set_alarm(&(config->alarm[i]), (enum rtc_calendar_alarm)i);
	}
}

/**
 * \brief Initializes the RTC module with given configurations.
 *
 * Initializes the module, setting up all given configurations to provide
 * the desired functionality of the RTC.
 *
 * \param[in] config  Pointer to the configuration structure.
 */
void rtc_calendar_init(
		const struct rtc_calendar_config *const config)
{
	/* Sanity check. */
	Assert(config);

	/* Set up GCLK */
	struct system_gclk_chan_config gclk_chan_conf;
	system_gclk_chan_get_config_defaults(&gclk_chan_conf);
	gclk_chan_conf.source_generator = GCLK_GENERATOR_2;
	system_gclk_chan_set_config(RTC_GCLK_ID, &gclk_chan_conf);
	system_gclk_chan_enable(RTC_GCLK_ID);

	/* Reset module to hardware defaults. */
	rtc_calendar_reset();

	/* Save conf_struct internally for continued use. */
	_rtc_dev.clock_24h           = config->clock_24h;
	_rtc_dev.continuously_update = config->continuously_update;
	_rtc_dev.year_init_value     = config->year_init_value;

	/* Set config. */
	_rtc_calendar_set_config(config);
}

/**
 * \brief Swaps between 12h and 24h clock mode.
 *
 * Swaps the current RTC time mode.
 * - If currently in 12h mode, it will swap to 24h.
 * - If currently in 24h mode, it will swap to 12h.
 *
 * \note This will not change setting in user's configuration structure.
 */
void rtc_calendar_swap_time_mode(void)
{
	/* Initialize module pointer. */
	Rtc *const rtc_module = RTC;

	/* Initialize time structure. */
	struct rtc_calendar_time time;
	struct rtc_calendar_alarm_time alarm;

	/* Get current time. */
	rtc_calendar_get_time(&time);

	/* Check current mode. */
	if (_rtc_dev.clock_24h) {
		/* Set pm flag. */
		time.pm = (uint8_t)(time.hour / 12);

		/* Set 12h clock hour value. */
		time.hour = time.hour % 12;
		if (time.hour == 0) {
			time.hour = 12;
		}

		/* Update alarms */
		for (uint8_t i = 0; i < RTC_NUM_OF_ALARMS; i++) {
			rtc_calendar_get_alarm(&alarm, (enum rtc_calendar_alarm)i);
			alarm.time.pm = (uint8_t)(alarm.time.hour / 12);
			alarm.time.hour = alarm.time.hour % 12;
			if (alarm.time.hour == 0) {
				alarm.time.hour = 12;
			}
			_rtc_dev.clock_24h = false;
			rtc_calendar_set_alarm(&alarm, (enum rtc_calendar_alarm)i);
			_rtc_dev.clock_24h = true;
		}

		/* Change value in configuration structure. */
		_rtc_dev.clock_24h = false;
	} else {
		/* Set hour value based on pm flag. */
		if (time.pm == 1) {
			time.hour = time.hour + 12;

			time.pm = 0;
		} else if (time.hour == 12) {
			time.hour = 0;
		}

		/* Update alarms */
		for (uint8_t i = 0; i < RTC_NUM_OF_ALARMS; i++) {
			rtc_calendar_get_alarm(&alarm, (enum rtc_calendar_alarm)i);
			if (alarm.time.pm == 1) {
				alarm.time.hour = alarm.time.hour + 12;
				alarm.time.pm = 0;
				_rtc_dev.clock_24h = true;
				rtc_calendar_set_alarm(&alarm, (enum rtc_calendar_alarm)i);
				_rtc_dev.clock_24h = false;
			} else if (alarm.time.hour == 12) {
				alarm.time.hour = 0;
			}
		}

		/* Change value in configuration structure. */
		_rtc_dev.clock_24h = true;
	}

	/* Disable RTC so new configuration can be set. */
	rtc_calendar_disable();

	/* Toggle mode. */
	rtc_module->MODE2.CTRL.reg ^= RTC_MODE2_CTRL_CLKREP;

	/* Enable RTC. */
	rtc_calendar_enable();

	/* Set new time format in CLOCK register. */
	rtc_calendar_set_time(&time);
}

/**
 * \brief Set the current calendar time to desired time.
 *
 * Sets the time provided to the calendar.
 *
 * \param[in] time  The time to set in the calendar.
 */
void rtc_calendar_set_time(
		const struct rtc_calendar_time *const time)
{
	/* Initialize module pointer. */
	Rtc *const rtc_module = RTC;

	uint32_t register_value = _rtc_calendar_time_to_register_value(time);

	while (rtc_calendar_is_syncing()) {
		/* Wait for synchronization */
	}

	/* Write value to register. */
	rtc_module->MODE2.CLOCK.reg = register_value;
}

/**
 * \brief Get the current calendar value.
 *
 * Retrieves the current time of the calendar.
 *
 * \param[out] time  Pointer to value that will be filled with current time.
 */
void rtc_calendar_get_time(
		struct rtc_calendar_time *const time)
{
	/* Initialize module pointer. */
	Rtc *const rtc_module = RTC;

	/* Change of read method based on value of continuously_update value in
	 * the configuration structure. */
	if (!(_rtc_dev.continuously_update)) {
		/* Request read on CLOCK register. */
		rtc_module->MODE2.READREQ.reg = RTC_READREQ_RREQ;

		while (rtc_calendar_is_syncing()) {
			/* Wait for synchronization */
		}
	}

	/* Read value. */
	uint32_t register_value = rtc_module->MODE2.CLOCK.reg;

	/* Convert value to time structure. */
	_rtc_calendar_register_value_to_time(register_value, time);
}

/**
 * \brief Set the alarm time for the specified alarm.
 *
 * Sets the time and mask specified to the requested alarm.
 *
 * \param[in] alarm        The alarm struct to set the alarm with.
 * \param[in] alarm_index  The index of the alarm to set.
 *
 * \return Status of setting alarm.
 * \retval STATUS_OK               If alarm was set correctly.
 * \retval STATUS_ERR_INVALID_ARG  If invalid argument(s) were provided.
 */
enum status_code rtc_calendar_set_alarm(
		const struct rtc_calendar_alarm_time *const alarm,
		const enum rtc_calendar_alarm alarm_index)
{
	/* Initialize module pointer. */
	Rtc *const rtc_module = RTC;

	/* Sanity check. */
	if ((uint32_t)alarm_index > RTC_NUM_OF_ALARMS) {
		return STATUS_ERR_INVALID_ARG;
	}

	/* Get register_value from time. */
	uint32_t register_value = _rtc_calendar_time_to_register_value(&(alarm->time));

	while (rtc_calendar_is_syncing()) {
		/* Wait for synchronization */
	}

	/* Set alarm value. */
	rtc_module->MODE2.Mode2Alarm[alarm_index].ALARM.reg = register_value;

	/* Set alarm mask */
	rtc_module->MODE2.Mode2Alarm[alarm_index].MASK.reg = alarm->mask;

	return STATUS_OK;
}

/**
 * \brief Get the current alarm time of specified alarm.
 *
 * Retrieves the current alarm time for the alarm specified.
 *
 * \param[out] alarm  Pointer to the struct that will be filled with alarm
 *                    time and mask of the specified alarm.
 * \param[in] alarm_index  Index of alarm to get alarm time from.
 *
 * \return Status of getting alarm.
 * \retval STATUS_OK               If alarm was read correctly.
 * \retval STATUS_ERR_INVALID_ARG  If invalid argument(s) were provided.
 */
enum status_code rtc_calendar_get_alarm(
		struct rtc_calendar_alarm_time *const alarm,
		const enum rtc_calendar_alarm alarm_index)
{
	/* Initialize module pointer. */
	Rtc *const rtc_module = RTC;

	/* Sanity check. */
	if ((uint32_t)alarm_index > RTC_NUM_OF_ALARMS) {
		return STATUS_ERR_INVALID_ARG;
	}

	/* Read alarm value. */
	uint32_t register_value =
			rtc_module->MODE2.Mode2Alarm[alarm_index].ALARM.reg;

	/* Convert to time structure. */
	_rtc_calendar_register_value_to_time(register_value, &(alarm->time));

	/* Read alarm mask */
	alarm->mask = (enum rtc_calendar_alarm_mask)rtc_module->MODE2.Mode2Alarm[alarm_index].MASK.reg;

	return STATUS_OK;
}

/**
 * \brief Calibrate for too-slow or too-fast oscillator.
 *
 * When used, the RTC will compensate for an inaccurate oscillator. The
 * RTC module will add or subtract cycles from the RTC prescaler to adjust the
 * frequency in approximately 1 PPM steps. The provided correction value should
 * be between -127 and 127, allowing for a maximum 127 PPM correction in either
 * direction.
 *
 * If no correction is needed, set value to zero.
 *
 * \note Can only be used when the RTC is operated at 1Hz.
 *
 * \param[in] value Between -127 and 127 used for the correction.
 *
 * \return Status of the calibration procedure.
 * \retval STATUS_OK               If calibration was done correctly.
 * \retval STATUS_ERR_INVALID_ARG  If invalid argument(s) were provided.
 */
enum status_code rtc_calendar_frequency_correction(
		const int8_t value)
{
	/* Initialize module pointer. */
	Rtc *const rtc_module = RTC;

	/* Check if valid argument. */
	if (abs(value) > 0x7F) {
		/* Value bigger than allowed, return invalid argument. */
		return STATUS_ERR_INVALID_ARG;
	}

	uint32_t new_correction_value;

	/* Load the new correction value as a positive value, sign added later */
	new_correction_value = abs(value);

	/* Convert to positive value and adjust register sign bit. */
	if (value < 0) {
		new_correction_value |= RTC_FREQCORR_SIGN;
	}

	while (rtc_calendar_is_syncing()) {
		/* Wait for synchronization */
	}

	/* Set value. */
	rtc_module->MODE2.FREQCORR.reg = new_correction_value;

	return STATUS_OK;
}
