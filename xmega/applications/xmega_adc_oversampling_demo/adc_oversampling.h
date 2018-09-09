/**
 * \file
 *
 * \brief XMEGA ADC oversampling header file for ADC source code
 *        This file contains the function prototypes
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
