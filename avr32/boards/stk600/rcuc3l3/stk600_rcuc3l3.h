/**
 * \file
 *
 * \brief STK600 with the RCUC3L3 routing card header file.
 *
 * This file contains definitions and services related to the features of the
 * STK600 with the RCUC3L3 routing card. As the STK600 Leds, switches, clocks 
 * are configurable (hardware through connectors and software through 
 * Atmel Studio6), the proposed definitions below should be considered as 
 * examples and may be modified for a given application.
 *
 * To use this board define BOARD=STK600_RCUC3L3.
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
#ifndef _STK600_RCUC3L3_H_
#define _STK600_RCUC3L43_H_

#include "compiler.h"
#include "led.h"
/**
 * \defgroup stk600_rcuc3l3_group STK600 with the RCUC3L3 routing card
 * @{
 */

/**
 * \defgroup stk600_rcuc3l3_feature_group Feature definitions
 * @{
 */

//! \name Miscellaneous data
//@{
//@}

/*! \name GPIO Connections of LEDs. 
 * To use these defines, connect the STK600 PORTE connectors to respectively 
 * the LEDs LED0-to-LED7 connectors. Note that when the JTAG is active, 
 * it uses pins PA00-to-PA03 of the UC3L. So we should not use pins 
 * PA00-to-PA03 for the LEDs.
 */
//@{
#define LED0_GPIO   AVR32_PIN_PB08  // STK600.PORTE.PE0 -> STK600.LEDS.LED0
#define LED1_GPIO   AVR32_PIN_PB09  // STK600.PORTE.PE1 -> STK600.LEDS.LED1
#define LED2_GPIO   AVR32_PIN_PB10  // STK600.PORTE.PE2 -> STK600.LEDS.LED2
#define LED3_GPIO   AVR32_PIN_PB11  // STK600.PORTE.PE3 -> STK600.LEDS.LED3
#define LED4_GPIO   AVR32_PIN_PB12  // STK600.PORTE.PE4 -> STK600.LEDS.LED4
#define LED5_GPIO   AVR32_PIN_PB13  // STK600.PORTE.PE5 -> STK600.LEDS.LED5
#define LED6_GPIO   AVR32_PIN_PB14  // STK600.PORTE.PE6 -> STK600.LEDS.LED6
#define LED7_GPIO   AVR32_PIN_PB15  // STK600.PORTE.PE7 -> STK600.LEDS.LED7
//! Number of LEDs.
#define LED_COUNT                       8
//@}

/*! \name GPIO Connections of Push Buttons. To use these defines, connect the
 * STK600 PORTF connector to the SWITCHES connector.
 */
//@{
// Connect STK600.PORTF.PF0 to STK600.SWITCHES.SW0
#define GPIO_PUSH_BUTTON_SW0            AVR32_PIN_PB16
#define GPIO_PUSH_BUTTON_SW0_PRESSED    0
// Connect STK600.PORTF.PF1 to STK600.SWITCHES.SW1
#define GPIO_PUSH_BUTTON_SW1            AVR32_PIN_PB17
#define GPIO_PUSH_BUTTON_SW1_PRESSED    0
// Connect STK600.PORTF.PF2 to STK600.SWITCHES.SW2
#define GPIO_PUSH_BUTTON_SW2            AVR32_PIN_PB18
#define GPIO_PUSH_BUTTON_SW2_PRESSED    0
// Connect STK600.PORTF.PF3 to STK600.SWITCHES.SW3
#define GPIO_PUSH_BUTTON_SW3            AVR32_PIN_PB19
#define GPIO_PUSH_BUTTON_SW3_PRESSED    0
// Connect STK600.PORTF.PF4 to STK600.SWITCHES.SW4
#define GPIO_PUSH_BUTTON_SW4            AVR32_PIN_PB20
#define GPIO_PUSH_BUTTON_SW4_PRESSED    0
// Connect STK600.PORTF.PF5 to STK600.SWITCHES.SW5
#define GPIO_PUSH_BUTTON_SW5            AVR32_PIN_PB21
#define GPIO_PUSH_BUTTON_SW5_PRESSED    0
// Connect STK600.PORTF.PF6 to STK600.SWITCHES.SW6
#define GPIO_PUSH_BUTTON_SW6            AVR32_PIN_PB22
#define GPIO_PUSH_BUTTON_SW6_PRESSED    0
// Connect STK600.PORTF.PF7 to STK600.SWITCHES.SW7
#define GPIO_PUSH_BUTTON_SW7            AVR32_PIN_PB23
#define GPIO_PUSH_BUTTON_SW7_PRESSED    0
//@}

//! \name DataFlash memory (AT45DBX)
//@{
#define AT45DBX_SPI                     (&AVR32_SPI)
#define AT45DBX_SPI_NPCS                0
//! SCK pin
#define AT45DBX_SPI_SCK_PIN             AVR32_SPI_SCK_0_0_PIN
#define AT45DBX_SPI_SCK_FUNCTION        AVR32_SPI_SCK_0_0_FUNCTION
//! MOSI pin
#define AT45DBX_SPI_MOSI_PIN            AVR32_SPI_MOSI_0_0_PIN
#define AT45DBX_SPI_MOSI_FUNCTION       AVR32_SPI_MOSI_0_0_FUNCTION
//! MISO pin
 #define AT45DBX_SPI_MISO_PIN           AVR32_SPI_MISO_0_0_PIN
#define AT45DBX_SPI_MISO_FUNCTION       AVR32_SPI_MISO_0_0_FUNCTION
//! CS pin
#define AT45DBX_SPI_NPCS0_PIN           AVR32_SPI_NPCS_0_2_PIN
#define AT45DBX_SPI_NPCS0_FUNCTION      AVR32_SPI_NPCS_0_2_FUNCTION
//@}

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
 * \defgroup stk600_rcuc3l3_config_group Configuration options
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

#endif /* _STK600_RCUC3L3_H_ */
