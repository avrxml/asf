/*****************************************************************************
 *
 * \file
 *
 * \brief AT42QT1060 configuration.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

#ifndef _CONF_AT42QT1060_H_
#define _CONF_AT42QT1060_H_


#include "board.h"

#if BOARD == UC3C_EK

#define AT42QT1060_DETECT_PIN       AVR32_PIN_PC07

#define AT42QT1060_TWI              (&AVR32_TWIM0)
#define AT42QT1060_TWI_SCL_PIN		AVR32_TWIMS0_TWCK_0_0_PIN
#define AT42QT1060_TWI_SCL_FUNCTION	AVR32_TWIMS0_TWCK_0_0_FUNCTION
#define AT42QT1060_TWI_SDA_PIN		AVR32_TWIMS0_TWD_0_0_PIN
#define AT42QT1060_TWI_SDA_FUNCTION	AVR32_TWIMS0_TWD_0_0_FUNCTION

#else

#error 'Board not supported'

#endif

/*! The AT42QT1060 can do max 100kHz on the TWI. */
#define AT42QT1060_TWI_MASTER_SPEED 100000

#define AT42QT1060_KEY_MASK_VALUE 0x3F
/*! Threshold value for touch sensors.
 *  A measured value above this value will count as a detect
 */
#if BOARD == UC3C_EK
#define AT42QT1060_KEY_0_NTHR_VALUE 15
#define AT42QT1060_KEY_1_NTHR_VALUE 20
#define AT42QT1060_KEY_2_NTHR_VALUE 20
#define AT42QT1060_KEY_3_NTHR_VALUE 25
#define AT42QT1060_KEY_4_NTHR_VALUE 15
#define AT42QT1060_KEY_5_NTHR_VALUE 10
#endif
/*! Number of consecutive measurements that must be confirmed as having
 * passed the key threshold before that key is registered as being in detect.
 */
#define AT42QT1060_DETECT_INTEGRATOR_VALUE 3

/*! The I2C address is fixed for the AT42QT1060 device. */
#define AT42QT1060_TWI_ADDRESS 0x12

#endif /* _CONF_AT42QT1060_H_ */
