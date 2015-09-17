/**
 * \file
 *
 * \brief XMEGA-E5-XPLAINED board header file.
 *
 * This file contains definitions and services related to the features of the
 * XMEGA-E5 Xplained board.
 *
 * To use this board define BOARD=XMEGA_E5_XPLAINED.
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
#ifndef _XMEGA_E5_XPLAINED_H_
#define _XMEGA_E5_XPLAINED_H_

#include <compiler.h>

/**
 * \defgroup xmega_e5_xplained_group XMEGA-E5 Xplained board
 * @{
 */

/**
 * \defgroup xmega_e5_xplained_feature_group Feature definitions
 * @{
 */

/**
 * \name LEDs
 *
 * LED0 and LED1 are single yellow LEDs that are active low..
 *
 * Note: the LEDs inside one package (Led red and green) close
 * to USB connector are connected to board controller.
 */
//@{
#define LED0_GPIO                       IOPORT_CREATE_PIN(PORTD, 4)
#define LED1_GPIO                       IOPORT_CREATE_PIN(PORTD, 5)
#define LED0                            LED0_GPIO
#define LED1                            LED1_GPIO
//! Number of LEDs.
#define LED_COUNT                       2
//@}

/**
 * \name Push buttons
 *
 * BUTTON_0 and BUTTON_1 are the SW0 and SW1 mecanical buttons
 * on the righ side of board.
 * BUTTON_2 is the SW2 mecanical buttons on the quadrature encoder.
 */
//@{
#define GPIO_PUSH_BUTTON_0              IOPORT_CREATE_PIN(PORTD, 0)
#define GPIO_PUSH_BUTTON_1              IOPORT_CREATE_PIN(PORTD, 2)
#define GPIO_PUSH_BUTTON_2              IOPORT_CREATE_PIN(PORTA, 5)
//@}

/**
 * \name Quadrature encoder
 */
//@{
#define QUADRATURE_ENCODER_LINE_A       IOPORT_CREATE_PIN(PORTA, 6)
#define QUADRATURE_ENCODER_LINE_B       IOPORT_CREATE_PIN(PORTA, 7)
//@}

//! \name Light sensor
//@{
#define LIGHT_SENSOR_ADC_MODULE         ADCA
#define LIGHT_SENSOR_ADC_INPUT          ADCCH_POS_PIN9
#define LIGHT_SENSOR_SIGNAL_PIN         IOPORT_CREATE_PIN(PORTD, 1)
//@}



//! \name OLED
//@{
#define UG_2832HSWEG04_SPI              &USARTC0
#define UG_2832HSWEG04_SPI_SCK          IOPORT_CREATE_PIN(PORTC, 5)
#define UG_2832HSWEG04_SPI_MOSI         IOPORT_CREATE_PIN(PORTC, 7)
#define UG_2832HSWEG04_SS               IOPORT_CREATE_PIN(PORTR, 1)
#define UG_2832HSWEG04_DATA_CMD         IOPORT_CREATE_PIN(PORTR, 0)
#define UG_2832HSWEG04_RESET            IOPORT_CREATE_PIN(PORTD, 3)
#define UG_2832HSWEG04_BAUDRATE         12000000
//! \name OLED dimensions
//@{
#define LCD_WIDTH_PIXELS                (128)
#define LCD_HEIGHT_PIXELS               (32)
//@}
//@}



