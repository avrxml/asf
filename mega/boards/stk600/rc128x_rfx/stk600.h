/**
 * \file
 *
 * \brief STK board header file.
 *
 * This file contains definitions and services related to the features of the
 * STK600 board.
 *
 * To use this board, define BOARD= STK600_MEGA.
 *
 * Copyright (c) 2012-2018 Microchip Technology Inc. and its subsidiaries.
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
#ifndef _STK600_H_
#define _STK600_H_
#include "compiler.h"

# include "led.h"

/*! \name GPIO Connections of LEDs
 * LEDS connector is connected to PORTB
 * \note
 * Caution! Please remove AREF0 Jumper on STK600.
 */

#define LED0_GPIO            IOPORT_CREATE_PIN(PORTB, 0)
#define LED1_GPIO            IOPORT_CREATE_PIN(PORTB, 1)
#define LED2_GPIO            IOPORT_CREATE_PIN(PORTB, 2)
#define LED3_GPIO            IOPORT_CREATE_PIN(PORTB, 3)
#define LED4_GPIO            IOPORT_CREATE_PIN(PORTB, 4)
#define LED5_GPIO            IOPORT_CREATE_PIN(PORTB, 5)
#define LED6_GPIO            IOPORT_CREATE_PIN(PORTB, 6)
#define LED7_GPIO            IOPORT_CREATE_PIN(PORTB, 7)

#define LED0                 LED0_GPIO
#define LED1                 LED1_GPIO
#define LED2                 LED2_GPIO
#define LED3                 LED3_GPIO
#define LED4                 LED4_GPIO
#define LED5                 LED5_GPIO
#define LED6                 LED6_GPIO
#define LED7                 LED7_GPIO

/*! \name GPIO Connections of Push Buttons. To use these defines, connect the
 * STK600 PORTD connector to the SWITCHES connector.
 */
/* ! @{ */
#define GPIO_PUSH_BUTTON_0    IOPORT_CREATE_PIN(PORTD, 0)
#define GPIO_PUSH_BUTTON_1    IOPORT_CREATE_PIN(PORTD, 1)
#define GPIO_PUSH_BUTTON_2    IOPORT_CREATE_PIN(PORTD, 2)
#define GPIO_PUSH_BUTTON_3    IOPORT_CREATE_PIN(PORTD, 3)
#define GPIO_PUSH_BUTTON_4    IOPORT_CREATE_PIN(PORTD, 4)
#define GPIO_PUSH_BUTTON_5    IOPORT_CREATE_PIN(PORTD, 5)
#define GPIO_PUSH_BUTTON_6    IOPORT_CREATE_PIN(PORTD, 6)
#define GPIO_PUSH_BUTTON_7    IOPORT_CREATE_PIN(PORTD, 7)

/* ! @} */

/*! \name External oscillator
 */
/* @{ */
#define BOARD_EXTERNAL_CLK     32000000UL
/* @} */

#if MEGA_RF
/* ! LED ON STK600-ATMEGA128RFA1 EVK. */
#define LED_GREEN_GPIO       IOPORT_CREATE_PIN(PORTE, 2)
#define LED_YELLOW_GPIO      IOPORT_CREATE_PIN(PORTE, 3)
#define LED_RED_GPIO         IOPORT_CREATE_PIN(PORTE, 4)

/* ! PUSH BUTTON ON STK600-ATMEGA128RFA1 EVK. */
#define GPIO_PUSH_BUTTON_ON_BOARD    IOPORT_CREATE_PIN(PORTE, 5)

/*! \name SPI Connections of the AT45DBX Data Flash Memory. To use these defines,
 * connect :
 * - PB0 to /CS pin on DataFlash connector
 * - PB1 to SCK pin on DataFlash connector
 * - PB3 to SO pin on DataFlash connector
 * - PB2 to SI pin on DataFlash connector
 */
//! @{
#define AT45DBX_SPI           &SPCR
#define AT45DBX_CS            IOPORT_CREATE_PIN(PORTB,0)  // CS as output
#define AT45DBX_MASTER_SS     IOPORT_CREATE_PIN(PORTB,0)  // SS as output and overlayed with CS
#define AT45DBX_MASTER_SCK    IOPORT_CREATE_PIN(PORTB,1)  // SCK as output
#define AT45DBX_MASTER_MOSI   IOPORT_CREATE_PIN(PORTB,2)  // MOSI as output
#define AT45DBX_MASTER_MISO   IOPORT_CREATE_PIN(PORTB,3)  // MISO as input
//! @}

#endif

#endif  /* _STK600_H_ */
