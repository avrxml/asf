/*****************************************************************************
 *
 * \file
 *
 * \brief PEVC ID for AT32UC3C* devices
 * as event user.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _UC3C_PEVC_IDS_H_
#define _UC3C_PEVC_IDS_H_

// GENERATORS
#ifdef AVR32_PEVC_ID_GEN_PAD_0
#error AVR32_PEVC_ID_GEN_PAD_0 definition above can be removed, now present in header files.
#else
#define  AVR32_PEVC_ID_GEN_PAD_0  0
#endif

#ifdef AVR32_PEVC_ID_GEN_PAD_1
#error AVR32_PEVC_ID_GEN_PAD_1 definition above can be removed, now present in header files.
#else
#define  AVR32_PEVC_ID_GEN_PAD_1  1
#endif

#define  AVR32_PEVC_ID_GEN_PAD_2  2
#define  AVR32_PEVC_ID_GEN_PAD_3  3
#define  AVR32_PEVC_ID_GEN_PAD_4  4
#define  AVR32_PEVC_ID_GEN_PAD_5  5
#define  AVR32_PEVC_ID_GEN_PAD_6  6
#define  AVR32_PEVC_ID_GEN_PAD_7  7
#define  AVR32_PEVC_ID_GEN_PAD_8  8
#define  AVR32_PEVC_ID_GEN_PAD_9  9
#define  AVR32_PEVC_ID_GEN_PAD_10  10
#define  AVR32_PEVC_ID_GEN_PAD_11  11
#define  AVR32_PEVC_ID_GEN_PAD_12  12
#define  AVR32_PEVC_ID_GEN_PAD_13  13
#define  AVR32_PEVC_ID_GEN_PAD_14  14
#define  AVR32_PEVC_ID_GEN_PAD_15  15
#define  AVR32_PEVC_ID_GEN_GCLK_0  16
#define  AVR32_PEVC_ID_GEN_GCLK_1  17
#define  AVR32_PEVC_ID_GEN_TC0_A0  18
#define  AVR32_PEVC_ID_GEN_TC0_A1  19
#define  AVR32_PEVC_ID_GEN_TC0_A2  20
#define  AVR32_PEVC_ID_GEN_TC0_B0  21
#define  AVR32_PEVC_ID_GEN_TC0_B1  22
#define  AVR32_PEVC_ID_GEN_TC0_B2  23
#define  AVR32_PEVC_ID_GEN_ACIFA0_0  24
#define  AVR32_PEVC_ID_GEN_ACIFA0_1  25
#define  AVR32_PEVC_ID_GEN_ACIFA1_0  26
#define  AVR32_PEVC_ID_GEN_ACIFA1_1  27
#define  AVR32_PEVC_ID_GEN_AST_ALARM0  28
#define  AVR32_PEVC_ID_GEN_AST_PER0  29
#define  AVR32_PEVC_ID_GEN_PWM_0  30
#define  AVR32_PEVC_ID_GEN_PWM_1  31
#define  AVR32_PEVC_ID_GEN_QDEC0_CMP  32
#define  AVR32_PEVC_ID_GEN_QDEC1_CMP  33

// USERS / CHANNELS

#define  AVR32_PEVC_ID_USER_ADCIFA_0      0
#define  AVR32_PEVC_ID_USER_ADCIFA_1      1
#define  AVR32_PEVC_ID_USER_DACIFB0_CHA   2
#define  AVR32_PEVC_ID_USER_DACIFB0_CHB   3
#define  AVR32_PEVC_ID_USER_DACIFB1_CHA   4
#define  AVR32_PEVC_ID_USER_DACIFB1_CHB   5
#define  AVR32_PEVC_ID_USER_PDCA_0        6
#define  AVR32_PEVC_ID_USER_PDCA_1        7
#define  AVR32_PEVC_ID_USER_PWM_0         8
#define  AVR32_PEVC_ID_USER_PWM_1         9
#define  AVR32_PEVC_ID_USER_QDEC0_CAPT    10
#define  AVR32_PEVC_ID_USER_QDEC0_TGLDIR  11
#define  AVR32_PEVC_ID_USER_QDEC0_TRIGGER 12
#define  AVR32_PEVC_ID_USER_QDEC1_CAPT    13
#define  AVR32_PEVC_ID_USER_QDEC1_TGLDIR  14
#define  AVR32_PEVC_ID_USER_QDEC1_TRIGGER 15
#define  AVR32_PEVC_ID_USER_TC0_A0        16
#define  AVR32_PEVC_ID_USER_TC0_A1        17
#define  AVR32_PEVC_ID_USER_TC0_A2        18
#define  AVR32_PEVC_ID_USER_TC0_B0        19
#define  AVR32_PEVC_ID_USER_TC0_B1        20
#define  AVR32_PEVC_ID_USER_TC0_B2        21

#endif // _UC3C_PEVC_IDS_H_
