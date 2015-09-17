/**
 * \file
 *
 * \brief Board configuration.
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

#ifndef CONF_BOARD_H_INCLUDED
#define CONF_BOARD_H_INCLUDED

/** Enable Com Port. */
#define CONF_BOARD_UART_CONSOLE

/**
 * \note For test the example on SAMG55 Xplained Pro, we need to connect pin7
 * in EXT1 and pin8 in EXT1.
 */

/** Use TC Peripheral 0 **/
#define TC             TC0
#define TC_PERIPHERAL  0

/** Configure TC0 channel 0 as waveform output. **/
#define TC_CHANNEL_WAVEFORM 0
#define ID_TC_WAVEFORM      ID_TC0
#define PIN_TC_WAVEFORM     EXT1_PIN_PWM_0
#define PIN_TC_WAVEFORM_MUX EXT1_PWM_0_MUX

/** Configure TC0 channel 1 as capture input. **/
#define TC_CHANNEL_CAPTURE 1
#define ID_TC_CAPTURE ID_TC1
#define PIN_TC_CAPTURE EXT1_PIN_PWM_1
#define PIN_TC_CAPTURE_MUX EXT1_PWM_1_MUX

/** Use TC1_Handler for TC capture interrupt**/
#define TC_Handler  TC1_Handler
#define TC_IRQn     TC1_IRQn

#endif /* CONF_BOARD_H_INCLUDED */
