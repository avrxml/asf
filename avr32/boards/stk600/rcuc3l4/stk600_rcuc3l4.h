/**
 * \file
 *
 * \brief STK600 with the RCUC3L4 routing card header file.
 *
 * This file contains definitions and services related to the features of the
 * STK600 with the RCUC3L4 routing card.
 *
 * To use this board define BOARD=STK600_RCUC3L4.
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
#ifndef _STK600_RCUC3L4_H_
#define _STK600_RCUC3L4_H_

#include "compiler.h"
#include "led.h"
/**
 * \defgroup stk600_rcuc3l4_group STK600 with the RCUC3L4 routing card
 * @{
 */

/**
 * \defgroup stk600_rcuc3l4_feature_group Feature definitions
 * @{
 */

//! \name Miscellaneous data
//@{
//@}

/**
 * \name LEDs
 *
 * LED0,1,2 and 3 are single yellow LEDs that are active low.
 */
//@{
#define LED0_GPIO                       AVR32_PIN_PA19
#define LED1_GPIO                       AVR32_PIN_PA15
#define LED2_GPIO                       AVR32_PIN_PA16
#define LED3_GPIO                       AVR32_PIN_PA17
#define LED0                            LED0_GPIO
#define LED1                            LED1_GPIO
#define LED2                            LED2_GPIO
#define LED3                            LED3_GPIO
//! Number of LEDs.
#define LED_COUNT                       4
//@}

//! \name Push buttons
//@{
#define GPIO_PUSH_BUTTON_0              AVR32_PIN_PB06
#define GPIO_PUSH_BUTTON_0_PRESSED      0
#define GPIO_PUSH_BUTTON_1              AVR32_PIN_PB07
#define GPIO_PUSH_BUTTON_1_PRESSED      0
#define GPIO_PUSH_BUTTON_2              AVR32_PIN_PB08
#define GPIO_PUSH_BUTTON_2_PRESSED      0
//@}

//! \name DataFlash memory (AT45DBX)
//@{
#define AT45DBX_SPI                     (&AVR32_USART2)
#define AT45DBX_SPI_NPCS                0
//! SCK pin
#define AT45DBX_SPI_SCK_PIN             AVR32_USART2_CLK_0_1_PIN
#define AT45DBX_SPI_SCK_FUNCTION        AVR32_USART2_CLK_0_1_FUNCTION
//! MOSI pin
#define AT45DBX_SPI_MOSI_PIN            AVR32_USART2_TXD_0_0_PIN
#define AT45DBX_SPI_MOSI_FUNCTION       AVR32_USART2_TXD_0_0_FUNCTION
//! MISO pin
 #define AT45DBX_SPI_MISO_PIN           AVR32_USART2_RXD_0_0_PIN
#define AT45DBX_SPI_MISO_FUNCTION       AVR32_USART2_RXD_0_0_FUNCTION
//! CS pin
#define AT45DBX_SPI_NPCS0_PIN           AVR32_USART2_RTS_0_1_PIN
#define AT45DBX_SPI_NPCS0_FUNCTION      AVR32_USART2_RTS_0_1_FUNCTION
//@}

/*! \name USART connection to the UC3B board controller
 */
//! @{
#define USART                          (&AVR32_USART1)
#define USART_RXD_PIN                  AVR32_USART1_RXD_0_1_PIN
#define USART_RXD_FUNCTION             AVR32_USART1_RXD_0_1_FUNCTION
#define USART_TXD_PIN                  AVR32_USART1_TXD_0_1_PIN
#define USART_TXD_FUNCTION             AVR32_USART1_TXD_0_1_FUNCTION
#define USART_IRQ                      AVR32_USART1_IRQ
#define USART_IRQ_GROUP                16
#define USART_SYSCLK                   SYSCLK_USART1
//! @}

/**
 * \name External oscillator
 *
 */
//@{
//! RCOsc frequency: Hz.
#define FRCOSC                      (AVR32_SCIF_RCOSC_FREQUENCY)
//! Osc32 frequency (Hz.) and startup time (RCOsc periods).
#define FOSC32                      (32768)
#define OSC32_STARTUP               (AVR32_SCIF_OSCCTRL32_STARTUP_8192_RCOSC)
//! Osc frequency (Hz.) and startup time (RCOsc periods).
#define FOSC0                       (16000000)
#define OSC0_STARTUP                (AVR32_SCIF_OSCCTRL0_STARTUP_128_RCOSC)
//@}

/**
 * \name Board oscillator configuration
 *
 */
//@{
#define BOARD_OSC32_IS_XTAL         true
#define BOARD_OSC32_HZ              FOSC32
#define BOARD_OSC32_STARTUP_US      (71000)
#define BOARD_OSC0_IS_XTAL          true
#define BOARD_OSC0_HZ               FOSC0
#define BOARD_OSC0_STARTUP_US       (1100)
//@}

/**
 * \defgroup stk600_rcuc3l4_config_group Configuration options
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
 * \def CONF_BOARD_AT45DBX
 * \brief Initialize SPI pins for AT45DBX DataFlash
 */
#  if !defined(CONF_BOARD_AT45DBX)
#    define CONF_BOARD_AT45DBX
#  endif

/**
 * \def CONF_BOARD_COM_PORT
 * \brief Initialize USART pins for uart usage
 */
#  if !defined(CONF_BOARD_COM_PORT)
#    define CONF_BOARD_COM_PORT
#  endif
//@}

#endif // __DOXYGEN__

/**
 * @}
 */

/**
 * @}
 */

#endif /* _STK600_RCUC3L4_H_ */
