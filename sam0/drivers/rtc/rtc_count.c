/**
 * \file
 *
 * \brief SAM D20 RTC Driver (Count Mode)
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
#include "rtc_count.h"
#include <gclk.h>

#if !defined(__DOXYGEN__)
volatile struct _rtc_device _rtc_dev;
#endif

/**
 * \brief Resets the RTC module.
 * Resets the RTC to hardware defaults.
 */
void rtc_count_reset(void)
{
	/* Initialize module pointer. */
	Rtc *const rtc_module = RTC;

	/* Disable module before reset. */
	rtc_count_disable();

#if RTC_COUNT_ASYNC == true
	_rtc_dev.registered_callback = 0;
	_rtc_dev.enabled_callback    = 0;
#endif

	while (rtc_count_is_syncing()) {
		/* Wait for synchronization */
	}

	/* Initiate software reset. */
	rtc_module->MODE0.CTRL.reg |= RTC_MODE0_CTRL_SWRST;
}

/**
 * \internal Applies the given configuration.
 *
 * Sets the configurations given from the configuration structure to the
 * hardware module.
 *
 * \param[in] config  Pointer to the configuration structure.
 *
 * \return Status of the configuration procedure.
 * \retval STATUS_OK               RTC configurations was set successfully.
 * \retval STATUS_ERR_INVALID_ARG  If invalid argument(s) were given.
 */
static enum status_code _rtc_count_set_config(
		const struct rtc_count_config *const config)
{
	/* Initialize module pointer. */
	Rtc *const rtc_module = RTC;

	rtc_module->MODE0.CTRL.reg = RTC_MODE0_CTRL_MODE(0) | config->prescaler;

	/* Set mode and clear on match if applicable. */
	switch (config->mode) {
		case RTC_COUNT_MODE_32BIT:
			/* Set 32bit mode and clear on match if applicable. */
			rtc_module->MODE0.CTRL.reg |= RTC_MODE0_CTRL_MODE(0);

			/* Check if clear on compare match should be set. */
			if (config->clear_on_match) {
				/* Set clear on match. */
				rtc_module->MODE0.CTRL.reg |= RTC_MODE0_CTRL_MATCHCLR;
			}
			/* Set compare values. */
			for (uint8_t i = 0; i < RTC_NUM_OF_ALARMS; i++) {
				while (rtc_count_is_syncing()) {
					/* Wait for synchronization */
				}

				rtc_count_set_compare(config->compare_values[i],
						(enum rtc_count_compare)i);
			}
			break;

		case RTC_COUNT_MODE_16BIT:
			/* Set 16bit mode. */
			rtc_module->MODE0.CTRL.reg |= RTC_MODE0_CTRL_MODE(1);

			/* Check if match on clear is set, and return invalid
			 * argument if set. */
			if (config->clear_on_match) {
				Assert(false);
				return STATUS_ERR_INVALID_ARG;
			}
			/* Set compare values. */
			for (uint8_t i = 0; i < RTC_NUM_OF_COMP16; i++) {
				while (rtc_count_is_syncing()) {
					/* Wait for synchronization */
				}

				rtc_count_set_compare(config->compare_values[i],
						(enum rtc_count_compare)i);
			}
			break;
		default:
			Assert(false);
			return STATUS_ERR_INVALID_ARG;
	}

	/* Check to set continuously clock read update mode. */
	if (config->continuously_update) {
		/* Set continuously mode. */
		rtc_module->MODE0.READREQ.reg |= RTC_READREQ_RCONT;
	}

	/* Return status OK if everything was configured. */
	return STATUS_OK;
}

/**
 * \brief Initializes the RTC module with given configurations.
 *
 * Initializes the module, setting up all given configurations to provide
 * the desired functionality of the RTC.
 *
 * \param[in] config  Pointer to the configuration structure.
 *
 * \return Status of the initialization procedure.
 * \retval STATUS_OK               If the initialization was run stressfully.
 * \retval STATUS_ERR_INVALID_ARG  If invalid argument(s) were given.
 */
