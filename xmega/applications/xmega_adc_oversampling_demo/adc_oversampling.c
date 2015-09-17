/**
 * \file
 *
 * \brief This file contains the function implementations for XMEGA ADC
 *        Oversampling Application.It shows how to use oversampling to
 *        increase the resolution.In this example configuration has been been
 *        selected for oversampling from 12 bit signed to 16 bit signed result
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

#include <compiler.h>
#include <sleepmgr.h>
#include <sysclk.h>
#include <conf_oversampling.h>
#include <adc_oversampling.h>
#include <asf.h>

/**
 * \brief Static variable to store total ADC Offset value for total number
 *        of sample
 */
static int16_t adc_offset = 0;

/* ! \brief Static variable to store offset for single sample */
static int8_t adc_offset_one_sample = 0;

/* ! \brief Static variable to accumulate sampled ADC result */
static volatile int64_t adc_result_accumulator = 0;

/* ! \brief Static variable to to store single sampled ADC result */
static volatile int64_t adc_result_one_sample = 0;

/* ! \brief Static variable to process the accumulated sampled ADC results */
static int64_t adc_result_accum_processed = 0;

/* ! \brief Static variable to process the single ADC result */
static int64_t adc_result_one_sample_processed = 0;

/* ! \brief Static variable to keep number of samples for oversampling */
static volatile uint16_t adc_samplecount = 0;

/**
 * \brief Static variable to find analog value at ADC input after
 *        oversampling process
 */
static int64_t v_input = 0;

/**
 * \brief Static variable to find analog value at ADC Input
 *        without oversampling process
 */
static int64_t v_input_one_sample = 0;

/**
 * \brief Global variable/flag to indicate that one set of
 *         oversampling is done for start processing
 */
volatile bool adc_oversampled_flag = false;

/* ! \brief Static variable to keep ADC configuration parameters */
static struct adc_config adc_conf;

/* ! \brief Static variable to keep ADC channel configuration parameters */
static struct adc_channel_config adc_ch_conf;

/**
 * \brief Static buffer variable to store ASCII value of calculated input
 *        voltage for display
 */
static uint8_t v_input_ascii_buf[ASCII_BUFFER_SIZE] = {"+1.123456"};

/**
 * \brief Callback function for ADCB-CH0 interrupts
 *  - Interrupt is configured for Conversion Complete Interrupt
 *  - ADCA CH0 result is accumulated
 *  - ADC sample count is incremented
 *  - Check If ADC sample count reached up to number of oversampling required
 *  - If so, disable ADC interrupt and set flag to start oversampling process
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask ADC channel mask.
 * \param result Conversion result from ADC channel.
 */
static void adc_handler(ADC_t *adc, uint8_t ch_mask, adc_result_t result)
{
	/*  Get Result from ADCB-CH0 Register and Accumulate */
	adc_result_accumulator +=  result;

	/* Increment sample count */
	adc_samplecount++;

	/* Check if sample count has reached oversample count */
	if (adc_samplecount >= ADC_OVER_SAMPLED_NUMBER) {
		/* Disable ADCB-CHO conversion complete interrupt until stored
		 * samples are processed
		 */
		adcch_disable_interrupt(&adc_ch_conf);
		adcch_write_configuration(&ADCB, ADC_CH0, &adc_ch_conf);

		/* Clear any pending interrupt request by clearing interrupt
		 * flag
		 */
		adc_clear_interrupt_flag(&ADCB, ADC_CH0);

		/*Set adc_oversampled_flag to start oversampling process from
		 * main function
		 */
		adc_oversampled_flag = true;

		/* Store single sample ADC result to find analog input without
		 * oversampling
		 */
		adc_result_one_sample = result;
	}
}

/**
 * \brief This function get the offset of the ADCB when it is configured
 *	      in signed mode
 *  \note The ADC must be configured and enabled before this function is run.
 * \return Offset on the ADCB
 */
static int8_t adc_offset_get_signed(void)
{
	int16_t offset = 0;
	uint8_t i;

	/* Sum four samples */
	for (i = 0; i < 4; i++) {
		/* Do one conversion to find offset */
		adc_start_conversion(&ADCB, ADC_CH0);
		adc_wait_for_interrupt_flag(&ADCB, ADC_CH0);

		/* Accumulate conversion results */
		offset += adc_get_result(&ADCB, ADC_CH0);
	}

	/* Return mean value */
	return ((int8_t)(offset / 4));
}

/**
 * \brief This Function converts a decimal value to ASCII
 *  - It will extract each digit from decimal value and add to
 *	  v_input_ascii_buf
 *  - It will take care to keep decimal point at correct position
 * \param buf_index Pointer to buffer used to store ASCII value.
 * \param dec_val Decimal to be converted as ASCII.
 */
