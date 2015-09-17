/**
 * \file
 *
 * \brief Demo configuration File.
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
#ifndef _CONF_DEMO_H_
#define _CONF_DEMO_H_

#include "board.h"
#include "conf_et024006dhu.h"
#include "sysclk.h"
//! Controller Configuration definitions
#define CONTROLLER_DEBOUNCE_MS     200

//! GUI Configuration definitions
#define GUI_BUFFER_LENGTH                     255
#define GUI_DEFAULT_SCREEN_UPDATE_FS_MS       100
#define GUI_SCALE_GAIN_VALUE                  100
#define GUI_SCALE_OFFSET_VALUE                0x8000

//! Signal Generator definition
#define SIGNAL_NB_SAMPLES                     SINUS_NB_SAMPLES
#define SIGNAL_TABLE                          sinus_table

//! Remote Task Configuration Parameters
#define REMOTE_PDCA_CHANNEL_USART     8
#define REMOTE_TXBUF_SIZE             ((4+GUI_BUFFER_LENGTH*4)*3)
#define REMOTE_USART                  (&AVR32_USART2)
#define REMOTE_USART_RX_PIN           AVR32_USART2_RXD_0_1_PIN
#define REMOTE_USART_RX_FUNCTION      AVR32_USART2_RXD_0_1_FUNCTION
#define REMOTE_USART_TX_PIN           AVR32_USART2_TXD_0_1_PIN
#define REMOTE_USART_TX_FUNCTION      AVR32_USART2_TXD_0_1_FUNCTION

//! Noise Task Configuration Parameters
#define NOISE_MAX_VALUE                       250
#define NOISE_MIN_VALUE                       5
#define NOISE_STARTUP_VALUE                   50
#define NOISE_STEP                            5

//! ADC Task Configuration Parameters
// Number of conversions per sequence
#define ADC_NUMBER_OF_CONV_SEQ0               1
#define ADC_NUMBER_OF_CONV_SEQ1               4
// Connection of the external analog input
#define ADC_EXT_INPUT_INP             AVR32_ADCIFA_INP_ADCIN2
#define ADC_EXT_INPUT_INN             AVR32_ADCIFA_INN_GNDANA
#define ADC_EXT_INPUT_PIN             AVR32_ADCIN2_PIN
#define ADC_EXT_INPUT_FUNCTION        AVR32_ADCIN2_FUNCTION
// Connection of the potentiometer sensor
#define ADC_POTENTIOMETER_INP         AVR32_ADCIFA_INP_ADCIN5
#define ADC_POTENTIOMETER_INN         AVR32_ADCIFA_INN_GNDANA
#define ADC_POTENTIOMETER_PIN         AVR32_ADCIN5_PIN
#define ADC_POTENTIOMETER_FUNCTION    AVR32_ADCIN5_FUNCTION
// PDCA Channel for SEQ0
#define ADC_PDCA_CHANNEL_SEQ0         0
// Number of Elements for PDCA Channel of SEQ0
#define ADC_PDCA_CHANNEL_NUMBER_OF_SAMPLES    \
	(GUI_BUFFER_LENGTH * ADC_NUMBER_OF_CONV_SEQ0)
#define ADC_SCALE_GAIN                        140
#define ADC_SCALE_OFFSET                      0x8000

//! DAC Task Configuration Parameters
#define DAC_OUTPUT_INSTANCE              0
#define DAC_OUTPUT_CHANNEL               DACIFB_CHANNEL_SELECTION_A
#define DAC_OUTPUT_PIN                   AVR32_DAC0A_PIN
#define DAC_OUTPUT_FUNCTION              AVR32_DAC0A_FUNCTION
#define DAC_PDCA_CHANNEL_DAC             4
#define DAC_PDCA_PID_DAC_TX              AVR32_PDCA_PID_DACIFB0_CHA_TX

#endif //_CONF_DEMO_H_
