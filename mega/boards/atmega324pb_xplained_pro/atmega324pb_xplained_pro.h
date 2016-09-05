/**
 * \file
 *
 * \brief ATMEGA324PB_XPLAINED_PRO board header file.
 *
 * This file contains definitions and services related to the features of the
 * Xplained Pro board.
 *
 * To use this board, define BOARD = ATMEGA324PB_XPLAINED_PRO.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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
#ifndef _ATMEGA324PB_XPLAINED_PRO_
#define _ATMEGA324PB_XPLAINED_PRO_
#include "compiler.h"

# include "led.h"

#define MCU_SOC_NAME        "ATMEGA324PB"
#define BOARD_NAME          "ATMEGA324PB_XPLAINED_PRO"


#define LED0_GPIO                       IOPORT_CREATE_PIN(PORTC, 7)
#define LED0                            LED0_GPIO

//! Number of User LEDs.
#define LED_COUNT                       1

#define GPIO_PUSH_BUTTON_0              IOPORT_CREATE_PIN(PORTC, 6)

//! \name Communication interfaces
//@{
#define TWID_SDA                        IOPORT_CREATE_PIN(PORTE, 5)
#define TWID_SCL                        IOPORT_CREATE_PIN(PORTE, 6)
#define USART_RX                        IOPORT_CREATE_PIN(PORTD, 2)
#define USART_TX                        IOPORT_CREATE_PIN(PORTD, 3)
#define SPI0_SS_A                       IOPORT_CREATE_PIN(PORTB, 4)
#define SPI0_MOSI                       IOPORT_CREATE_PIN(PORTB, 5)
#define SPI0_MISO                       IOPORT_CREATE_PIN(PORTB, 6)
#define SPI0_SCK                        IOPORT_CREATE_PIN(PORTB, 7)
#define SPI1_SS_A                       IOPORT_CREATE_PIN(PORTD, 6)
//@}
#endif  /* _ATMEGA324PB_XPLAINED_PRO_ */