static void convert_to_ascii(uint8_t *buf_index, uint64_t dec_val)
{
	uint8_t digit_count = 0;

	/* Loop through all digits to convert to ASCII */
	for (digit_count = 0; digit_count <= NUMBER_OF_DIGITS_IN_RESULT;
			digit_count++) {
		/*
		 * Check if first digit location has reached,
		 *  - If so,keep Decimal point(.),decrement position
		 */
		if (digit_count == NUMBER_OF_DIGITS_IN_RESULT) {
			buf_index--;
		}

		/*
		 * Extract each Digit by doing %10 and convert to ASCII,
		 *  - Then store to buffer index
		 *	- Initially we will get the right most digit and so on
		 */
		*buf_index = (dec_val % 10) + 48;

		/* Remove extracted digit by doing divide with 10 */
		dec_val = (dec_val / 10);

		/*
		 * Decrement the buffer Index to store next digit ,start from
		 * right most digit and move backwards for extracting each digit
		 */
		buf_index--;
	}
}

/**
 * \brief This Function Display ADC count on LCD
 *  - It will extract each digit from ADC count and convert to ASCII
 *  - Use GFX service for displaying each character
 * \param adc_rawcount ADC raw count value to be displayed.
 * \param x_cordinate X-coordinate where display should end.
 * \param sign_flag Sign of ADC count.If negative sign_flag is set.
 */
void display_adccount( uint64_t adc_rawcount, uint8_t x_cordinate,
		uint8_t sign_flag)
{
	uint8_t digit_count = 0;
	uint8_t adc_count_digit = 0;
	uint8_t sign_digit = '+';

	/* Loop through all digits to convert to ASCII */
	for (digit_count = 0; digit_count <= NUMBER_OF_DIGITS_IN_ADCCOUNT;
			digit_count++) {
		/* Extract each digit from raw ADC count and convert to ASCII
		 * - The Last digit will be extracted first and displayed
		 */
		adc_count_digit =   (adc_rawcount % 10)  + 48;

		/* Display the extracted character on LCD */
		gfx_mono_draw_char((const char)adc_count_digit, x_cordinate, 23,
				&sysfont);

		/* Point to x-coordinate for display of next digit */
		x_cordinate = (x_cordinate - 7);

		/* Remove extracted digit from raw count by doing divide
		 * with 10
		 */
		adc_rawcount = (adc_rawcount / 10);
	}

	/* If sign_flag is set display negative symbol */
	if (sign_flag) {
		sign_digit = '-';
	}

	/* Display the sign character on LCD */
	gfx_mono_draw_char((const char)sign_digit, x_cordinate, 23, &sysfont);
}

/**
 * \brief This function initialize the ADCB,gets ADCB-CH0 offset and configure
 *        ADCB-CH0 for oversampling
 *  - ADCB-CH0 is configured in 12bit, signed differential mode without gain
 *  - To read ADC offset, ADCB-Pin3(PB3) used as both +ve and -ve input
 *  - After reading ADC offset,to start oversampling,ADCB +ve and -ve input
 *    are configured
 */
void init_adc(void)
{
	/* Initialize configuration structures */
	adc_read_configuration(&ADCB, &adc_conf);
	adcch_read_configuration(&ADCB, ADC_CH0, &adc_ch_conf);

	/* Configure the ADCB module:
	 * - Signed, 12-bit resolution
	 * - External reference on AREFB pin.
	 * - 250 KSPS ADC clock rate
	 * - Manual conversion triggering
	 * - Callback function
	 */
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12,
			ADC_REF_AREFB);
	adc_set_clock_rate(&adc_conf, 250000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_write_configuration(&ADCB, &adc_conf);
	adc_set_callback(&ADCB, &adc_handler);

	/* Configure ADC B channel 0 for offset calculation
	 * - Differential mode without gain
	 * - Selected Pin3 (PB3) as +ve and -ve input for offset calculation
	 */
	adcch_set_input(&adc_ch_conf, ADCCH_POS_PIN3, ADCCH_NEG_PIN3, 1);
	adcch_write_configuration(&ADCB, ADC_CH0, &adc_ch_conf);

	/* Enable ADCB */
	adc_enable(&ADCB);

	/* Get ADC offset in to ADC_Offset variable and disable ADC */
	adc_offset_one_sample = adc_offset_get_signed();

	/* Find ADC_Offset for for total number of samples */
	adc_offset = adc_offset_one_sample * ADC_OVER_SAMPLED_NUMBER;

	/* Disable ADC to configure for oversampling */
	adc_disable(&ADCB);

	/* Configure the ADCB module for oversampling:
	 * - Signed, 12-bit resolution
	 * - External reference on AREFB pin.
	 * - 250 KSPS ADC clock rate
	 * - Free running mode on Channel0 ( First Channel)
	 */

	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_FREERUN_SWEEP, 1, 0);
	adc_write_configuration(&ADCB, &adc_conf);

	/* Configure ADC B channel 0 for oversampling input
	 * - Differential mode without gain
	 * - Selected Pin1 (PB1) as +ve and Pin2 (PB2) as-ve input
	 * - Conversion complete interrupt
	 */
	adcch_set_input(&adc_ch_conf, ADC_OVER_SAMP_POSTIVE_PIN,
			ADC_OVER_SAMP_NEGATIVE_PIN, 1);
	adcch_set_interrupt_mode(&adc_ch_conf, ADCCH_MODE_COMPLETE);
	adcch_enable_interrupt(&adc_ch_conf);
	adcch_write_configuration(&ADCB, ADC_CH0, &adc_ch_conf);

	/* Enable ADCB */
	adc_enable(&ADCB);
}

