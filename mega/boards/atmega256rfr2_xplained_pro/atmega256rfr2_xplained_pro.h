/**
 * \file
 *
 * \brief ATmega256RFR2 Xplained Pro board header file.
 *
 * This file contains definitions and services related to the features of the
 * ATmega256RFR2 Xplained Pro board.
 *
 * To use this board, define BOARD= ATMEGA256RFR2_XPLAINED_PRO.
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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
#ifndef _ATMEGA256RFR2_XPLAINED_PRO_
#define _ATMEGA256RFR2_XPLAINED_PRO_
#include "compiler.h"

# include "led.h"

#define MCU_SOC_NAME        "ATMEGA256RFR2"
#define BOARD_NAME          "ATMEGA256RFR2-XPRO"

 /*! \name GPIO Connections of LED
 * LED0 is connected to PORTB pin 4
 */
 #define LED_ON_BOARD         IOPORT_CREATE_PIN(PORTB, 4)
 #define LED0_GPIO			  LED_ON_BOARD		  
 #define LED0                 LED0_GPIO
 #define LED_COUNT            1
 /*!  \name GPIO Connections of Switch
 * Push button is connected to PORTE pin 4. 
 */
 #define GPIO_PUSH_BUTTON_ON_BOARD    IOPORT_CREATE_PIN(PORTE, 4)
 #define GPIO_PUSH_BUTTON_0			  GPIO_PUSH_BUTTON_ON_BOARD 

#endif  /* _ATMEGA256RFR2_XPLAINED_PRO_ */
