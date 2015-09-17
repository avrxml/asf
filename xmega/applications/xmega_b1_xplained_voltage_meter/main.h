/**
 * \file
 *
 * \brief Main functions for the Voltage meter demonstration with ADC calibration.
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
