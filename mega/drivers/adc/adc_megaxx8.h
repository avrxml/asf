/**
 * \file
 *
 * \brief ATmegaxx8 family defines
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

#ifndef _ADC_MEGAXX8_H_
#define _ADC_MEGAXX8_H_

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
#if MEGA_XX8_A
	//! ADC8, single ended. Temperature sensor on megaxx8A/P/PA devices
	ADC_MUX_TEMPSENSE = 0x8,
#endif
	//! 1.1V internal bandgap voltage
	ADC_MUX_1V1 = 0x0E,
	//! GND, single ended
	ADC_MUX_GND = 0x0F,
};

enum adc_voltage_reference {
	//! AREF, internal Vref turned off
	ADC_VREF_AREF = (0x0 << REFS0),
	//! AVCC
	ADC_VREF_AVCC = (0x1  << REFS0),
	//! Reserved value, not available.
	ADC_VREF_RESERVED = (0x2  << REFS0),
	//! Internal 1.1V voltage reference
	ADC_VREF_1V1 = (0x3  << REFS0),
};

#endif  // _ADC_MEGAXX8_H_
