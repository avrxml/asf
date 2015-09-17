/**
 * \file
 *
 * \brief XMEGA ADC oversampling header file for ADC source code
 *        This file contains the function prototypes
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

#ifndef ADC_OVERSAMPLING_H
#define ADC_OVERSAMPLING_H

/* ! \brief (Number of digits -1) in which result is displayed on LCD */
#define NUMBER_OF_DIGITS_IN_RESULT    6

/* ! \brief (Number of digits -1) in which ADC raw count is displayed on LCD */
#define NUMBER_OF_DIGITS_IN_ADCCOUNT  5

/* ! \brief Size of buffer used to store ASCII value of result */
#define ASCII_BUFFER_SIZE    9

/**
 * \brief Static variable/flag to indicate that one set of oversampling is
 *        done for start processing
 */
extern volatile bool adc_oversampled_flag;

/* FUNCTION PROTOTYPES */

/*! \brief Function to get the offset of ADC */
static int8_t adc_offset_get_signed(void);

/*! \brief Function to convert decimal value to ASCII */
static void convert_to_ascii(uint8_t *buf_index, uint64_t dec_val);

/*! \brief Function to display raw ADC count on LCD */
void display_adccount( uint64_t adc_rawcount, uint8_t x_cordinate,
		uint8_t sign_flag);

/*! \brief Function to initialize the ADC */
extern void init_adc(void);

/*! \brief Function to process sampled ADC values */
extern void adc_oversampled(void);

#endif /* ADC_OVERSAMPLING_H */