enum status_code rtc_count_init(
		const struct rtc_count_config *const config)
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
	rtc_count_reset();

	/* Save conf_struct internally for continued use. */
	_rtc_dev.mode                = config->mode;
	_rtc_dev.continuously_update = config->continuously_update;

	/* Set config and return status. */
	return _rtc_count_set_config(config);
}

/**
 * \brief Set the current count value to desired value.
 *
 * Sets the value of the counter to the specified value.
 *
 * \param[in] count_value  The value to be set in count register.
 *
 * \return Status of setting the register.
 * \retval STATUS_OK               If everything was executed correctly.
 * \retval STATUS_ERR_INVALID_ARG  If invalid argument(s) were provided.
 */
enum status_code rtc_count_set_count(
		const uint32_t count_value)
{
	/* Initialize module pointer. */
	Rtc *const rtc_module = RTC;

	while (rtc_count_is_syncing()) {
		/* Wait for synchronization */
	}

	/* Set count according to mode */
	switch(_rtc_dev.mode){
		case RTC_COUNT_MODE_32BIT:
			/* Write value to register. */
			rtc_module->MODE0.COUNT.reg = count_value;

			break;

		case RTC_COUNT_MODE_16BIT:
			/* Check if 16 bit value is provided. */
			if(count_value > 0xffff){
				return STATUS_ERR_INVALID_ARG;
			}

			/* Write value to register. */
			rtc_module->MODE1.COUNT.reg = (uint32_t)count_value;

			break;

		default:
			Assert(false);
			return STATUS_ERR_INVALID_ARG;
	}
	return STATUS_OK;
}

/**
 * \brief Get the current count value.
 *
 * Returns the current count value.
 *
 * \return The current counter value as a 32 bit unsigned integer.
 */
uint32_t rtc_count_get_count(void)
{
	/* Initialize module pointer. */
	Rtc *const rtc_module = RTC;

	/* Initialize return value. */
	uint32_t ret_val;

	/* Change of read method based on value of continuously_update value in
	 * the configuration structure. */
	if(!(_rtc_dev.continuously_update)){
		/* Request read on count register. */
		rtc_module->MODE0.READREQ.reg = RTC_READREQ_RCONT;

		while (rtc_count_is_syncing()) {
			/* Wait for synchronization */
		}
	}

	/* Read value based on mode. */
	switch (_rtc_dev.mode) {
		case RTC_COUNT_MODE_32BIT:
			/* Return count value in 32 bit mode. */
			ret_val = rtc_module->MODE0.COUNT.reg;

			break;

		case RTC_COUNT_MODE_16BIT:
			/* Return count value in 16 bit mode. */
			ret_val = (uint32_t)rtc_module->MODE1.COUNT.reg;

			break;

		default:
			Assert(false);
			/* Counter not initialized. Assume counter value 0.*/
			ret_val = 0;
			break;
	}

	return ret_val;
}

/**
 * \brief Set the compare value for the specified compare.
 *
 * Sets the value specified by the implementer to the requested compare.
 *
 * \note Compare 4 and 5 are only available in 16 bit mode.
 *
 * \param[in] comp_value  The value to be written to the compare.
 * \param[in] comp_index  Index of the compare to set.
 *
 * \return Status indicating if compare was successfully set.
 * \retval STATUS_OK               If compare was successfully set.
 * \retval STATUS_ERR_INVALID_ARG  If invalid argument(s) were provided.
 * \retval STATUS_ERR_BAD_FORMAT   If the module was not initialized in a mode.
 */
enum status_code rtc_count_set_compare(
		const uint32_t comp_value,
		const enum rtc_count_compare comp_index)
{
	/* Initialize module pointer. */
	Rtc *const rtc_module = RTC;

	while (rtc_count_is_syncing()) {
		/* Wait for synchronization */
	}

	/* Set compare values based on operation mode. */
	switch (_rtc_dev.mode) {
		case RTC_COUNT_MODE_32BIT:
			/* Check sanity of comp_index. */
			if ((uint32_t)comp_index > RTC_NUM_OF_ALARMS) {
				return STATUS_ERR_INVALID_ARG;
			}

			/* Set compare value for COMP. */
			rtc_module->MODE0.COMP[comp_index].reg = comp_value;

			break;

		case RTC_COUNT_MODE_16BIT:
			/* Check sanity of comp_index. */
			if ((uint32_t)comp_index > RTC_NUM_OF_COMP16) {
				return STATUS_ERR_INVALID_ARG;
			}

			/* Check that 16 bit value is provided. */
			if (comp_value > 0xffff) {
				Assert(false);
				return STATUS_ERR_INVALID_ARG;
			}

			/* Set compare value for COMP. */
			rtc_module->MODE1.COMP[comp_index].reg = comp_value & 0xffff;

			break;

		default:
			Assert(false);
			return STATUS_ERR_BAD_FORMAT;
	}

	/* Return status if everything is OK. */
	return STATUS_OK;
}

