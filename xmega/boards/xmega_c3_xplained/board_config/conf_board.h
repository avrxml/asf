/**
 * \file
 *
 * \brief XMEGA-C3 Xplained board configuration template
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H

// Initialize SPI and control pins for UG_2832HSWEG04 OLED controller
#define CONF_BOARD_OLED_UG_2832HSWEG04

// Initialize SPI pins and presence pin for MicroSD card slot
#define CONF_BOARD_SD_MMC_SPI

// Initialize Analog Comparator pin for light sensor
#define CONF_BOARD_LIGHT_SENSOR

// Initialize Analog Comparator pin for NTC sensor
#define CONF_BOARD_TEMPERATURE_SENSOR

// Initialize Analog Comparator pin and input signal pin
// for Analog Filter (lowpass RC @ 159 Hz)
#define CONF_BOARD_ANALOG_FILTER

// Initialize IO pins for use with USART 0 on port C
#define CONF_BOARD_ENABLE_USARTC0

// Initialize IO pins for use with USART 0 on port D
#define CONF_BOARD_ENABLE_USARTD0

// Initialize IO pins for use with USART 0 on port E
#define CONF_BOARD_ENABLE_USARTE0

#endif // CONF_BOARD_H
