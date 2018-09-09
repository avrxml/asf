/*****************************************************************************
 *
 * \file
 *
 * \brief AT24Cxx configuration.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _CONF_AT24CXX_H_
#define _CONF_AT24CXX_H_

#include "board.h"

#define AT24CXX_TWI                   (&AVR32_TWIM0)
#define AT24CXX_TWI_SCL_PIN           AVR32_TWIMS0_TWCK_0_0_PIN
#define AT24CXX_TWI_SCL_FUNCTION      AVR32_TWIMS0_TWCK_0_0_FUNCTION
#define AT24CXX_TWI_SDA_PIN           AVR32_TWIMS0_TWD_0_0_PIN
#define AT24CXX_TWI_SDA_FUNCTION      AVR32_TWIMS0_TWD_0_0_FUNCTION

/*! The I2C address is fixed for the AT24CXX device. */
#define AT24CXX_TWI_ADDRESS                 0x50

/*! The AT42QT1060 can do max 100kHz on the TWI. */
#define AT24CXX_TWI_MASTER_SPEED 100000


#endif /* _CONF_AT24CXX_H_ */