/**
 * \brief Get the current compare value of specified compare.
 *
 * Retrieves the current value of the specified compare.
 *
 * \note Compare 4 and 5 are only available in 16 bit mode.
 *
 * \param[out] comp_value  Pointer to 32 bit integer that will be populated with
 *                         the current compare value.
 * \param[in]  comp_index  Index of compare to check.
 *
 * \return Status of the reading procedure.
 * \retval STATUS_OK               If the value was read correctly.
 * \retval STATUS_ERR_INVALID_ARG  If invalid argument(s) were provided.
 * \retval STATUS_ERR_BAD_FORMAT   If the module was not initialized in a mode.
 */
enum status_code rtc_count_get_compare(
		uint32_t *const comp_value,
		const enum rtc_count_compare comp_index)
{
	/* Initialize module pointer. */
	Rtc *const rtc_module = RTC;

	switch (_rtc_dev.mode) {
		case RTC_COUNT_MODE_32BIT:
			/* Check sanity of comp_index. */
			if ((uint32_t)comp_index > RTC_NUM_OF_ALARMS) {
				return STATUS_ERR_INVALID_ARG;
			}

			/* Get compare value for COMP. */
			*comp_value = rtc_module->MODE0.COMP[comp_index].reg;

			break;

		case RTC_COUNT_MODE_16BIT:
			/* Check sanity of comp_index. */
			if ((uint32_t)comp_index > RTC_NUM_OF_COMP16) {
				return STATUS_ERR_INVALID_ARG;
			}

			/* Get compare value for COMP. */
			*comp_value = (uint32_t)rtc_module->MODE1.COMP[comp_index].reg;

			break;

		default:
			Assert(false);
			return STATUS_ERR_BAD_FORMAT;
	}
	/* Return status showing everything is OK. */
	return STATUS_OK;
}

/**
 * \brief Retrieves the value of period.
 *
 * Retrieves the value of the period for the 16 bit mode counter.
 *
 * \note Only available in 16 bit mode.
 *
 * \param[out] period_value  Pointer to value for return argument.
 *
 * \return Status of getting the period value.
 * \retval STATUS_OK                   If the period value was read correctly.
 * \retval STATUS_ERR_UNSUPPORTED_DEV  If incorrect mode was set.
 */
enum status_code rtc_count_get_period(
		uint16_t *const period_value)
{
	/* Initialize module pointer. */
	Rtc *const rtc_module = RTC;

	/* Check that correct mode is set. */
	if (_rtc_dev.mode != RTC_COUNT_MODE_16BIT) {
		return STATUS_ERR_UNSUPPORTED_DEV;
	}

	/* Returns the value. */
	*period_value = rtc_module->MODE1.PER.reg;

	return STATUS_OK;
}

/**
 * \brief Set the given value to the period.
 *
 * Sets the given value to the period.
 *
 * \note Only available in 16 bit mode.
 *
 * \param[in] period_value  The value to set to the period.
 *
 * \return Status of setting the period value.
 * \retval STATUS_OK                   If the period was set correctly.
 * \retval STATUS_ERR_UNSUPPORTED_DEV  If module is not operated in 16 bit mode.
 */
