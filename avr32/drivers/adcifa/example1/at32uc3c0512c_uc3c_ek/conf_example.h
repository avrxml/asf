/**
 * \file
 *
 * \brief ADCIFA driver example configuration
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
#ifndef CONF_EXAMPLE_H
#define CONF_EXAMPLE_H

#define EXAMPLE_ADCIFA_NUMBER_OF_SEQUENCE     2

#define EXAMPLE_ADC_POTENTIOMETER_INP         AVR32_ADCIFA_INP_ADCIN5
#define EXAMPLE_ADC_POTENTIOMETER_INN         AVR32_ADCIFA_INN_GNDANA
#define EXAMPLE_ADC_POTENTIOMETER_PIN         AVR32_ADCIN5_PIN
#define EXAMPLE_ADC_POTENTIOMETER_FUNCTION    AVR32_ADCIN5_FUNCTION

#define EXAMPLE_ADC_MIC_INP                   AVR32_ADCIFA_INP_GNDANA
#define EXAMPLE_ADC_MIC_INN                   AVR32_ADCIFA_INN_ADCIN14
#define EXAMPLE_ADC_MIC_PIN                   AVR32_ADCIN14_PIN
#define EXAMPLE_ADC_MIC_FUNCTION              AVR32_ADCIN14_FUNCTION

#endif /* CONF_EXAMPLE_H */
