/**
 * \file
 *
 * \brief STK600 with the RCUC3A0 routing card header file.
 *
 * This file contains definitions and services related to the features of the
 * STK600 board with RCUC3A0 routing card. As the STK600 Leds, switches, clocks are configurable
 * through connectors and software through Atmel Studio, the proposed definitions
 * below should be considered as examples and may be modified for a given application.
 *
 * To use this board, define BOARD=STK600_RCUC3A0.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

#ifndef _STK600_RCUC3A0_H_
#define _STK600_RCUC3A0_H_

#include "compiler.h"

#ifdef __AVR32_ABI_COMPILER__ // Automatically defined when compiling for AVR32, not when assembling.
#  include "led.h"
#endif  // __AVR32_ABI_COMPILER__


/*! \name Oscillator Definitions
 */
//! @{


#define FOSC32          32768                                 //!< Osc32 frequency: Hz.
#define OSC32_STARTUP   AVR32_PM_OSCCTRL32_STARTUP_8192_RCOSC //!< Osc32 startup time: RCOsc periods.

// Osc0 crystal is not mounted by default. Set the following definitions to the
// appropriate values if a custom Osc0 crystal is mounted on your STK600

#define FOSC0           12000000                              //!< Osc0 frequency: Hz.
#define OSC0_STARTUP    AVR32_PM_OSCCTRL0_STARTUP_2048_RCOSC  //!< Osc0 startup time: RCOsc periods.

// Osc1 crystal is not mounted by default. Set the following definitions to the
// appropriate values if a custom Osc1 crystal is mounted on your board.
//#define FOSC1           12000000                              //!< Osc1 frequency: Hz.
//#define OSC1_STARTUP    AVR32_PM_OSCCTRL1_STARTUP_2048_RCOSC  //!< Osc1 startup time: RCOsc periods.
// Osc1 crystal is not mounted by default. Set the following definitions to the

//! @}

/* These are documented in services/basic/clock/uc3a0_a1/osc.h */
#define BOARD_OSC0_HZ           12000000
#define BOARD_OSC0_STARTUP_US   17000
#define BOARD_OSC0_IS_XTAL      true
#define BOARD_OSC32_HZ          32768
#define BOARD_OSC32_STARTUP_US  71000
#define BOARD_OSC32_IS_XTAL     true

/*! \name SDRAM Definitions
 */
//! @{

//! Part header file of used SDRAM(s).
#define SDRAM_PART_HDR  "mt48lc16m16a2tg7e/mt48lc16m16a2tg7e.h"

//! Data bus width to use the SDRAM(s) with (16 or 32 bits; always 16 bits on
//! UC3).
#define SDRAM_DBW       16

//! @}
//! Number of LEDs.
#define LED_COUNT   8

/*! \name GPIO Connections of LEDs. To use these defines, connect the STK600 PORTA 
 * PB07-to-PB15 connectors to respectively the LEDs LED0-to-LED3 connectors.
 * Note that when the JTAG is active, it uses pins PA00-to-PA03 of the UC3A. This
 * is why we didn't use pins PA00-to-PA03 for the LEDs LED0-to-LED3.
 */
//! @{
#define LED0_GPIO   AVR32_PIN_PB08  // STK600.PORTF.PF0 -> STK600.LEDS.LED0
#define LED1_GPIO   AVR32_PIN_PB09  // STK600.PORTF.PF1 -> STK600.LEDS.LED1
#define LED2_GPIO   AVR32_PIN_PB10  // STK600.PORTF.PF2 -> STK600.LEDS.LED2
#define LED3_GPIO   AVR32_PIN_PB11  // STK600.PORTF.PF3 -> STK600.LEDS.LED3
#define LED4_GPIO   AVR32_PIN_PB12  // STK600.PORTF.PF4 -> STK600.LEDS.LED4
#define LED5_GPIO   AVR32_PIN_PB13  // STK600.PORTF.PF5 -> STK600.LEDS.LED5
#define LED6_GPIO   AVR32_PIN_PB14  // STK600.PORTF.PF6 -> STK600.LEDS.LED6
#define LED7_GPIO   AVR32_PIN_PB15  // STK600.PORTF.PF7 -> STK600.LEDS.LED7
//! @}

