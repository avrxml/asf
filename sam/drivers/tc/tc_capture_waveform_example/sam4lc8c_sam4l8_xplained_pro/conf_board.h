/**
 * \file
 *
 * \brief Configuration File for SAM4L8 Xplained Pro Board.
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
#define CONF_BOARD_COM_PORT

/**
 * \note For test the example on SAM4L8 Xplained Pro, we need to connect pin6
 * in EXT1 and pin7 in EXT2.
 */

#define TC             TC1
#define TC_PERIPHERAL  1

/** Configure TC1 channel 2 as waveform output. */
#define TC_CHANNEL_WAVEFORM  2
#define ID_TC_WAVEFORM       TC1
#define PIN_TC_WAVEFORM      PIN_PC04D_TC1_A2
#define PIN_TC_WAVEFORM_MUX  MUX_PC04D_TC1_A2

/** Configure TC1 channel 1 as capture input. */
#define TC_CHANNEL_CAPTURE  1
#define ID_TC_CAPTURE       TC1
#define PIN_TC_CAPTURE      PIN_PC02D_TC1_A1
#define PIN_TC_CAPTURE_MUX  MUX_PC02D_TC1_A1

/** Use TC11_Handler for TC capture interrupt */
#define TC_Handler  TC11_Handler
#define TC_IRQn     TC11_IRQn

#endif  /* CONF_BOARD_H_INCLUDED */
