/**
 * \file
 *
 * \brief ADC Sensor interface
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

#include "asf.h"
#include "adc_sensors.h"
#include "usb_hid_com.h"

//! Table of ADC conversions to be performed to access the on-board sensors
static enum adcch_positive_input adc_conv[] = { TEMPERATURE_SENSOR_ADC_INPUT,
	LIGHT_SENSOR_ADC_INPUT, POTENTIOMETER_ADC_INPUT,  EXT_VIN_ADC_INPUT};

//! ADC module configuration
struct adc_config adc_conf;
//! ADC channel configuration
static struct adc_channel_config adcch_conf;
//! Index to navigate within the ADC conversion table
static uint8_t adc_mux_index = 0;
//! On-Board Sensors values
static uint16_t light, temperature, potentiometer, ext_voltage;


/**
 * \brief Table to store ADC codes matching a specific NTC temperature value
 *
 * This table gives the correspondence between the ADC code and the temperature.
 * Each elements of the table corresponds to an ADC code value.
 * The index in the table give the corresponding temperature (in°C) with
 * the following formula : Tempeature=index-5.
 * table[O] corresponds to -5°C temperature code
 * The following table gives the ADC code for VCC=3.3V
 */
uint16_t temperature_code[]={
	4047,4032,4017,4001,3984,3966,3948,3929,3910,3889,3868,3846,3823,3800,
	3775,3750,3724,3697,3669,3641,3612,3582,3551,3519,3486,3453,3419,3384,
	3348,3312,3275,3237,3198,3159,3119,3079,3038,2997,2955,2912,2869,2826,
	2782,2739,2694,2650,2605,2560,2516,2471,2426,2381,2336,2291,2246,2201};


/**
 * \brief Convert an ADC code from the NTC channel into temperature value
 *
 * \param adc_code The ADC input code read from the NTC
 * \return The temperature value in degree Celsius
 */
static int8_t code_to_temperature(uint16_t adc_code)
{
	int8_t index=0;
	if (adc_code>temperature_code[0]) {
		return (int8_t)(-5);
	} else {
		while(temperature_code[index++]>adc_code);
		return (int8_t)(index-1-5);
	}
}

int8_t adc_sensor_get_temperature(void)
{
	return temperature;
}

uint16_t adc_sensor_get_light(void)
{
	return light;
}

uint16_t adc_sensor_get_potentiometer(void)
{
	return potentiometer;
}

uint16_t adc_sensor_get_ext_voltage(void)
{
	return ext_voltage;
}

/**
 * \brief Callback function for ADC interrupts
 *
 * \param adc Pointer to ADC module.
 * \param channel ADC channel number.
 * \param result Conversion result from ADC channel.
 */
static void adc_handler(ADC_t *adc, uint8_t channel, adc_result_t result)
{
	uint32_t temp32;
	int8_t temps8;
	// Remove Delta V
	if (result>ADC_DELTA_V) {
		result -= ADC_DELTA_V;
	} else {
		result = 0;
	}
	//Store ADC conversion result for the corresponding on-board sensor
	switch (adc_conv[adc_mux_index]) {
		case LIGHT_SENSOR_ADC_INPUT:
			light = result;
			break;

		case TEMPERATURE_SENSOR_ADC_INPUT:
			temps8 = code_to_temperature (result);
			temperature = temps8;
			break;

		case POTENTIOMETER_ADC_INPUT:
			temp32 = ((uint32_t)result*1000UL)>>12;
			potentiometer = temp32;
			break;

		case EXT_VIN_ADC_INPUT:
			temp32 = ((uint32_t)result*8000UL)>>12;
			ext_voltage = temp32;
			break;
		default:
			break;
	}
	// Parse table of ADC conversions, setup ADC to start next one
	if (++adc_mux_index < sizeof(adc_conv)) {
		adc_disable(&BOARD_SENSOR_ADC_MODULE);
		adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF,
		ADC_RES_12, ADC_REF_BANDGAP);
		adc_write_configuration(&BOARD_SENSOR_ADC_MODULE, &adc_conf);
		adc_enable(&BOARD_SENSOR_ADC_MODULE);
		adcch_set_input(&adcch_conf, adc_conv[adc_mux_index],
		ADCCH_NEG_NONE, 1);
		adcch_write_configuration(&BOARD_SENSOR_ADC_MODULE, ADC_CH0,
		&adcch_conf);
		adc_start_conversion(&BOARD_SENSOR_ADC_MODULE, ADC_CH0);
	}

}

/**
 * \brief Initialize ADC channels for NTC and lightsensor
 *
 * This will set up the ADC for reading the NTC and light sensor
 * present on the board.
 */
void adc_sensors_init(void)
{
	/* Configure the ADC module:
	 * - unsigned, 12-bit results
	 * - 200 kHz maximum clock rate
	 * - manual conversion triggering
	 * - callback function
	 */
	adc_enable(&BOARD_SENSOR_ADC_MODULE);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12,
			ADC_REF_BANDGAP);
	adc_set_clock_rate(&adc_conf, ADC_CLOCK_HZ);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_enable_internal_input(&adc_conf, ADC_INT_TEMPSENSE);
	adc_write_configuration(&BOARD_SENSOR_ADC_MODULE, &adc_conf);
	adc_set_callback(&BOARD_SENSOR_ADC_MODULE, &adc_handler);

	// Configure ADC channel:
	adcch_set_input(&adcch_conf, LIGHT_SENSOR_ADC_INPUT, ADCCH_NEG_NONE, 1);
	adcch_set_interrupt_mode(&adcch_conf, ADCCH_MODE_COMPLETE);
	adcch_enable_interrupt(&adcch_conf);
	adcch_write_configuration(&BOARD_SENSOR_ADC_MODULE, ADC_CH0, &adcch_conf);
}


/**
 * \brief Stop the ADC sensors
 *
 */
void adc_sensors_stop(void)
{
	adc_disable(&BOARD_SENSOR_ADC_MODULE);
}

void adc_sensors_start_conversions(void)
{
	adc_mux_index = 0;
	adc_disable(&BOARD_SENSOR_ADC_MODULE);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF,
			ADC_RES_12, ADC_REF_BANDGAP);
	adc_write_configuration(&BOARD_SENSOR_ADC_MODULE, &adc_conf);
	adc_enable(&BOARD_SENSOR_ADC_MODULE);
	adcch_set_input(&adcch_conf, adc_conv[adc_mux_index],ADCCH_NEG_NONE, 1);
	adcch_write_configuration(&BOARD_SENSOR_ADC_MODULE, ADC_CH0, &adcch_conf);
	adc_start_conversion(&BOARD_SENSOR_ADC_MODULE, ADC_CH0);
}

