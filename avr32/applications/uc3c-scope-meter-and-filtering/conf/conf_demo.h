/**
 * \file
 *
 * \brief Demo configuration File.
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
