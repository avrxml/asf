/**
 * \file
 *
 * \brief Main functions for the Voltage meter demonstration with ADC calibration.
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

#ifndef _MAIN_H_
#define _MAIN_H_

//default values for offset
#define ADC_OFFSET_POT 160
#define ADC_OFFSET_EXT 245

//default values for gain
#define ADC_GAIN_POT  600
#define ADC_GAIN_EXT  7000

//offset range
#define EXT_OFFSET_MIN_VAL 200
#define EXT_OFFSET_MAX_VAL 400
#define POT_OFFSET_MIN_VAL 130
#define POT_OFFSET_MAX_VAL 250

//gain range
#define EXT_GAIN_MIN_VAL 6700
#define EXT_GAIN_MAX_VAL 7400
#define POT_GAIN_MIN_VAL 570
#define POT_GAIN_MAX_VAL 640

#define TIMER_EXAMPLE_PERIOD 31250

//! Timer Counter Capture Compare Channel index
enum sensor_display_t {
	//! External voltage offset calibration
	GAIN_CALIBRATION = 1,
	//! Potentiometer offset calibration
	OFFSET_CALIBRATION = 2,
	//! Potentiometer Sensor
	POTENTIOMETER_SENSOR = 3,
	//! External Voltage Input
	EXT_VOLTAGE_INPUT = 4,
};

#endif // _MAIN_H_