enum status_code rtc_count_set_period(
		const uint16_t period_value)
{
	/* Initialize module pointer. */
	Rtc *const rtc_module = RTC;

	/* Check that correct mode is set. */
	if (_rtc_dev.mode != RTC_COUNT_MODE_16BIT) {
		return STATUS_ERR_UNSUPPORTED_DEV;
	}

	while (rtc_count_is_syncing()) {
		/* Wait for synchronization */
	}

	/* Write value to register. */
	rtc_module->MODE1.PER.reg = period_value;

	return STATUS_OK;
}

/**
 * \brief Check if RTC compare match has occurred.
 *
 * Checks the compare flag to see if a match has occurred. The compare flag is
 * set when there is a compare match between counter and the compare.
 *
 * \note Compare 4 and 5 are only available in 16 bit mode.
 *
 * \param[in] comp_index  Index of compare to check current flag.
 */
bool rtc_count_is_compare_match(
		const enum rtc_count_compare comp_index)
{
	/* Initialize module pointer. */
	Rtc *const rtc_module = RTC;

	/* Check sanity. */
	switch (_rtc_dev.mode) {
		case RTC_COUNT_MODE_32BIT:
			/* Check sanity for 32 bit mode. */
			if (comp_index > RTC_NUM_OF_ALARMS) {
				return false;
			}

			break;

		case RTC_COUNT_MODE_16BIT:
			/* Check sanity for 16 bit mode. */
			if (comp_index > RTC_NUM_OF_COMP16) {
				return false;
			}

			break;

		default:
			Assert(false);
			return false;
	}

	/* Set status of INTFLAG as return argument. */
	return (rtc_module->MODE0.INTFLAG.reg & (1 << comp_index));
}

/**
 * \brief Clears RTC compare match flag.
 *
 * Clears the compare flag. The compare flag is set when there is a compare
 * match between the counter and the compare.
 *
 * \note Compare 4 and 5 are only available in 16 bit mode.
 *
 * \param[in] comp_index  Index of compare to check current flag.
 *
 * \return Status indicating if flag was successfully cleared.
 * \retval STATUS_OK               If flag was successfully cleared.
 * \retval STATUS_ERR_INVALID_ARG  If invalid argument(s) were provided.
 * \retval STATUS_ERR_BAD_FORMAT   If the module was not initialized in a mode.
 */
enum status_code rtc_count_clear_compare_match(
		const enum rtc_count_compare comp_index)
{
	/* Initialize module pointer. */
	Rtc *const rtc_module = RTC;

	/* Check sanity. */
	switch (_rtc_dev.mode){
		case RTC_COUNT_MODE_32BIT:
			/* Check sanity for 32 bit mode. */
			if (comp_index > RTC_NUM_OF_ALARMS) {
				return STATUS_ERR_INVALID_ARG;
			}

			break;

		case RTC_COUNT_MODE_16BIT:
			/* Check sanity for 16 bit mode. */
			if (comp_index > RTC_NUM_OF_COMP16) {
				return STATUS_ERR_INVALID_ARG;
			}

			break;

		default:
			Assert(false);
			return STATUS_ERR_BAD_FORMAT;
	}

	/* Clear INTFLAG. */
	rtc_module->MODE0.INTFLAG.reg = RTC_MODE1_INTFLAG_CMP(1 << comp_index);

	return STATUS_OK;
}

/**
 * \brief Calibrate for too-slow or too-fast oscillator.
 *
 * When used, the RTC will compensate for an inaccurate oscillator. The
 * RTC module will add or subtract cycles from the RTC prescaler to adjust the
 * frequency in approximately 1 PPM steps. The provided correction value should
 * be between 0 and 127, allowing for a maximum 127 PPM correction.
 *
 * If no correction is needed, set value to zero.
 *
 * \note Can only be used when the RTC is operated in 1Hz.
 *
 * \param[in] value  Ranging from -127 to 127 used for the correction.
 *
 * \return Status of the calibration procedure.
 * \retval STATUS_OK               If calibration was executed correctly.
 * \retval STATUS_ERR_INVALID_ARG  If invalid argument(s) were provided.
 */
enum status_code rtc_count_frequency_correction(
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

	while (rtc_count_is_syncing()) {
		/* Wait for synchronization */
	}

	/* Set value. */
	rtc_module->MODE0.FREQCORR.reg = new_correction_value;

	return STATUS_OK;
}

