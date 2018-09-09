/**
 * \file
 *
 * \brief AVR Demo Configuration File
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
#include "conf_clock.h"

#define BUFFER_LENGTH                256

#define DEFAULT_SCREEN_UPDATE_FS_MS  100

#define NUMBER_OF_INPUTS_ADC_SEQ0    1
#define INPUT1_ADC_CHANNEL           5
#define INPUT1_ADC_INP               AVR32_ADCIFA_INP_ADCIN5
#define INPUT1_ADC_INN               AVR32_ADCIFA_INN_GNDANA
#define INPUT1_ADC_PIN               AVR32_ADCIN5_PIN
#define INPUT1_ADC_FUNCTION          AVR32_ADCIN5_FUNCTION

#define NUMBER_OF_INPUTS_ADC_SEQ1    1
#define INPUT2_ADC_CHANNEL           14
#define INPUT2_ADC_INP               AVR32_ADCIFA_INP_GNDANA
#define INPUT2_ADC_INN               AVR32_ADCIFA_INN_ADCIN14
#define INPUT2_ADC_PIN               AVR32_ADCIN14_PIN
#define INPUT2_ADC_FUNCTION          AVR32_ADCIN14_FUNCTION

#define TC_INSTANCE                  &AVR32_TC0
#define TC_INSTANCE_IRQ              AVR32_TC0_IRQ0
#define TC_INSTANCE_IRQ_GROUP        AVR32_TC0_IRQ_GROUP
// Timer Counter Channel Used
#define TC_CHANNEL                   0

#endif //_CONF_DEMO_H_
