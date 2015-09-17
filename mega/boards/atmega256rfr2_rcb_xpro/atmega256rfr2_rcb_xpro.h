/**
 * \file
 *
 * \brief ATMEGA256RFR2-Zigbit board header file.
 *
 * This file contains definitions and services related to the features of the
 * STK600 board.
 *
 * To use this board, define BOARD= ATMEGA256RFR2_ZIGBIT.
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
#ifndef _ATMEGA256RFR2_RCB_XPRO_
#define _ATMEGA256RFR2_RCB_XPRO_
#include "compiler.h"

# include "led.h"

#define MCU_SOC_NAME        "ATMEGA256RFR2"
#define BOARD_NAME          "RCB256RFR2-XPRO"

#define LED0_GPIO                       IOPORT_CREATE_PIN(PORTD, 6)
#define LED1_GPIO                       IOPORT_CREATE_PIN(PORTE, 2)
#define LED2_GPIO                       IOPORT_CREATE_PIN(PORTG, 2)

#define LED0                            LED0_GPIO
#define LED1                            LED1_GPIO
#define LED2                            LED2_GPIO

//! Number of LEDs.
#define LED_COUNT                       3

#define GPIO_PUSH_BUTTON_0              IOPORT_CREATE_PIN(PORTE, 0)

//! \name Communication interfaces on header J1
//@{
#define TWID_SDA                        IOPORT_CREATE_PIN(PORTD, 1)
#define TWID_SCL                        IOPORT_CREATE_PIN(PORTD, 0)
#define USARTA1_RXD                     IOPORT_CREATE_PIN(PORTD, 2)
#define USARTA1_TXD                     IOPORT_CREATE_PIN(PORTD, 3)
#define SPIB_SS                         IOPORT_CREATE_PIN(PORTB, 0)
#define SPIB_MOSI                       IOPORT_CREATE_PIN(PORTB, 2)
#define SPIB_MISO                       IOPORT_CREATE_PIN(PORTB, 3)
#define SPIB_SCK                        IOPORT_CREATE_PIN(PORTB, 1)
//@}



//@}
#endif  /* _ATMEGA256RFR2_RCB_XPRO_ */
