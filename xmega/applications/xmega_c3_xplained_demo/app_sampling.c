/**
 * \file
 *
 * \brief Task to manage the sensor sampling
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
#include "app_sampling.h"
#include "app_touch.h"
#include "app_microsd.h"

/**
 * \ingroup sampling_task_group
 * \defgroup sampling_task_group_implementation Sampling task implementation
 * @{
 */

/**
 * @name Defines and variables to mamage the sampling rate
 * The sampling rate is the RTC alram frequency.
 * @{
 */
/* ! Sampling rate minimum */
#define APP_SAMPLING_RATE_MIN  (2)      /* Unit 250ms */
/* ! Sampling rate maximum */
#define APP_SAMPLING_RATE_MAX  (60 * 4)   /* Unit 250ms */

/* ! Signal if the RTC alarm has been requested */
static bool app_sampling_rtc_run = false;
/* ! Variable to store the current sampling rate */
static uint8_t app_sampling_rate = 2 * 4;
/* ! @} */

/**
 * @name Defines and variables to mamage the ADC conversion
 * @{
 */
/* ! ADC Delta V Value */
#define APP_ADC_DELTA_V  205

/* ! Table of ADC conversions to be performed to access the on-board sensors */
static enum adcch_positive_input adc_conv[] = {
	TEMPERATURE_SENSOR_ADC_INPUT,
	LIGHT_SENSOR_ADC_INPUT
};
/* ! Index to navigate within the ADC conversion table */
static uint8_t adc_mux_index = 0;
/* ! ADC module configuration */
struct adc_config adc_conf;
/* ! ADC channel configuration */
static struct adc_channel_config adcch_conf;
/* ! Last light sensor value */
static adc_result_t light;
/* ! Last light sensor value */
static int8_t temperature;

/**
 * \brief Table to store ADC codes matching a specific NTC temperature value
 *
 * This table gives the correspondance between the ADC code and the temperature.
 * Each elements of the table corresponds to an ADC code value.
 * The index in the table give the corresponding temperature (in°C) with
 * the following formula : Tempeature=index-5.
 * table[O] corresponds to -5°C temperature code
 * The following table gives the ADC code for signed single ended mode
 * with internal VCC/1.6 reference.
 */
static uint16_t temperature_code[] = {
	1640, 1594, 1548, 1503, 1458, 1414, 1370, 1327, 1285, 1243, 1202, 1162,
	1123, 1084, 1047, 1010, 975, 940, 907, 874, 842, 811, 781, 752, 724, 697,
	671, 645, 621, 597, 575, 553, 531, 511, 491, 472, 454, 437, 420, 404, 388,
	373, 359, 345, 332, 319, 307, 295, 283, 273, 262, 252, 243, 233, 225
};
/* ! @} */

/**
 * @name Defines and variables to mamage the FIFO
 * The FIFO is used to store RTC times, Temperature values, Light values.
 * @{
 */
/* ! FIFO size */
#define APP_SAMPLING_FIFO_SIZE (32)

/* ! FIFO buffer */
static uint16_t app_sampling_fifo_buffer[APP_SAMPLING_FIFO_SIZE];
/* ! FIFO handle */
static fifo_desc_t app_sampling_fifo_desc;
/* ! @} */

static void app_sampling_display_rate(void);
static void app_sampling_start(uint32_t time);
static void app_sampling_start_next_conversion(void);
static void app_sampling_handler(ADC_t *adc, uint8_t channel,
		adc_result_t result);

/**
 * \brief Displays the current sampling rate on OLED display
 */
static void app_sampling_display_rate(void)
{
	char string[20];
	sprintf(string, DISPLAY_SAMPLING_TEXT,
			app_sampling_rate / 4,
			(app_sampling_rate % 4) ? 5 : 0);
	gfx_mono_draw_string(string,
			DISPLAY_SAMPLING_TEXT_POS_X,
			DISPLAY_SAMPLING_TEXT_POS_Y,
			&sysfont);
}

/**
 * \brief Start the ADC conversions
 * It is the callback called by the RTC alarm.
 *
 * \param time   Current RTC time
 */
static void app_sampling_start(uint32_t time)
{
	UNUSED(time);
	app_sampling_rtc_run = false;
	LED_On(LED2_GPIO);
	app_sampling_start_next_conversion();
}

/**
 * \brief Start the next convertion according to \ref adc_mux_index index
 */
static void app_sampling_start_next_conversion(void)
{
	/* Setup ADC to start next one */
	adcch_set_input(&adcch_conf, adc_conv[adc_mux_index], ADCCH_NEG_NONE, 1);
	adcch_write_configuration(&LIGHT_SENSOR_ADC_MODULE, ADC_CH0, &adcch_conf);
	adc_start_conversion(&LIGHT_SENSOR_ADC_MODULE, ADC_CH0);
}

