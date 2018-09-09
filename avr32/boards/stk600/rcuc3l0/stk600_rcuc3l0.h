/*****************************************************************************
 *
 * \file
 *
 * \brief STK600 with the RCUC3L routing card board header file.
 *
 * This file contains definitions and services related to the features of the
 * STK600 board. As the STK600 Leds, switches, clocks are configurable
 * through connectors and software through Atmel Studio, the proposed definitions
 * below should be considered as examples and may be modified for a given application.
 *
 * To use this board, define BOARD=STK600_UC3L.
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _STK600_RCUC3L0_H_
#define _STK600_RCUC3L0_H_

#include "compiler.h"

#ifdef __AVR32_ABI_COMPILER__ // Automatically defined when compiling for AVR32, not when assembling.
#  include "led.h"
#endif  // __AVR32_ABI_COMPILER__


/*! \name Oscillator Definitions
 */
//! @{

#define FOSC32          32768                                   //!< Osc32 frequency: Hz.
#define OSC32_STARTUP   AVR32_SCIF_OSCCTRL32_STARTUP_8192_RCOSC //!< Osc32 startup time: RCOsc periods.

// Osc0 crystal is not mounted by default. Set the following definitions to the
// appropriate values if a custom Osc0 crystal is mounted on your STK600
#define FOSC0           12000000                                //!< Osc0 frequency: Hz.
#define OSC0_STARTUP    AVR32_SCIF_OSCCTRL0_STARTUP_2048_RCOSC  //!< Osc0 startup time: RCOsc periods.

// Osc1 crystal is not mounted by default. Set the following definitions to the
// appropriate values if a custom Osc1 crystal is mounted on your board.
// #define FOSC1           12000000                              //!< Osc1 frequency: Hz.
// #define OSC1_STARTUP    AVR32_SCIF_OSCCTRL1_STARTUP_2048_RCOSC  //!< Osc1 startup time: RCOsc periods.

//! @}

//! Number of LEDs.
#define LED_COUNT   4

/*! \name GPIO Connections of LEDs. To use these defines, connect the STK600 PORTA
 * PA4-to-PA7 connectors to respectively the LEDs LED0-to-LED3 connectors.
 * Note that when the JTAG is active, it uses pins PA00-to-PA03 of the UC3L. This
 * is why we didn't use pins PA00-to-PA03 for the LEDs LED0-to-LED3.
 */
//! @{
#define LED0_GPIO   AVR32_PIN_PA04  // STK600.PORTA.PA4 -> STK600.LEDS.LED0
#define LED1_GPIO   AVR32_PIN_PA05  // STK600.PORTA.PA5 -> STK600.LEDS.LED1
#define LED2_GPIO   AVR32_PIN_PA06  // STK600.PORTA.PA6 -> STK600.LEDS.LED2
#define LED3_GPIO   AVR32_PIN_PA07  // STK600.PORTA.PA7 -> STK600.LEDS.LED3
#define LED4_GPIO   AVR32_PIN_PA10  // STK600.PORTA.PB2 -> STK600.LEDS.LED4
#define LED5_GPIO   AVR32_PIN_PA11  // STK600.PORTA.PB3 -> STK600.LEDS.LED5
#define LED6_GPIO   AVR32_PIN_PA12  // STK600.PORTA.PB4 -> STK600.LEDS.LED6
#define LED7_GPIO   AVR32_PIN_PA13  // STK600.PORTA.PB5 -> STK600.LEDS.LED7
//! @}

/*! \name GPIO Connections of Push Buttons. To use these defines, connect the
 * STK600 PORTD connector to the SWITCHES connector.
 */
//! @{
#define GPIO_PUSH_BUTTON_SW0            AVR32_PIN_PB00  // Connect STK600.PORTD.PD0 to STK600.SWITCHES.SW0
#define GPIO_PUSH_BUTTON_SW0_PRESSED    0
#define GPIO_PUSH_BUTTON_SW1            AVR32_PIN_PB01  // Connect STK600.PORTD.PD1 to STK600.SWITCHES.SW1
#define GPIO_PUSH_BUTTON_SW1_PRESSED    0
#define GPIO_PUSH_BUTTON_SW2            AVR32_PIN_PB02  // Connect STK600.PORTD.PD2 to STK600.SWITCHES.SW2
#define GPIO_PUSH_BUTTON_SW2_PRESSED    0
#define GPIO_PUSH_BUTTON_SW3            AVR32_PIN_PB03  // Connect STK600.PORTD.PD3 to STK600.SWITCHES.SW3
#define GPIO_PUSH_BUTTON_SW3_PRESSED    0
#define GPIO_PUSH_BUTTON_SW4            AVR32_PIN_PB04  // Connect STK600.PORTD.PD4 to STK600.SWITCHES.SW4
#define GPIO_PUSH_BUTTON_SW4_PRESSED    0
#define GPIO_PUSH_BUTTON_SW5            AVR32_PIN_PB05  // Connect STK600.PORTD.PD5 to STK600.SWITCHES.SW5
#define GPIO_PUSH_BUTTON_SW5_PRESSED    0
#define GPIO_PUSH_BUTTON_SW6            AVR32_PIN_PB06  // Connect STK600.PORTD.PD6 to STK600.SWITCHES.SW6
#define GPIO_PUSH_BUTTON_SW6_PRESSED    0
#define GPIO_PUSH_BUTTON_SW7            AVR32_PIN_PB07  // Connect STK600.PORTD.PD7 to STK600.SWITCHES.SW7
#define GPIO_PUSH_BUTTON_SW7_PRESSED    0
//! @}

//! \name SPI Connections of the AT45DBX Data Flash Memory
// @{
#define AT45DBX_SPI                 (&AVR32_SPI)
#define AT45DBX_SPI_NPCS            0
#define AT45DBX_SPI_SCK_PIN         AVR32_SPI_SCK_0_0_PIN
#define AT45DBX_SPI_SCK_FUNCTION    AVR32_SPI_SCK_0_0_FUNCTION
#define AT45DBX_SPI_MISO_PIN        AVR32_SPI_MISO_0_0_PIN
#define AT45DBX_SPI_MISO_FUNCTION   AVR32_SPI_MISO_0_0_FUNCTION
#define AT45DBX_SPI_MOSI_PIN        AVR32_SPI_MOSI_0_0_PIN
#define AT45DBX_SPI_MOSI_FUNCTION   AVR32_SPI_MOSI_0_0_FUNCTION
#define AT45DBX_SPI_NPCS0_PIN       AVR32_SPI_NPCS_0_0_PIN
#define AT45DBX_SPI_NPCS0_FUNCTION  AVR32_SPI_NPCS_0_0_FUNCTION
//! @}

#endif  // _STK600_RCUC3L0_H_