/*! \name GPIO Connections of Push Buttons. To use these defines, connect the
 * STK600 PORTH connector to the SWITCHES connector.
 */
//! @{
#define GPIO_PUSH_BUTTON_SW0            AVR32_PIN_PB24  // Connect STK600.PORTH.PH0 to STK600.SWITCHES.SW0
#define GPIO_PUSH_BUTTON_SW0_PRESSED    0
#define GPIO_PUSH_BUTTON_SW1            AVR32_PIN_PB25  // Connect STK600.PORTH.PH1 to STK600.SWITCHES.SW1
#define GPIO_PUSH_BUTTON_SW1_PRESSED    0
#define GPIO_PUSH_BUTTON_SW2            AVR32_PIN_PB26  // Connect STK600.PORTH.PH2 to STK600.SWITCHES.SW2
#define GPIO_PUSH_BUTTON_SW2_PRESSED    0
#define GPIO_PUSH_BUTTON_SW3            AVR32_PIN_PB27  // Connect STK600.PORTH.PH3 to STK600.SWITCHES.SW3
#define GPIO_PUSH_BUTTON_SW3_PRESSED    0
#define GPIO_PUSH_BUTTON_SW4            AVR32_PIN_PB28  // Connect STK600.PORTH.PH4 to STK600.SWITCHES.SW4
#define GPIO_PUSH_BUTTON_SW4_PRESSED    0
#define GPIO_PUSH_BUTTON_SW5            AVR32_PIN_PB29  // Connect STK600.PORTH.PH5 to STK600.SWITCHES.SW5
#define GPIO_PUSH_BUTTON_SW5_PRESSED    0
#define GPIO_PUSH_BUTTON_SW6            AVR32_PIN_PB30  // Connect STK600.PORTH.PH6 to STK600.SWITCHES.SW6
#define GPIO_PUSH_BUTTON_SW6_PRESSED    0
#define GPIO_PUSH_BUTTON_SW7            AVR32_PIN_PB31  // Connect STK600.PORTH.PH7 to STK600.SWITCHES.SW7
#define GPIO_PUSH_BUTTON_SW7_PRESSED    0
//! @}

/*! \name SPI Connections of the DIP204 LCD
 */
//! @{
#define DIP204_SPI                  (&AVR32_SPI1)
#define DIP204_SPI_NPCS             2
#define DIP204_SPI_SCK_PIN          AVR32_SPI1_SCK_0_0_PIN
#define DIP204_SPI_SCK_FUNCTION     AVR32_SPI1_SCK_0_0_FUNCTION
#define DIP204_SPI_MISO_PIN         AVR32_SPI1_MISO_0_0_PIN
#define DIP204_SPI_MISO_FUNCTION    AVR32_SPI1_MISO_0_0_FUNCTION
#define DIP204_SPI_MOSI_PIN         AVR32_SPI1_MOSI_0_0_PIN
#define DIP204_SPI_MOSI_FUNCTION    AVR32_SPI1_MOSI_0_0_FUNCTION
#define DIP204_SPI_NPCS_PIN         AVR32_SPI1_NPCS_2_0_PIN
#define DIP204_SPI_NPCS_FUNCTION    AVR32_SPI1_NPCS_2_0_FUNCTION
//! @}

/*! \name GPIO and PWM Connections of the DIP204 LCD Backlight
 */
//! @{
#define DIP204_BACKLIGHT_PIN        AVR32_PIN_PB18
#define DIP204_PWM_CHANNEL          6
#define DIP204_PWM_PIN              AVR32_PWM_6_PIN
#define DIP204_PWM_FUNCTION         AVR32_PWM_6_FUNCTION
//! @}

/*! \name SPI Connections of the AT45DBX Data Flash Memory
 */
