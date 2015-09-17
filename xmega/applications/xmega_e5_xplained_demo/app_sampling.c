/**
 * \file
 *
 * \brief Task to manage the sensor sampling
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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

/**
 * \ingroup sampling_task_group
 * \defgroup sampling_task_group_implementation Sampling task implementation
 * @{
 */

/* ! store the QDec configuration */
static qdec_config_t qdec_config;

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
/* ! Table of ADC conversions to be performed to access the on-board sensors */
static enum adcch_positive_input adc_conv[] = {
	LIGHT_SENSOR_ADC_INPUT
};
/* ! Index to navigate within the ADC conversion table */
static uint8_t adc_mux_index = 0;
/* ! ADC channel configuration */
static struct adc_channel_config adcch_conf;
/* ! Last light sensor value */
static adc_result_t light;
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
	LED_On(LED1_GPIO);
	app_sampling_start_next_conversion();
}

/**
 * \brief Start the next convertion according to \ref adc_mux_index index
 */
static void app_sampling_start_next_conversion(void)
{
	/* Setup ADC to start next one */
	adcch_set_input(&adcch_conf, adc_conv[adc_mux_index],
			ADCCH_NEG_INTERNAL_GND, 0);
	adcch_write_configuration(&LIGHT_SENSOR_ADC_MODULE, ADC_CH0,
			&adcch_conf);
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
	/* Store ADC convertion result for the coresponding on-board sensor */
	switch (adc_conv[adc_mux_index]) {
	case LIGHT_SENSOR_ADC_INPUT:
		light = 2048 - result;
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
	if (2 > fifo_get_free_size(&app_sampling_fifo_desc)) {
		return; /* Error */
	}

	fifo_push_uint16_nocheck(&app_sampling_fifo_desc, rtc_get_time());
	fifo_push_uint16_nocheck(&app_sampling_fifo_desc, (uint16_t)light);
	LED_Off(LED1_GPIO);

	/* Enable the next RTC alarm */
	app_sampling_rtc_run = true;
	rtc_set_alarm_relative(app_sampling_rate - 1);
}

/****************** PUBLIC FUNCTIONS **********************/

void app_sampling_init(void)
{
	/* QDec configuration */
	qdec_get_config_defaults(&qdec_config);
	qdec_config_phase_pins(&qdec_config, &PORTA, 6, false, 500);
	qdec_config_enable_rotary(&qdec_config);
	qdec_config_tc(&qdec_config, &TCC5);
	qdec_config_revolution(&qdec_config, 40);
	qdec_enabled(&qdec_config);

	/* ! ADC module configuration */
	struct adc_config adc_conf;

	/* Configure the ADC module:
	 * - signed, 12-bit results
	 * - VCC reference
	 * - 200 kHz maximum clock rate
	 * - manual conversion triggering
	 * - callback function
	 */
	adc_read_configuration(&LIGHT_SENSOR_ADC_MODULE, &adc_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12,
			ADC_REF_VCC);
	adc_set_clock_rate(&adc_conf, 200000);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_write_configuration(&LIGHT_SENSOR_ADC_MODULE, &adc_conf);
	adc_set_callback(&LIGHT_SENSOR_ADC_MODULE, &app_sampling_handler);
	adc_enable(&LIGHT_SENSOR_ADC_MODULE);

	/* Configure ADC A channel 0 for light and NTC sensors.
	 * - differantial measurement (V- linked on internal GND)
	 * - gain x0.5
	 * - interrupt flag set on completed conversion
	 * - interrupts enabled
	 */
	adcch_read_configuration(&LIGHT_SENSOR_ADC_MODULE, ADC_CH0,
			&adcch_conf);
	adcch_set_input(&adcch_conf, LIGHT_SENSOR_ADC_INPUT,
			ADCCH_NEG_INTERNAL_GND, 0);
	adcch_set_interrupt_mode(&adcch_conf, ADCCH_MODE_COMPLETE);
	adcch_enable_interrupt(&adcch_conf);
	adcch_write_configuration(&LIGHT_SENSOR_ADC_MODULE, ADC_CH0,
			&adcch_conf);

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
	gfx_mono_draw_string(DISPLAY_LIGHT_TEXT,
			DISPLAY_LIGHT_TEXT_POS_X,
			DISPLAY_LIGHT_TEXT_POS_Y,
			&sysfont);
	gfx_mono_draw_filled_rect(
			DISPLAY_LIGHT_PROBAR_START_POS_X,
			DISPLAY_LIGHT_PROBAR_START_POS_Y,
			DISPLAY_LIGHT_PROBAR_START_SIZE_X,
			DISPLAY_LIGHT_PROBAR_START_SIZE_Y,
			GFX_PIXEL_SET);
	gfx_mono_draw_filled_rect(
			DISPLAY_LIGHT_PROBAR_STOP_POS_X,
			DISPLAY_LIGHT_PROBAR_STOP_POS_Y,
			DISPLAY_LIGHT_PROBAR_STOP_SIZE_X,
			DISPLAY_LIGHT_PROBAR_STOP_SIZE_Y,
			GFX_PIXEL_SET);

	/* Start a RTC alarm immediatly */
	rtc_set_alarm_relative(0);
}

void app_sampling_task(void)
{
	static uint16_t time_stamp;
	static uint8_t time_last;
	uint8_t time_pos;
	adc_result_t light;
	char string[30];
	uint8_t light_bar_pos;
	irqflags_t flags;
	static uint16_t qdec_position_last = 2;
	uint16_t qdec_position;

	/* Manage frequency sample through Quadrature encoder */
	qdec_position = qdec_get_position(&qdec_config) / 2;
	if (qdec_position != qdec_position_last) {
		/* Quadrature encoder have changed */
		qdec_position_last = qdec_position;
		app_sampling_rate = (qdec_position_last + 1) * 2;
		flags = cpu_irq_save();
		if (app_sampling_rtc_run) {
			time_stamp = rtc_get_time();
			rtc_set_alarm_relative(app_sampling_rate - 1);
		}

		cpu_irq_restore(flags);
		app_sampling_display_rate();
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
	if (2 > fifo_get_used_size(&app_sampling_fifo_desc)) {
		return; /* Not enought data */
	}

	/* Get values */
	time_stamp = fifo_pull_uint16_nocheck(&app_sampling_fifo_desc);
	light = (adc_result_t)fifo_pull_uint16_nocheck(&app_sampling_fifo_desc);

	/* Display values through USART */
	sprintf(string, "%4u,%02us - %3d light\r\n",
			time_stamp / 4, (time_stamp % 4) * 25,
			light);
	printf("%s", string);

	/* Update light progress bar */
	light_bar_pos = ((uint32_t)light * DISPLAY_LIGHT_PROBAR_MAX_SIZE_X)
			/ 2048lu;
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
}

/* ! @} */