/**
 * \brief This function processes sampled ADC values and calculate
 *        the oversampling result
 *  - Offset error compensation is applied on accumulated ADC value
 *  - After, scaling is done with scaled factor.
 *  - Finally, Analog value at ADC input pin is calculated
 *  - Reset all variable used in ADC ISR and enable ADC interrupt to start
 *    next oversampling Process.
 */
void adc_oversampled(void)
{
	/* ***********Processing and display of oversampled
	 * Input*************
	 **/

	/* Assign sign as +ve (as zero) in default for Rrw ADC count display */
	uint8_t sign_flag = 0;

	/* Offset error Compensation for entire number of samples */
	adc_result_accum_processed = adc_result_accumulator - adc_offset;

	/* Gain error Compensation for entire number of samples */
	adc_result_accum_processed = (adc_result_accum_processed *
			ADC_GAIN_ERROR_FACTOR) >> 16;

	/* Scale the accumulated result to get over sampled Result */
	adc_result_accum_processed = adc_result_accum_processed >>
			ADC_OVER_SAMP_SCALING_FACTOR;

	/* Calculate the analog input voltage value
	 * - Input Analog value = (ADC_Count * Reference
	 * Volt)/(2^adcresolution))
	 */
	v_input = (adc_result_accum_processed) *
			(ADC_OVER_SAMP_REF_VOLT_IN_MICRO);

	v_input = v_input / ADC_OVER_SAMP_MAX_COUNT;

	/* If input is negative, assign sign for display and use absolute value
	 */
	if (v_input < 0) {
		v_input = abs(v_input);
		v_input_ascii_buf[0] = '-';
	} else {
		v_input_ascii_buf[0] = '+';
	}

	/* Convert calculated analog value to ASCII for display */
	convert_to_ascii(&v_input_ascii_buf[ASCII_BUFFER_SIZE - 1], v_input);

	/* Display the result on LCD display */
	gfx_mono_draw_string(v_input_ascii_buf, 0, 10, &sysfont);

	/* If ADC count is negative, assign sign for display and use absolute
	 * value */
	if (adc_result_accum_processed < 0) {
		adc_result_accum_processed = abs(adc_result_accum_processed);
		sign_flag = 1;
	} else {
		sign_flag = 0;
	}

	/* Display oversampled raw ADC count on LCD display */
	display_adccount((int64_t)adc_result_accum_processed, (uint8_t)42,
			sign_flag );

	/* ***********Processing and display of Single Sampled
	 * Input*************
	 **/

	/* Offset error compensation for one sample */
	adc_result_one_sample_processed = adc_result_one_sample -
			adc_offset_one_sample;

	/* Gain error compensation for one sample */
	adc_result_one_sample_processed = (adc_result_one_sample_processed *
			ADC_GAIN_ERROR_FACTOR) >> 16;

	/* Calculate the analog input voltage value without oversampling
	 * - Input analog value = (ADC_Count * Reference
	 * Volt)/(2^adcresolution))
	 */
	v_input_one_sample = (adc_result_one_sample_processed) *
			(ADC_OVER_SAMP_REF_VOLT_IN_MICRO);

	v_input_one_sample = v_input_one_sample / ADC_NO_OVER_SAMP_MAX_COUNT;

	/* If input is negative, assign sign for display and use absolute value
	 */
	if (v_input_one_sample < 0) {
		v_input_one_sample = abs(v_input_one_sample);
		v_input_ascii_buf[0] = '-';
	} else {
		v_input_ascii_buf[0] = '+';
	}

	/* Convert calculated analog value to ASCII for display(no oversampling)
	 */
	convert_to_ascii(&v_input_ascii_buf[ASCII_BUFFER_SIZE - 1],
			v_input_one_sample);

	/* Display the result on LCD display(no oversampling) */
	gfx_mono_draw_string(v_input_ascii_buf, 75, 10, &sysfont);

	/* If ADC count is negative, assign sign for display and use absolute
	 * value */
	if (adc_result_one_sample_processed < 0) {
		adc_result_one_sample_processed = abs(
				adc_result_one_sample_processed);
		sign_flag = 1;
	} else {
		sign_flag = 0;
	}

	/* Display oversampled raw ADC count on LCD display */
	display_adccount((int64_t)adc_result_one_sample_processed,
			(uint8_t)117, sign_flag );

	/*Reset ADC_result accumulator value and ADC_sample count to zero
	 * for next oversampling process
	 */
	adc_result_accumulator = 0;
	adc_result_accum_processed = 0;
	adc_samplecount = 0;

	adc_result_one_sample = 0;
	adc_result_one_sample_processed = 0;

	/* Configure conversion complete interrupt for  ADCB-CH0 to re-start
	 * over sampling
	 */
	adcch_set_interrupt_mode(&adc_ch_conf, ADCCH_MODE_COMPLETE);
	adcch_enable_interrupt(&adc_ch_conf);
	adcch_write_configuration(&ADCB, ADC_CH0, &adc_ch_conf);
}
