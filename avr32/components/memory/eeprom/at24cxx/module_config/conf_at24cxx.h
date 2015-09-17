/*****************************************************************************
 *
 * \file
 *
 * \brief AT24Cxx configuration.
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
