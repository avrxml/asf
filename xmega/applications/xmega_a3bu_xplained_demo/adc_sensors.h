/**
 * \file
 *
 * \brief ADC Sensor interface
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
