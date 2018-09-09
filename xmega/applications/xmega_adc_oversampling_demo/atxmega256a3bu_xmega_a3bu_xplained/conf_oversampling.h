/**
 * \file
 *
 * \brief Configuration file for XMEGA ADC Oversampling Demo Application
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

#ifndef CONF_OVERSAMPLING_H
#define CONF_OVERSAMPLING_H

/**************************** Oversampling configuration*********************/
/****************************************************************************/

/**
 * \name Oversampling configuration
 */
/* ! @{ */

/**
 * \brief For each additional bit of ADC resolution, n, the signal must be
 *        oversampled 4^n times.To achieve 16 signed bit result from 12 bit
 *        signed ADC,we are choosing  4 bit additional resolution.
 *        Hence signal has  to be sampled 4^4( 4 to the power 4 ),
 *        i.e. 256 times more samples.
 *        Define this Macro as per number of additional samples required,here
 *        it did choose as 256 in this example
 */
#define ADC_OVER_SAMPLED_NUMBER        256

/**
 * \brief The scale factor, sf, given by sf = 2^n, is the factor, which is
 *        the sum of 4^n samples should be divided by, to scale the result
 *        properly.n is the desired number of extra bit.
 *        In this example we did choose 4 extra bit. Hence the value
 *        is 16, i.e. 2^4.Instead of dividing by 16,we can right shift by 4.
 *		  Hence this Macro is defined as 4.
 */
#define ADC_OVER_SAMP_SCALING_FACTOR    4

/**
 * \brief Define the Maximum ADC count with selected ADC resolution. Here we
 *        are oversampling to get 16 bit signed result.That is 15 bit Result.
 *        Hence the value of this macro will be (2^15)-1, which is 32767
 */
 #define ADC_OVER_SAMP_MAX_COUNT       32767

/**
 * \brief Define the maximum ADC count with selected ADC resolution when no
 *        oversampling is done. Here we configured to get 12 bit signed .
 *        result.That is 11 bit Result. Hence the value of this macro will be
 *        (2^11)-1, which is 2047
 */
 #define ADC_NO_OVER_SAMP_MAX_COUNT    2047

/* ! @} */

/**************************** ADC configuration for oversampling*************/
/****************************************************************************/

/**
 * \name ADC configuration for oversampling
 */
/* ! @{ */

/**
 * \brief for oversampling, setting up the positive pin. Here,
 *        selected PIN1 (PB1 for ADCB) as positive input
 */
#define ADC_OVER_SAMP_POSTIVE_PIN    ADCCH_POS_PIN1

/**
 * \brief for oversampling, setting up the negative pin. Here,
 *        selected PIN2 (PB2 for ADCB) as negative input
 */
#define ADC_OVER_SAMP_NEGATIVE_PIN  ADCCH_NEG_PIN2

/**
 * \brief Select ADCB reference voltage .Here, selected as,
 *        external reference voltage at PortB(at PB0 Pin)
 */
#define ADC_OVER_SAMP_REFSEL        ADC_REFSEL_AREFB_gc

/**
 * \brief Define the value of actual reference voltage.It is used in analog
 *        input value.Here, If external 2.5V is selected as ADC Reference,
 *        value of this macro should be reference voltage in microvolt.
 */
#define ADC_OVER_SAMP_REF_VOLT_IN_MICRO      2500000

/**
 * \brief Gain error of ADC which has to be calculated statically for each
 *        device. Gain Error is is deviation of the last output steps
 *        midpoint from the ideal line.To measure the gain error:
 *        -Increase input voltage from 0 until the last output step
 *        -The scaling factor for gain compensation equals the ideal output
 *         value for the midpoint of the last step divided by the actual value
 *         of the step
 *        Here ,given value is for a one specific device, it will vary from
 *        device to device and has to be calculated statically.
 *        It is calculated as
 *		  - (Gain error * 2^16), Here it is, (0.9984 * 2^16)
 */
#define ADC_GAIN_ERROR_FACTOR      (0xFF97)

/*! @} */

#endif /* CONF_OVERSAMPLING_H */
