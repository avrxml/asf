/**
 * \file
 *
 * \brief XMEGA DAC Calibration application Main File
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

/**
 * \mainpage
 *
 * \section intro Introduction
 *  This Projects demonstrates:\n
 *  - How to calibrate the XMEGA DAC by using the internal ADC
 *  - Testing the calibrated DAC by outputting a sawtooth signal
 *
 * \section files Main File
 * - main.c                : DAC Calibration application main file
 *
 * \section referenceinfo References
 *  - Refer Application Note: AVR1301: Using the XMEGA DAC
 *  - Refer Application Note: AVR1000: Getting Started with the XMEGA ADC
 *
 * \section device_info Device Info
 * All AVR XMEGA AU devices. This example have been tested with following setup:
 * - XMEGA-A3BU Xplained
 *
 * \section description Description of the example
 * - The example calibrates the DAC by using the internal ADC. Please be aware
 *   that signals will be outputted on the DAC pins. 
 * - DAC and ADC is configured for the calibration sequence
 * - Offset and gain is calibrated for DAC CH0 and DAC CH1.
 *   When calibrating always start with the offset then do gain calibration.
 * - A SAW tooth signal is generated to demonstrate that the calibration works
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 */


/**
 * \file
 *
 * \brief This is an example on how to calibrate the XMEGA DAC by using the 
 * internal ADC.
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <conf_board.h>

/* Target values for calibrating the DAC using the ADC */
/* Target for ADC value when correct offset for the DAC */
#warning "Values are only applicable to DAC ref AVCC and ADC ref AVCC/1.6"
#define ADC_DAC_OFFSET_TARGET ((uint16_t)((2048*1.6)/2))
#define ADC_DAC_GAIN_TARGET ((uint16_t)((2048*1.6)/4))
#define DAC_OUTPUT_FOR_OFFSET_CALIB ((uint16_t)(4096/2))
#define DAC_OUTPUT_FOR_GAIN_CALIB ((uint16_t)(4096/4))

/* Number of samples to average for each ADC measurement of DAC input */
#define AVG_SAMPLES 32

/* Which ADC channels to use for what DAC channel for calibration */
#define ADC_CH_TO_DAC_CH0 ADC_CH0
#define ADC_CH_TO_DAC_CH1 ADC_CH1

/* This define must match the DAC output pins */
#define CALIBRATION_ADC ADCA


/* Global variables */
/* Holds the measured ADC offset value */
int16_t adc_offset = 0;

/**
 * \brief Configure the DAC pins for analog operation only and as output
 */
static void configure_pins(void)
{
	/* Disable the digital part of the DAC output pin */
	ioport_configure_pin(DAC_OUTPUT_PIN_CH0, IOPORT_DIR_OUTPUT |
			IOPORT_INPUT_DISABLE);
	ioport_configure_pin(DAC_OUTPUT_PIN_CH1, IOPORT_DIR_OUTPUT |
			IOPORT_INPUT_DISABLE);
}

/**
 * \brief Configure the DAC for calibration
 */
static void configure_dac(void)
{
	struct dac_config conf;

	dac_read_configuration(&DACB, &conf);
	dac_set_conversion_parameters(&conf, DAC_REF_AVCC, DAC_ADJ_RIGHT);
	dac_set_active_channel(&conf, DAC_CH0 | DAC_CH1, 0);

	dac_write_configuration(&DACB, &conf);
	dac_enable(&DACB);
}

/**
 * \brief Wrapper function for getting and ADC sample
 *
 * \param   adc_ch which channel to get the ADC samples from
 * /returns        16-bit signed ADC result
 */
static int16_t adc_get_sample(uint8_t adc_ch)
{
	int16_t result = 0;

	adc_start_conversion(&CALIBRATION_ADC, adc_ch);

	adc_wait_for_interrupt_flag(&CALIBRATION_ADC, adc_ch);

	result = adc_get_result(&CALIBRATION_ADC, adc_ch);

	adc_clear_interrupt_flag(&CALIBRATION_ADC, adc_ch);

	return result;
}

