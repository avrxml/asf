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

#include "adc.h"
#include "adc_sensors.h"

#define NTC_SENSOR_MAX_SAMPLES   4
#define LIGHT_SENSOR_MAX_SAMPLES 4

//! bool to hold the data ready flag
bool ntc_sensor_data_ready = false;
bool light_sensor_data_ready = false;
adc_result_t ntc_sensor_sample = 0;
adc_result_t light_sensor_sample = 0;

/**
 * \brief Check of there is NTC data ready to be read
 *
 * When data is ready to be read this function will return true, and assume
 * that the data is going to be read so it sets the ready flag to false.
 *
 * \retval true if the NTC value is ready to be read
 * \retval false if data is not ready yet
 */
bool ntc_data_is_ready(void)
{
	irqflags_t irqflags;
	/* We will need to save and turn of global interrupts to make sure that we
	read the latest NTC value and not the next one if a conversation finish
	before one have time read the data. */
	irqflags = cpu_irq_save();
	if (ntc_sensor_data_ready) {
		ntc_sensor_data_ready = false;
		cpu_irq_restore(irqflags);
		return true;
	} else {
		cpu_irq_restore(irqflags);
		return false;
	}
}

bool lightsensor_data_is_ready(void)
{
	irqflags_t irqflags;
	/* We will need to save and turn of global interrupts to make sure that we
	read the latest NTC value and not the next one if a conversation finish
	before one have time read the data. */
	irqflags = cpu_irq_save();
	if (light_sensor_data_ready) {
		light_sensor_data_ready = false;
		cpu_irq_restore(irqflags);
		return true;
	} else {
		cpu_irq_restore(irqflags);
		return false;
	}
}

/**
 * \brief Callback for the ADC conversion complete
 *
 * The ADC module will call this function on a conversion complete.
 *
 * \param adc the ADC from which the interrupt came
 * \param ch_mask the ch_mask that produced the interrupt
 * \param result the result from the ADC
 */
void adc_handler(ADC_t *adc, uint8_t ch_mask, adc_result_t result)
{
	static uint8_t light_sensor_samples = 0;
	static uint8_t ntc_sensor_samples = 0;

	// Lightsensor on channel 0 , NTC on channel 1
	if (ch_mask == ADC_CH0) {
		light_sensor_samples++;
		if (light_sensor_samples == 1) {
			light_sensor_sample = result;
			light_sensor_data_ready = false;
		} else {
			light_sensor_sample += result;
			light_sensor_sample >>= 1;
		}
		if (light_sensor_samples == LIGHT_SENSOR_MAX_SAMPLES) {
			light_sensor_samples = 0;
			light_sensor_data_ready = true;
		} else {
			lightsensor_measure();
		}
	} else if (ch_mask == ADC_CH1) {
		ntc_sensor_samples++;
		if (ntc_sensor_samples == 1) {
			ntc_sensor_sample = result;
			ntc_sensor_data_ready = false;
		} else {
			ntc_sensor_sample += result;
			ntc_sensor_sample >>= 1;
		}
		if (ntc_sensor_samples == NTC_SENSOR_MAX_SAMPLES) {
			ntc_sensor_samples = 0;
			ntc_sensor_data_ready = true;
		} else {
			ntc_measure();
		}
	}
}

/**
 * \brief Initialize ADC channels for NTC and lightsensor
 *
 * This will set up the ADC for reading the NTC and light sensor
 * present on the A3BU-Xplained board.
 */
void adc_sensors_init(void)
{
	struct adc_config adc_conf;
	struct adc_channel_config adc_ch_conf;

	/* Clear the ADC configuration structs */
	adc_read_configuration(&ADCA, &adc_conf);
	adcch_read_configuration(&ADCA, ADC_CH0, &adc_ch_conf);

	/* configure the ADCA module:
	- signed, 12-bit resolution
	- VCC / 1.6 reference
	- 31 kHz max clock rate
	- manual conversion triggering
	- callback function
	*/
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12,
			ADC_REF_VCC);
	adc_set_clock_rate(&adc_conf, 125000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_write_configuration(&ADCA, &adc_conf);
	adc_set_callback(&ADCA, &adc_handler);

	/* Configure ADC A channel 0 (lightsensor):
	 * - single-ended measurement
	 * - interrupt flag set on completed conversion
	 * - interrupts enabled
	 */
	adcch_set_input(&adc_ch_conf, ADCCH_POS_PIN0, ADCCH_NEG_NONE, 1);
	adcch_set_interrupt_mode(&adc_ch_conf, ADCCH_MODE_COMPLETE);
	adcch_enable_interrupt(&adc_ch_conf);
	adcch_write_configuration(&ADCA, ADC_CH0, &adc_ch_conf);

	/* Configure ADC A channel 1 (NTC sensor):
	 * - single-ended measurement
	 * - interrupt flag set on completed conversion
	 * - interrupts enabled
	 */
	adcch_set_input(&adc_ch_conf, ADCCH_POS_PIN1, ADCCH_NEG_NONE, 1);
	adcch_write_configuration(&ADCA, ADC_CH1, &adc_ch_conf);

	adc_enable(&ADCA);
}

/**
 * \brief Read the NTC value from the ADC
 *
 * This will read the ADC value of the channel and pin connected to the NTC
 * on the A3BU-Xplained.
 *
 * \retval the raw ADC value from the current ntc_sensor_sample
 */
int16_t ntc_get_raw_value(void)
{
	return ntc_sensor_sample;
}

/**
 * \brief Read the actual temperature from the NTC
 *
 * This function returns the temperature in Celsius by piecewise linearisation
 * of the complex transfer function between NTC resistance and ADC codes. The
 * accuracy of the temperature is somewhat limited.
 *
 * \note The result is inaccurate for temperatures outside 5-45 degrees.
 *
 * \retval the temperature in Celsius
 */
int8_t ntc_get_temperature(void)
{
	int8_t retval = 0;
	float ntc_sample = ntc_sensor_sample;
	if (ntc_sensor_sample > 697) {
		retval = (int8_t)((-0.0295 * ntc_sample) + 40.5);
	} if (ntc_sensor_sample > 420) {
		retval = (int8_t)((-0.0474 * ntc_sample) + 53.3);
	} else {
		retval = (int8_t)((-0.0777 * ntc_sample) + 65.1);
	}
	return retval;
}

/**
 * \brief Read the lightsensor value from the ADC
 *
 * This will read the ADC value of the channel and pin connected to the
 * lightsensor on the A3BU-Xplained.
 *
 * \retval the raw ADC value from the current ntc_sensor_sample
 */
int16_t lightsensor_get_raw_value(void)
{
	return light_sensor_sample;
}

