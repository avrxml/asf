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
 * Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
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