/**
 * \brief Wrapper function for getting averaged ADC measurements
 *
 * \param   adc_ch  which ADC channel to get samples from e.g. ADC_CH0, ADC_CH1
 * \param   samples number of samples to average over
 * /returns         16-bit signed result of the averaged ADC samples
 */
static int16_t adc_get_sample_avg(uint8_t adc_ch, uint8_t samples)
{
	int32_t result = 0;

	for (uint8_t i = samples; i > 0; i--) {
		result = result + (int32_t)adc_get_sample(adc_ch);
	}

	result = result / samples;

	return ((int16_t)result);
}

/**
 * \brief Configure the ADC for DAC calibration
 */
static void configure_adc(void)
{
	struct adc_config adc_conf;
	struct adc_channel_config adcch_conf;

	adc_read_configuration(&CALIBRATION_ADC, &adc_conf);
	adcch_read_configuration(&CALIBRATION_ADC, ADC_CH_TO_DAC_CH0, &adcch_conf);

	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12, ADC_REF_VCC);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 2, 0);
	adc_set_clock_rate(&adc_conf, 62500UL);

	adcch_set_input(&adcch_conf, ADCCH_POS_PIN1, ADCCH_NEG_PIN1, 1);

	adc_write_configuration(&CALIBRATION_ADC, &adc_conf);
	adcch_write_configuration(&CALIBRATION_ADC, ADC_CH_TO_DAC_CH0, &adcch_conf);

	/* Find the offset of the ADC */
	adc_enable(&CALIBRATION_ADC);
	adc_offset = adc_get_sample_avg(ADC_CH_TO_DAC_CH0, 128);
	adc_disable(&CALIBRATION_ADC);

	/* Switch input to the DAC output pin PB3, i.e. ADC pin 11 */
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN10, ADCCH_NEG_PAD_GND, 1);
	adcch_write_configuration(&CALIBRATION_ADC, ADC_CH_TO_DAC_CH0, &adcch_conf);

	adcch_set_input(&adcch_conf, ADCCH_POS_PIN11, ADCCH_NEG_PAD_GND, 1);
	adcch_write_configuration(&CALIBRATION_ADC, ADC_CH_TO_DAC_CH1, &adcch_conf);

	adc_enable(&CALIBRATION_ADC);

}

/**
 * \brief Function for setting the offset calibration value
 *
 * \param dac    pointer to the DAC hardware module.
 * \param dac_ch which channel to set gain calibration for (DAC_CH0, DAC_CH1)
 * \param offset the calibration value to write.
 */
static void dac_set_channel_offset(DAC_t *dac, uint8_t dac_ch, uint8_t offset)
{
	if (dac_ch == DAC_CH0) {
		dac->CH0OFFSETCAL = offset;
	} else if (dac_ch == DAC_CH1) {
		dac->CH1OFFSETCAL = offset;
	}
}

/**
 * \brief Function for setting the gain calibration value
 *
 * \param dac    pointer to the DAC hardware module.
 * \param dac_ch which channel to set gain calibration for (DAC_CH0, DAC_CH1)
 * \param gain   the calibration value to write.
 */
static void dac_set_channel_gain(DAC_t *dac, uint8_t dac_ch, uint8_t gain)
{
	if (dac_ch == DAC_CH0) {
		dac->CH0GAINCAL = gain;
	} else if (dac_ch == DAC_CH1) {
		dac->CH1GAINCAL = gain;
	}
}

/**
 * \brief Offset calibrate the DAC channel
 *
 * \param dac_ch which DAC channel to calibrate, valid values are DAC_CH0 and
 * DAC_CH1.
 */
