/**
 * \file
 *
 * \brief ATMEGA328PB_XPLAINED_MINI board header file.
 *
 * This file contains definitions and services related to the features of the
 * Xplained Mini board.
 *
 * To use this board, define BOARD= ATMEGA328PB_XPLAINED_MINI.
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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
#ifndef _ATMEGA328PB_XPLAINED_MINI_
#define _ATMEGA328PB_XPLAINED_MINI_
#include "compiler.h"

# include "led.h"

#define MCU_SOC_NAME        "ATMEGA328PB"
#define BOARD_NAME          "ATMEGA328PB_XPLAINED_MINI"


#define LED0_GPIO                       IOPORT_CREATE_PIN(PORTB, 5)
#define LED0                            LED0_GPIO

//! Number of LEDs.
#define LED_COUNT                       1

#define GPIO_PUSH_BUTTON_0              IOPORT_CREATE_PIN(PORTB, 7)

//! \name Communication interfaces
//@{
#define TWID_SDA                        IOPORT_CREATE_PIN(PORTC, 4)
#define TWID_SCL                        IOPORT_CREATE_PIN(PORTC, 5)
#define USART_RX                        IOPORT_CREATE_PIN(PORTD, 0)
#define USART_TX                        IOPORT_CREATE_PIN(PORTD, 1)
#define SPI_SS_A                        IOPORT_CREATE_PIN(PORTB, 2)
#define SPI_MOSI                        IOPORT_CREATE_PIN(PORTB, 3)
#define SPI_MISO                        IOPORT_CREATE_PIN(PORTB, 4)
#define SPI_SCK                         IOPORT_CREATE_PIN(PORTB, 5)
//@}
#endif  /* _ATMEGA328PB_XPLAINED_MINI_ */
