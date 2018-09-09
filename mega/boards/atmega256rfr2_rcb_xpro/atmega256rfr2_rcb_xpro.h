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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