/**
 * \brief Callback function for ADC interrupts
 *
 * \param adc Pointer to ADC module.
 * \param channel ADC channel number.
 * \param result Conversion result from ADC channel.
 */
static void app_sampling_handler(ADC_t *adc, uint8_t channel,
		adc_result_t result)
{
	int8_t index = 0;

	/* Store ADC convertion result for the coresponding on-board sensor */
	switch (adc_conv[adc_mux_index]) {
	case LIGHT_SENSOR_ADC_INPUT:
		light = result;
		break;

	case TEMPERATURE_SENSOR_ADC_INPUT:

		/* Convert an ADC code from the NTC channel
		 * into temperature value */
		if (result > temperature_code[0]) {
			temperature = -5;
			break;
		}

		while (temperature_code[index++] > result) {
		}
		temperature = index - 1 - 5;
		break;

	default:
		break;
	}

	/* Parse table of ADC conversions */
	if (++adc_mux_index < sizeof(adc_conv)) {
		app_sampling_start_next_conversion();
		return;
	}

	/* End of all conversions */
	adc_mux_index = 0;

	/* Save values in FIFO */
	if (3 > fifo_get_free_size(&app_sampling_fifo_desc)) {
		return; /* Error */
	}

	fifo_push_uint16_nocheck(&app_sampling_fifo_desc, rtc_get_time());
	fifo_push_uint16_nocheck(&app_sampling_fifo_desc, (uint16_t)light);
	fifo_push_uint16_nocheck(&app_sampling_fifo_desc, (uint16_t)temperature);
	LED_Off(LED2_GPIO);

	/* Enable the next RTC alarm */
	app_sampling_rtc_run = true;
	rtc_set_alarm_relative(app_sampling_rate - 1);
}

/****************** PUBLIC FUNCTIONS **********************/

void app_sampling_init(void)
{
	/* Configure the ADC module:
	 * - signed, 12-bit results
	 * - VCC / 1.6 reference
	 * - 200 kHz maximum clock rate
	 * - manual conversion triggering
	 * - callback function
	 */
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12,
			ADC_REF_VCC);
	adc_set_clock_rate(&adc_conf, 200000);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_write_configuration(&LIGHT_SENSOR_ADC_MODULE, &adc_conf);
	adc_set_callback(&LIGHT_SENSOR_ADC_MODULE, &app_sampling_handler);
	adc_enable(&LIGHT_SENSOR_ADC_MODULE);

	/* Configure ADC A channel 0 for light and NTC sensors.
	 * - single-ended measurement
	 * - gain x1
	 * - interrupt flag set on completed conversion
	 * - interrupts enabled
	 */
	adcch_set_input(&adcch_conf, LIGHT_SENSOR_ADC_INPUT, ADCCH_NEG_NONE, 1);
	adcch_set_interrupt_mode(&adcch_conf, ADCCH_MODE_COMPLETE);
	adcch_enable_interrupt(&adcch_conf);
	adcch_write_configuration(&LIGHT_SENSOR_ADC_MODULE, ADC_CH0, &adcch_conf);

	fifo_init(&app_sampling_fifo_desc, app_sampling_fifo_buffer,
			APP_SAMPLING_FIFO_SIZE);
	rtc_set_callback(&app_sampling_start);

	/* Display background */
	gfx_mono_draw_line(DISPLAY_SAMPLING_TEXT_POS_X - 3,
			0,
			DISPLAY_SAMPLING_TEXT_POS_X - 3,
			32,
			GFX_PIXEL_SET);
	app_sampling_display_rate();
	gfx_mono_draw_circle(DISPLAY_TEMP_TEXT_POS_X,
			DISPLAY_TEMP_TEXT_POS_Y,
			1,
			GFX_PIXEL_SET,
			GFX_WHOLE);
	gfx_mono_draw_string(DISPLAY_TEMP_TEXT,
			DISPLAY_TEMP_TEXT_POS_X + 2,
			DISPLAY_TEMP_TEXT_POS_Y,
			&sysfont);
	gfx_mono_draw_string(DISPLAY_LIGHT_TEXT,
			DISPLAY_LIGHT_TEXT_POS_X,
			DISPLAY_LIGHT_TEXT_POS_Y,
			&sysfont);

	/* Start a RTC alarm immediatly */
	rtc_set_alarm_relative(0);
}

