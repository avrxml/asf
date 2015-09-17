/**
 * \file
 *
 * \brief ADC Sensor interface
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
#ifndef ADC_SENSORS_H_INCLUDED
#define ADC_SENSORS_H_INCLUDED

#include "adc.h"

/**
 * \brief Call this to schedule a ADC reading of the temperature sensor
 *
 * Calling this will trigger the ADC to perform a reading of the on board NTC,
 * the ADC will automatically recall this function the number of times
 * specified for averaging.
 */
static inline void ntc_measure(void)
{
	adc_start_conversion(&ADCA, ADC_CH1);
}

static inline void lightsensor_measure(void)
{
	adc_start_conversion(&ADCA, ADC_CH0);
}

void adc_handler(ADC_t *adc, uint8_t channel, adc_result_t result);
void adc_sensors_init(void);
int16_t ntc_get_raw_value(void);
int8_t ntc_get_temperature(void);
int16_t lightsensor_get_raw_value(void);
bool ntc_data_is_ready(void);
bool lightsensor_data_is_ready(void);

#endif /* ADC_SENSORS_H_INCLUDED */
