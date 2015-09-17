/**
 * \file
 *
 * \brief STK600 with the RCUC3B0 routing card header file.
 *
 * This file contains definitions and services related to the features of the
 * STK600 board with RCUC3B0 routing card. As the STK600 Leds, switches, clocks are configurable
 * through connectors and software through Atmel Studio, the proposed definitions
 * below should be considered as examples and may be modified for a given application.
 *
 * To use this board, define BOARD=STK600_UC3B0.
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

#ifndef _STK600_RCUC3B0_H_
#define _STK600_RCUC3B0_H_

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


//! @}

/* These are documented in services/basic/clock/uc3b0_b1/osc.h */
#define BOARD_OSC0_HZ           12000000
#define BOARD_OSC0_STARTUP_US   17000
#define BOARD_OSC0_IS_XTAL      true
#define BOARD_OSC32_HZ          32768
#define BOARD_OSC32_STARTUP_US  71000
#define BOARD_OSC32_IS_XTAL     true

//! Number of LEDs.
#define LED_COUNT   4

/*! \name GPIO Connections of LEDs. To use these defines, connect the STK600 PORTA 
 * PB08-to-PB15 connectors to respectively the LEDs LED0-to-LED3 connectors.
 * Note that when the JTAG is active, it uses pins PA00-to-PA03 of the UC3A. This
 * is why we didn't use pins PA00-to-PA03 for the LEDs LED0-to-LED3.
 */
//! @{
#define LED0_GPIO   AVR32_PIN_PB08  // STK600.PORTF.PF0 -> STK600.LEDS.LED0
#define LED1_GPIO   AVR32_PIN_PB09  // STK600.PORTF.PF1 -> STK600.LEDS.LED1
#define LED2_GPIO   AVR32_PIN_PB10  // STK600.PORTF.PF2 -> STK600.LEDS.LED2
#define LED3_GPIO   AVR32_PIN_PB11  // STK600.PORTF.PF3 -> STK600.LEDS.LED3
//! @}

/*! \name SPI Connections of the AT45DBX Data Flash Memory
 */
//! @{
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

#endif  // _STK600_RCUC3B0_H_