//! \name BOOST and LED demonstration top module
//@{
#define TOP_BLED_PWM_STRING1            IOPORT_CREATE_PIN(PORTC, 0)
#define TOP_BLED_PWM_STRING2            IOPORT_CREATE_PIN(PORTC, 1)
#define TOP_BLED_PWM_STRING3            IOPORT_CREATE_PIN(PORTC, 2)
#define TOP_BLED_PWM_STRING4            IOPORT_CREATE_PIN(PORTC, 3)
#define TOP_BLED_PWM_STRING5            IOPORT_CREATE_PIN(PORTC, 4)
#define TOP_BLED_PWM_STRING6            IOPORT_CREATE_PIN(PORTC, 5)
#define TOP_BLED_LED_FAULT_STRING       IOPORT_CREATE_PIN(PORTA, 1)
#define TOP_BLED_LED_FAULT_LED          IOPORT_CREATE_PIN(PORTA, 2)
#define TOP_BLED_SHUNT_AC_MODULE        ACA
#define TOP_BLED_SHUNT_AC_INPUT         AC_POS_PIN0
#define TOP_BLED_SHUNT_PIN              IOPORT_CREATE_PIN(PORTA, 0)
//@}



//! \name Communication interfaces on header J1
//@{
#define TWIC_SDA                        IOPORT_CREATE_PIN(PORTC, 0)
#define TWIC_SCL                        IOPORT_CREATE_PIN(PORTC, 1)
#define USARTC0_RXD                     IOPORT_CREATE_PIN(PORTC, 2)
#define USARTC0_TXD                     IOPORT_CREATE_PIN(PORTC, 3)
#define SPIC_SS                         IOPORT_CREATE_PIN(PORTC, 4)
#define SPIC_MOSI                       IOPORT_CREATE_PIN(PORTC, 5)
#define SPIC_MISO                       IOPORT_CREATE_PIN(PORTC, 6)
#define SPIC_SCK                        IOPORT_CREATE_PIN(PORTC, 7)
//@}

/**
 * \name Pin connections on header J1
 *
 * The whole port C is directly connected to J1.
 *
 * \note For the TWI lines there are footprints for pull up resistors, which
 * are by default not mounted on the board.
 *
 * \note PIN5 and PIN7 are swapped on XMEGA E versus other XMEGA devices.
 */
//@{
#define J1_PIN0                        IOPORT_CREATE_PIN(PORTC, 0)
#define J1_PIN1                        IOPORT_CREATE_PIN(PORTC, 1)
#define J1_PIN2                        IOPORT_CREATE_PIN(PORTC, 2)
#define J1_PIN3                        IOPORT_CREATE_PIN(PORTC, 3)
#define J1_PIN4                        IOPORT_CREATE_PIN(PORTC, 4)
#define J1_PIN5                        IOPORT_CREATE_PIN(PORTC, 7)
#define J1_PIN6                        IOPORT_CREATE_PIN(PORTC, 6)
#define J1_PIN7                        IOPORT_CREATE_PIN(PORTC, 5)
//@}

/**
 * \name Pin connections on header J2
 *
 * The whole port A is directly connected to J2.
 *
 * The port pins are connected directly to this header and are not shared with
 * any on-board functionality.
 */
//@{
#define J2_PIN0                        IOPORT_CREATE_PIN(PORTA, 0)
#define J2_PIN1                        IOPORT_CREATE_PIN(PORTA, 1)
#define J2_PIN2                        IOPORT_CREATE_PIN(PORTA, 2)
#define J2_PIN3                        IOPORT_CREATE_PIN(PORTA, 3)
#define J2_PIN4                        IOPORT_CREATE_PIN(PORTA, 4)
#define J2_PIN5                        IOPORT_CREATE_PIN(PORTA, 5)
#define J2_PIN6                        IOPORT_CREATE_PIN(PORTA, 6)
#define J2_PIN7                        IOPORT_CREATE_PIN(PORTA, 7)
//@}

/**
 * \name Pin connections on header J3
 *
 * The lower half of port C is connected to the higher pins of J3
 * while 2 bits of port R are connected to the lower pins of J3.
 * Also the lines A & B of quadrature encoder are connected on PIN2 and PIN3
 * if switch 3 is down.
 *
 */
