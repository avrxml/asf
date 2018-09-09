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

#include "asf.h"

/**
 * \brief Initialize ADC channels for on-board sensors.
 *
 * This will set up the ADC for reading the NTC, light sensor, potentiometer,
 * and external voltage input present on the A3BU-Xplained board.
 */
void adc_sensors_init(void);

/**
 * \brief Stop the ADC sensors
 *
 */
void adc_sensors_stop(void);

/**
 * \brief Start a new burst of ADC conversions.
 *
 * This will initiated a new burst of ADC conversions to update the on-board
 * sensors values.
 */
void adc_sensors_start_conversions(void);

/**
 * \brief Read the temperature value.
 *
 * \return temperature in Degree Celsius
 */
int8_t adc_sensor_get_temperature(void);

/**
 * \brief Read the light sensor value.
 *
 * This will read the ADC value of the channel and pin connected to the
 * lightsensor.
 *
 * \return the raw light sensor value from the ACD
 */
uint16_t adc_sensor_get_light(void);

/**
 * \brief Read the potentiometer value.
 *
 * This will read the ADC value of the channel and pin connected to the
 * potentiometer.
 *
 * \return the potentiometer voltage value in millivolt
 */
uint16_t adc_sensor_get_potentiometer(void);

/**
 * \brief Read the external voltage input value.
 *
 * This will read the ADC value of the channel and pin connected to the
 * external voltage input.
 *
 * \return the external voltage input value in millivolt
 */
uint16_t adc_sensor_get_ext_voltage(void);


#endif /* ADC_SENSORS_H_INCLUDED */
