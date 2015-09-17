/**
 * \file
 *
 * \brief ATmegaxx0/1/4 family defines
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

#ifndef _ADC_MEGAXX0_1_4_H_
#define _ADC_MEGAXX0_1_4_H_

#define ADC_ADTS_REG     ADCSRB

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
	//! 1.1V internal bandgap voltage
	ADC_MUX_1V1 = 0x1E,
	//! GND, single ended
	ADC_MUX_GND = 0x1F,
#if MEGA_XX0
	//! ADC8, single ended
	ADC_MUX_ADC8 = 0x20,
	//! ADC9, single ended
	ADC_MUX_ADC9 = 0x21,
	//! ADC10, single ended
	ADC_MUX_ADC10 = 0x22,
	//! ADC11, single ended
	ADC_MUX_ADC11 = 0x23,
	//! ADC12, single ended
	ADC_MUX_ADC12 = 0x24,
	//! ADC13, single ended
	ADC_MUX_ADC13 = 0x25,
	//! ADC14, single ended
	ADC_MUX_ADC14 = 0x26,
	//! ADC15, single ended
	ADC_MUX_ADC15 = 0x27,
#endif
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
#if MEGA_XX0
	//! Positive: ADC8 Negative: ADC8 Gain: 10x
	ADC_MUX_ADC8_ADC8_10X = 0x28,
	//! Positive: ADC9 Negative: ADC8 Gain: 10x
	ADC_MUX_ADC9_ADC8_10X = 0x29,
	//! Positive: ADC8 Negative: ADC8 Gain: 200x
	ADC_MUX_ADC8_ADC8_200X = 0x2A,
	//! Positive: ADC9 Negative: ADC8 Gain: 200x
	ADC_MUX_ADC9_ADC8_200X = 0x2B,
	//! Positive: ADC10 Negative: ADC10 Gain: 10x
	ADC_MUX_ADC10_ADC10_10X = 0x2C,
	//! Positive: ADC11 Negative: ADC10 Gain: 10x
	ADC_MUX_ADC11_ADC10_10X = 0x2D,
	//! Positive: ADC10 Negative: ADC10 Gain: 200x
	ADC_MUX_ADC10_ADC10_200X = 0x2E,
	//! Positive: ADC11 Negative: ADC10 Gain: 200x
	ADC_MUX_ADC11_ADC10_200X = 0x2F,
	//! Positive: ADC8 Negative: ADC9
	ADC_MUX_ADC8_ADC9 = 0x30,
	//! Positive: ADC9 Negative: ADC9
	ADC_MUX_ADC9_ADC9 = 0x31,
	//! Positive: ADC10 Negative: ADC9
	ADC_MUX_ADC10_ADC9 = 0x32,
	//! Positive: ADC11 Negative: ADC9
	ADC_MUX_ADC11_ADC9 = 0x33,
	//! Positive: ADC12 Negative: ADC9
	ADC_MUX_ADC12_ADC9 = 0x34,
	//! Positive: ADC13 Negative: ADC9
	ADC_MUX_ADC13_ADC9 = 0x35,
	//! Positive: ADC14 Negative: ADC9
	ADC_MUX_ADC14_ADC9 = 0x36,
	//! Positive: ADC15 Negative: ADC9
	ADC_MUX_ADC15_ADC9 = 0x37,
	//! Positive: ADC8 Negative: ADC10
	ADC_MUX_ADC8_ADC10 = 0x38,
	//! Positive: ADC9 Negative: ADC10
	ADC_MUX_ADC9_ADC10 = 0x39,
	//! Positive: ADC10 Negative: ADC10
	ADC_MUX_ADC10_ADC10 = 0x3A,
	//! Positive: ADC11 Negative: ADC10
	ADC_MUX_ADC11_ADC10 = 0x3B,
	//! Positive: ADC12 Negative: ADC10
	ADC_MUX_ADC12_ADC10 = 0x3C,
	//! Positive: ADC13 Negative: ADC10
	ADC_MUX_ADC13_ADC10 = 0x3D,
#endif
};

enum adc_voltage_reference {
	//! AREF, internal Vref turned off
	ADC_VREF_AREF = (0x0 << REFS0),
	//! AVCC
	ADC_VREF_AVCC = (0x1  << REFS0),
	//! Internal 1.1V voltage reference
	ADC_VREF_1V1 = (0x2  << REFS0),
	//! Internal 2.56V voltage reference
	ADC_VREF_2V56 = (0x3  << REFS0),
};

#endif  // _ADC_MEGAXX0_1_4_H_
