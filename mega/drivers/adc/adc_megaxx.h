/**
 * \file
 *
 * \brief ATmegaxx family defines
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

#ifndef _ADC_MEGAXX_H_
#define _ADC_MEGAXX_H_

#define ADC_ADTS_REG     SFIOR

enum adc_mux_setting {
	//! ADC0, single ended
	ADC_MUX_ADC0 = 0x0,
	//! ADC1, single ended
	ADC_MUX_ADC1 = 0x1,
	//! ADC2, single ended
	ADC_MUX_ADC2 = 0x2,
	//! ADC3, single ended
	ADC_MUX_ADC3 = 0x3,
	//! ADC4, single ended
	ADC_MUX_ADC4 = 0x4,
	//! ADC5, single ended
	ADC_MUX_ADC5 = 0x5,
	//! ADC6, single ended
	ADC_MUX_ADC6 = 0x6,
	//! ADC7, single ended
	ADC_MUX_ADC7 = 0x7,
	//! 1.22V internal bandgap voltage
	ADC_MUX_1V22 = 0x1E,
	//! GND, single ended
	ADC_MUX_GND = 0x1F,
	//! Positive: ADC0 Negative: ADC0 Gain: 10x
	ADC_MUX_ADC0_ADC0_10X = 0x8,
	//! Positive: ADC1 Negative: ADC0 Gain: 10x
	ADC_MUX_ADC1_ADC0_10X = 0x9,
	//! Positive: ADC0 Negative: ADC0 Gain: 200x
	ADC_MUX_ADC0_ADC0_200X = 0xA,
	//! Positive: ADC1 Negative: ADC0 Gain: 200x
	ADC_MUX_ADC1_ADC0_200X = 0xB,
	//! Positive: ADC2 Negative: ADC2 Gain: 10x
	ADC_MUX_ADC2_ADC2_10X = 0xC,
	//! Positive: ADC3 Negative: ADC2 Gain: 10x
	ADC_MUX_ADC3_ADC2_10X = 0xD,
	//! Positive: ADC2 Negative: ADC2 Gain: 200x
	ADC_MUX_ADC2_ADC2_200X = 0xE,
	//! Positive: ADC3 Negative: ADC2 Gain: 200x
	ADC_MUX_ADC3_ADC2_200X = 0xF,
	//! Positive: ADC0 Negative: ADC1
	ADC_MUX_ADC0_ADC1 = 0x10,
	//! Positive: ADC1 Negative: ADC1
	ADC_MUX_ADC1_ADC1 = 0x11,
	//! Positive: ADC2 Negative: ADC1
	ADC_MUX_ADC2_ADC1 = 0x12,
	//! Positive: ADC3 Negative: ADC1
	ADC_MUX_ADC3_ADC1 = 0x13,
	//! Positive: ADC4 Negative: ADC1
	ADC_MUX_ADC4_ADC1 = 0x14,
	//! Positive: ADC5 Negative: ADC1
	ADC_MUX_ADC5_ADC1 = 0x15,
	//! Positive: ADC6 Negative: ADC1
	ADC_MUX_ADC6_ADC1 = 0x16,
	//! Positive: ADC7 Negative: ADC1
	ADC_MUX_ADC7_ADC1 = 0x17,
	//! Positive: ADC0 Negative: ADC2
	ADC_MUX_ADC0_ADC2 = 0x18,
	//! Positive: ADC1 Negative: ADC2
	ADC_MUX_ADC1_ADC2 = 0x19,
	//! Positive: ADC2 Negative: ADC2
	ADC_MUX_ADC2_ADC2 = 0x1A,
	//! Positive: ADC3 Negative: ADC2
	ADC_MUX_ADC3_ADC2 = 0x1B,
	//! Positive: ADC4 Negative: ADC2
	ADC_MUX_ADC4_ADC2 = 0x1C,
	//! Positive: ADC5 Negative: ADC2
	ADC_MUX_ADC5_ADC2 = 0x1D,
};

enum adc_voltage_reference {
	//! AREF, internal Vref turned off
	ADC_VREF_AREF = (0x0 << REFS0),
	//! AVCC
	ADC_VREF_AVCC = (0x1  << REFS0),
	//! Reserved value, not available.
	ADC_VREF_RESERVED = (0x2 << REFS0),
	//! Internal 1.1V voltage reference
	ADC_VREF_2V56 = (0x3 << REFS0),

};

#endif  // _ADC_MEGAXX_H_