void app_sampling_task(void)
{
	static bool qtb0_last_state = false;
	static bool qtb1_last_state = false;
	static int8_t temperature_last = 0;
	bool qtb_state;
	static uint16_t time_stamp;
	static uint8_t time_last;
	uint8_t time_pos;
	adc_result_t light;
	int8_t temperature;
	char string[30];
	uint8_t light_bar_pos;
	irqflags_t flags;

	/* Manage frequency sample through QTouch buttons */
	qtb_state = app_touch_check_key_pressed(0);
	if (qtb_state != qtb0_last_state) {
		/* QTouch button have changed */
		qtb0_last_state = qtb_state;
		if (!qtb_state) {
			/* It is a press */
			if (app_sampling_rate < APP_SAMPLING_RATE_MAX) {
				app_sampling_rate += 2;
				flags = cpu_irq_save();
				if (app_sampling_rtc_run) {
					time_stamp = rtc_get_time();
					rtc_set_alarm_relative(
							app_sampling_rate - 1);
				}

				cpu_irq_restore(flags);
				app_sampling_display_rate();
			}

			LED_On(LED0_GPIO);
		} else {
			LED_Off(LED0_GPIO);
		}
	}

	qtb_state = app_touch_check_key_pressed(1);
	if (qtb_state != qtb1_last_state) {
		/* QTouch button have changed */
		qtb1_last_state = qtb_state;
		if (!qtb_state) {
			/* It is a press */
			if (app_sampling_rate > APP_SAMPLING_RATE_MIN) {
				app_sampling_rate -= 2;
				flags = cpu_irq_save();
				if (app_sampling_rtc_run) {
					time_stamp = rtc_get_time();
					rtc_set_alarm_relative(
							app_sampling_rate - 1);
				}

				cpu_irq_restore(flags);
				app_sampling_display_rate();
			}

			LED_On(LED1_GPIO);
		} else {
			LED_Off(LED1_GPIO);
		}
	}

	/* Update sampling progress bar */
	time_pos = ((rtc_get_time() - time_stamp) * 8) / app_sampling_rate;
	if (time_pos > 8) {
		time_pos = 8;
	}

	if (time_last > time_pos) {
		gfx_mono_draw_filled_circle(
				DISPLAY_SAMPLING_PROCIRCLE_POS_X,
				DISPLAY_SAMPLING_PROCIRCLE_POS_Y,
				DISPLAY_SAMPLING_PROCIRCLE_RADIUS,
				GFX_PIXEL_CLR,
				GFX_WHOLE);
	}

	if (time_last != time_pos) {
		gfx_mono_draw_filled_circle(
				DISPLAY_SAMPLING_PROCIRCLE_POS_X,
				DISPLAY_SAMPLING_PROCIRCLE_POS_Y,
				DISPLAY_SAMPLING_PROCIRCLE_RADIUS,
				GFX_PIXEL_SET,
				(1lu << time_pos) - 1);
	}

	time_last = time_pos;

	/* Manage FIFO about sensor data */
	if (3 > fifo_get_used_size(&app_sampling_fifo_desc)) {
		return; /* Not enought data */
	}

	/* Get values */
	time_stamp = fifo_pull_uint16_nocheck(&app_sampling_fifo_desc);
	light = (adc_result_t)fifo_pull_uint16_nocheck(&app_sampling_fifo_desc);
	temperature = (int8_t)fifo_pull_uint16_nocheck(&app_sampling_fifo_desc);

	if (app_usb_cdc_is_open() || app_microsd_is_ready()) {
		sprintf(string, "%4u,%02us - %3d light - %2dC\r\n",
				time_stamp / 4, (time_stamp % 4) * 25,
				light, temperature);

		if (app_usb_cdc_is_open()) {
			/* Display values through USB Device CDC */
			printf("%s", string);
		}

		if (app_microsd_is_ready()) {
			/* Write values on SD/MMC card */
			if (!app_microsd_write_file(string)) {
				printf("MicroSD write fail\r\n");
			}
		}
	}

	/* Update light progress bar */
	light_bar_pos = (light * DISPLAY_LIGHT_PROBAR_MAX_SIZE_X) / 256;
	if (light_bar_pos > DISPLAY_LIGHT_PROBAR_MAX_SIZE_X) {
		light_bar_pos = DISPLAY_LIGHT_PROBAR_MAX_SIZE_X;
	}

	gfx_mono_draw_filled_rect(DISPLAY_LIGHT_PROBAR_POS_X,
			DISPLAY_LIGHT_PROBAR_POS_Y,
			light_bar_pos,
			DISPLAY_LIGHT_PROBAR_SIZE_Y,
			GFX_PIXEL_SET);
	gfx_mono_draw_filled_rect(DISPLAY_LIGHT_PROBAR_POS_X + light_bar_pos,
			DISPLAY_LIGHT_PROBAR_POS_Y,
			DISPLAY_LIGHT_PROBAR_MAX_SIZE_X - light_bar_pos,
			DISPLAY_LIGHT_PROBAR_SIZE_Y,
			GFX_PIXEL_CLR);

	/* Update temperature value */
	if (temperature_last != temperature) {
		sprintf(string, "%2d", temperature);
		gfx_mono_draw_string(string,
				DISPLAY_TEMP_TEXT_VALUE_POS_X,
				DISPLAY_TEMP_TEXT_VALUE_POS_Y,
				&sysfont);
		temperature_last = temperature;
	}
}

/* ! @} */
