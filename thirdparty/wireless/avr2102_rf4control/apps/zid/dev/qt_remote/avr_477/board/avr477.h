/**
 * \file
 *
 * \brief AVR477 board header file.
 *
 * This file contains definitions and services related to the features of the
 * ATmega256RFR2 AVR477 board.
 *
 * To use this board, define BOARD= DUMMY_BOARD.
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
#ifndef _AVR477_
#define _AVR477_
#include "compiler.h"

#include "led.h"

#define MCU_SOC_NAME        "ATMEGA256RFR2"
#define BOARD_NAME          "AVR477"

/*! \name GPIO Connections of LED
 * LED0 is connected to PORTE pin 4
 */
 #define LED_ON_BOARD         IOPORT_CREATE_PIN(PORTE, 4)
 #define LED0_GPIO                LED_ON_BOARD
 #define LED0                 LED0_GPIO

/*!  \name GPIO Connections of Switch
 * Push button is connected to PORTE pin 3.
 */
 #define LED_ON_BOARD1        IOPORT_CREATE_PIN(PORTE, 3)
 #define LED1_GPIO                LED_ON_BOARD1
 #define LED1                 LED1_GPIO

#endif  /* _AVR477_ */