//! @{
#define AT45DBX_SPI                 (&AVR32_SPI1)
#define AT45DBX_SPI_NPCS            0
#define AT45DBX_SPI_SCK_PIN         AVR32_SPI1_SCK_0_0_PIN
#define AT45DBX_SPI_SCK_FUNCTION    AVR32_SPI1_SCK_0_0_FUNCTION
#define AT45DBX_SPI_MISO_PIN        AVR32_SPI1_MISO_0_0_PIN
#define AT45DBX_SPI_MISO_FUNCTION   AVR32_SPI1_MISO_0_0_FUNCTION
#define AT45DBX_SPI_MOSI_PIN        AVR32_SPI1_MOSI_0_0_PIN
#define AT45DBX_SPI_MOSI_FUNCTION   AVR32_SPI1_MOSI_0_0_FUNCTION
#define AT45DBX_SPI_NPCS0_PIN       AVR32_SPI1_NPCS_0_0_PIN
#define AT45DBX_SPI_NPCS0_FUNCTION  AVR32_SPI1_NPCS_0_0_FUNCTION
//! @}

/*! \name MACB connections to the DP83848 external phy controller
 */
//! @{

//! GPIO connection of the MAC PHY PWR_DOWN/INT signal for the external phy controller
#define MACB_INTERRUPT_PIN          AVR32_PIN_PA24
#define EXTPHY_MACB_INTERRUPT_PIN   MACB_INTERRUPT_PIN // Added for homogeneity

#define EXTPHY_MACB                 (&AVR32_MACB)
#define EXTPHY_MACB_MDC_PIN         AVR32_MACB_MDC_0_PIN
#define EXTPHY_MACB_MDC_FUNCTION    AVR32_MACB_MDC_0_FUNCTION
#define EXTPHY_MACB_MDIO_PIN        AVR32_MACB_MDIO_0_PIN
#define EXTPHY_MACB_MDIO_FUNCTION   AVR32_MACB_MDIO_0_FUNCTION
#define EXTPHY_MACB_RXD_0_PIN       AVR32_MACB_RXD_0_PIN
#define EXTPHY_MACB_RXD_0_FUNCTION  AVR32_MACB_RXD_0_FUNCTION
#define EXTPHY_MACB_RXD_1_PIN       AVR32_MACB_RXD_1_PIN
#define EXTPHY_MACB_RXD_1_FUNCTION  AVR32_MACB_RXD_1_FUNCTION
#define EXTPHY_MACB_TXD_0_PIN       AVR32_MACB_TXD_0_PIN
#define EXTPHY_MACB_TXD_0_FUNCTION  AVR32_MACB_TXD_0_FUNCTION
#define EXTPHY_MACB_TXD_1_PIN       AVR32_MACB_TXD_1_PIN
#define EXTPHY_MACB_TXD_1_FUNCTION  AVR32_MACB_TXD_1_FUNCTION
#define EXTPHY_MACB_TX_EN_PIN       AVR32_MACB_TX_EN_0_PIN
#define EXTPHY_MACB_TX_EN_FUNCTION  AVR32_MACB_TX_EN_0_FUNCTION
#define EXTPHY_MACB_RX_ER_PIN       AVR32_MACB_RX_ER_0_PIN
#define EXTPHY_MACB_RX_ER_FUNCTION  AVR32_MACB_RX_ER_0_FUNCTION
#define EXTPHY_MACB_RX_DV_PIN       AVR32_MACB_RX_DV_0_PIN
#define EXTPHY_MACB_RX_DV_FUNCTION  AVR32_MACB_RX_DV_0_FUNCTION
#define EXTPHY_MACB_TX_CLK_PIN      AVR32_MACB_TX_CLK_0_PIN
#define EXTPHY_MACB_TX_CLK_FUNCTION AVR32_MACB_TX_CLK_0_FUNCTION

//! Phy Address (set through strap options)
#define EXTPHY_PHY_ADDR             0x01

//! Configure the ethernet phy component to use the DP83848 phy
#define PHY_DP83848

//! @}
#endif  // _STK600_RCUCA0_H_