//@{
#define J3_PIN0                        IOPORT_CREATE_PIN(PORTR, 0)
#define J3_PIN1                        IOPORT_CREATE_PIN(PORTR, 1)
// J3_PIN2 = quadrature encoder line A
// J3_PIN3 = quadrature encoder line B
#define J3_PIN4                        IOPORT_CREATE_PIN(PORTC, 4)
#define J3_PIN5                        IOPORT_CREATE_PIN(PORTC, 7)
#define J3_PIN6                        IOPORT_CREATE_PIN(PORTC, 6)
#define J3_PIN7                        IOPORT_CREATE_PIN(PORTC, 5)
//@}

/**
 * \name Pin connections on header J4
 *
 * The whole port D is connected to J4.
 */
//@{
#define J4_PIN0                        IOPORT_CREATE_PIN(PORTD, 0)
#define J4_PIN1                        IOPORT_CREATE_PIN(PORTD, 1)
#define J4_PIN2                        IOPORT_CREATE_PIN(PORTD, 2)
#define J4_PIN3                        IOPORT_CREATE_PIN(PORTD, 3)
#define J4_PIN4                        IOPORT_CREATE_PIN(PORTD, 4)
#define J4_PIN5                        IOPORT_CREATE_PIN(PORTD, 7)
#define J4_PIN6                        IOPORT_CREATE_PIN(PORTD, 6)
#define J4_PIN7                        IOPORT_CREATE_PIN(PORTD, 5)
//@}

/**
 * @}
 */

/**
 * \defgroup xmega_e5_xplained_config_group Configuration options
 * @{
 */

#if defined(__DOXYGEN__)

/**
 * \name Initialization
 * \note Define these symbols in \ref conf_board.h to enable the corresponding
 * features.
 */
//@{

/**
 * \def CONF_BOARD_OLED_UG_2832HSWEG04
 * \brief Initialize SPI and control pins for UG_2832HSWEG04 OLED controller
 */
#  if !defined(CONF_BOARD_OLED_UG_2832HSWEG04)
#    define CONF_BOARD_OLED_UG_2832HSWEG04
#  endif


/**
 * \def CONF_BOARD_LIGHT_SENSOR
 * \brief Initialize Analog Comparator pin for light sensor
 */
#  if !defined(CONF_BOARD_LIGHT_SENSOR)
#    define CONF_BOARD_LIGHT_SENSOR
#  endif

/**
 * \def CONF_BOARD_QUADRATURE_ENCODER
 * \brief Initialize Quadrature encoder pins
 */
#  if !defined(CONF_BOARD_QUADRATURE_ENCODER)
#    define CONF_BOARD_QUADRATURE_ENCODER
#  endif

/**
 * \def CONF_BOARD_ENABLE_USARTC0
 * \brief Initialize IO pins for USART 0 on pins 2 and 3 of port C
 */
#  if !defined(CONF_BOARD_ENABLE_USARTC0)
#    define CONF_BOARD_ENABLE_USARTC0
#  endif

/**
 * \def CONF_BOARD_ENABLE_USARTC0_REMAP
 * \brief Initialize IO pins for USART 0 on pins 6 and 7 of port C
 */
#  if !defined(CONF_BOARD_ENABLE_USARTC0_REMAP)
#    define CONF_BOARD_ENABLE_USARTC0_REMAP
#  endif

/**
 * \def CONF_BOARD_ENABLE_USARTD0
 * \brief Initialize IO pins for USART 0 on pins 2 and 3 of port D
 */
#  if !defined(CONF_BOARD_ENABLE_USARTD0)
#    define CONF_BOARD_ENABLE_USARTD0
#  endif

/**
 * \def CONF_BOARD_ENABLE_USARTD0_REMAP
 * \brief Initialize IO pins for USART 0 on pins 6 and 7 of port D
 */
#  if !defined(CONF_BOARD_ENABLE_USARTD0_REMAP)
#    define CONF_BOARD_ENABLE_USARTD0_REMAP
#  endif

//@}

#endif // __DOXYGEN__

/**
 * @}
 */

/**
 * @}
 */

#endif /* _XMEGA_E5_XPLAINED_H_ */