static void dac_calibrate_offset(uint8_t dac_ch)
{
	int16_t result = 0;
	uint8_t dac_offset = 0;
	uint8_t adc_ch = ADC_CH_TO_DAC_CH0;

	/* Make sure that the gain calibration is zero before offset calibration */
	if (dac_ch == DAC_CH0) {
		DACB.CH0GAINCAL = 0;
		adc_ch = ADC_CH_TO_DAC_CH0;
	} else if (dac_ch == DAC_CH1) {
		DACB.CH1GAINCAL = 0;
		adc_ch = ADC_CH_TO_DAC_CH1;
	}

	/* Set the DAC to output half its output voltage */
	dac_set_channel_value(&DACB, dac_ch, DAC_OUTPUT_FOR_OFFSET_CALIB);

	while (result != ADC_DAC_OFFSET_TARGET) {
		dac_set_channel_offset(&DACB, dac_ch, dac_offset++);

		/* Allow the DAC output to settle */
		delay_us(50);
		result = adc_get_sample_avg(adc_ch, AVG_SAMPLES) - adc_offset;

		if (dac_offset == 0xFF) {
			/* Offset calibration failed */
			Assert(false);
			break;
		}
	}
}

/**
 * \brief Gain calibrate a DAC channel
 *
 * \param dac_ch which DAC channel to calibrate, valid values are DAC_CH0 and
 * DAC_CH1.
 */
static void dac_calibrate_gain(uint8_t dac_ch)
{
	int16_t result = 0;
	uint8_t dac_gain = 0;
	uint8_t adc_ch = ADC_CH_TO_DAC_CH0;

	if (dac_ch == DAC_CH0) {
		adc_ch = ADC_CH_TO_DAC_CH0;
	} else if (dac_ch == DAC_CH1) {
		adc_ch = ADC_CH_TO_DAC_CH1;
	}

	/* Set the DAC output value to 1/4 of its output voltage */
	dac_set_channel_value(&DACB, dac_ch, DAC_OUTPUT_FOR_GAIN_CALIB);

	while (result != ADC_DAC_GAIN_TARGET) {
		dac_set_channel_gain(&DACB, dac_ch, dac_gain++);
		/* Allow the DAC output to settle */
		delay_us(50);
		result = adc_get_sample_avg(adc_ch, AVG_SAMPLES) - adc_offset;

		if (dac_gain == 0xFF) {
			/* Gain calibration failed*/
			Assert(false);
			break;
		}
	}
}

/**
 * \brief Function for calibration the DAC using the internal ADC
 *
 * This function will calibrate the DAC using the internal ADC, please be aware
 * that signals will be outputted on the DAC pins.
 *
 * When calibrating always start with the offset then do gain calibration.
 *
 */
static void dac_calibrate(void)
{
	/* Configure the DAC output pins */
	configure_pins();

	/* Configure the DAC for this calibration sequence */
	configure_dac();

	/* Configure the ADC for this calibration sequence */
	configure_adc();

	/* Calibrate offset and gain for DAC CH0 */
	dac_calibrate_offset(DAC_CH0);
	dac_calibrate_gain(DAC_CH0);

	/* Calibrate offset and gain for DAC CH1 */
	dac_calibrate_offset(DAC_CH1);
	dac_calibrate_gain(DAC_CH1);

	adc_disable(&CALIBRATION_ADC);
}

/**
 * \brief Main application rutine
 */
int main (void)
{
	/* Variables used to produce a saw tooth signal */
	static uint16_t dac_data = 0;
	static int8_t direction = 1;

	board_init();

	sysclk_init();
	
	/* Calibrate the DAC */
	dac_calibrate();

	/* Generate a SAW tooth signal, just as a demo that the calibration works */
	while (true) {

		dac_wait_for_channel_ready(&DACB, DAC_CH0);

		dac_set_channel_value(&DACB, DAC_CH0, dac_data);
		dac_set_channel_value(&DACB, DAC_CH1, dac_data);

		dac_data = dac_data + direction;

		/* If the output value is the top value, switch direction */
		if (dac_data == 4095) {
			direction = -1;
		}

		/* If the output value is the lowest value, switch direction */
		if (dac_data == 0) {
			direction = 1;
		}
	}
}
