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
