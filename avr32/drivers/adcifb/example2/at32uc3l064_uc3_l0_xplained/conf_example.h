/**
 * \file
 *
 * \brief Application Configuration
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
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

/**
 * \name ADCIFB channel configuration
 */
/*! @{ */
/* Connection of the NTC temperature sensor */

#define EXAMPLE_ADCIFB_PIN          ADC_TEMPERATURE_PIN
#define EXAMPLE_ADCIFB_FUNCTION     ADC_TEMPERATURE_FUNCTION
#define EXAMPLE_ADCIFB_CHANNEL      ADC_TEMPERATURE_CHANNEL

/* Alternate Inputs - Connect input to P -> J2.5 in UC3_L0_XPLAINED */

/*
 * #define EXAMPLE_ADCIFB_PIN           AVR32_ADCIFB_AD_7_PIN
 * #define EXAMPLE_ADCIFB_FUNCTION      AVR32_ADCIFB_AD_7_FUNCTION
 * #define EXAMPLE_ADCIFB_CHANNEL       AVR32_ADCIFB_CH7_MASK
 */
/*! @} */
#endif /* CONF_EXAMPLE_H */
