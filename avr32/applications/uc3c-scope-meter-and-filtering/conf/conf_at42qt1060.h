/*****************************************************************************
 *
 * \file
 *
 * \brief AT42QT1060 configuration.
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _CONF_AT42QT1060_H_
#define _CONF_AT42QT1060_H_


#include "board.h"

#if BOARD == UC3C_EK

#define AT42QT1060_DETECT_PIN       AVR32_PIN_PC07

#define AT42QT1060_TWI              (&AVR32_TWIM0)
#define AT42QT1060_TWI_SCL_PIN      AVR32_TWIMS0_TWCK_0_0_PIN
#define AT42QT1060_TWI_SCL_FUNCTION AVR32_TWIMS0_TWCK_0_0_FUNCTION
#define AT42QT1060_TWI_SDA_PIN      AVR32_TWIMS0_TWD_0_0_PIN
#define AT42QT1060_TWI_SDA_FUNCTION AVR32_TWIMS0_TWD_0_0_FUNCTION

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
