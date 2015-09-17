/*****************************************************************************
 *
 * \file
 *
 * \brief AT32UC3B _USER_BOARD_H_ board header file.
 *
 * This file contains definitions and services related to the features of the
 * EVK1101 board rev. A and B.
 *
 * To use this board, define BOARD=USER_BOARD.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _USER_BOARD_H_
#define _USER_BOARD_H_

#include "compiler.h"

#ifdef __AVR32_ABI_COMPILER__ // Automatically defined when compiling for AVR32, not when assembling.
#  include "led.h"
#endif  // __AVR32_ABI_COMPILER__


/*! \name Oscillator Definitions
 */
//! @{

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

/*! \name USB Definitions
 */
//! @{

//! Multiplexed pin used for USB_ID: AVR32_USBB_USB_ID_x_x.
//! To be selected according to the AVR32_USBB_USB_ID_x_x_PIN and
//! AVR32_USBB_USB_ID_x_x_FUNCTION definitions from <avr32/uc3bxxxx.h>.
#define USB_ID                      AVR32_USBB_USB_ID_0_0

//! Multiplexed pin used for USB_VBOF: AVR32_USBB_USB_VBOF_x_x.
//! To be selected according to the AVR32_USBB_USB_VBOF_x_x_PIN and
//! AVR32_USBB_USB_VBOF_x_x_FUNCTION definitions from <avr32/uc3bxxxx.h>.
#define USB_VBOF                    AVR32_USBB_USB_VBOF_0_0

//! Active level of the USB_VBOF output pin.
#define USB_VBOF_ACTIVE_LEVEL       LOW

//! USB overcurrent detection pin.
#define USB_OVERCURRENT_DETECT_PIN  AVR32_PIN_PA20

//! @}


//! Number of LEDs.
#define LED_COUNT   4

/*! \name GPIO Connections of LEDs
 */
//! @{
#define LED0_GPIO   AVR32_PIN_PA07
#define LED1_GPIO   AVR32_PIN_PA08
#define LED2_GPIO   AVR32_PIN_PA21
#define LED3_GPIO   AVR32_PIN_PA22
//! @}

/*! \name PWM Channels of LEDs
 */
//! @{
#define LED0_PWM    0
#define LED1_PWM    1
#define LED2_PWM    2
#define LED3_PWM    6
//! @}

/*! \name PWM Functions of LEDs
 */
//! @{
#define LED0_PWM_FUNCTION   AVR32_PWM_0_0_FUNCTION
#define LED1_PWM_FUNCTION   AVR32_PWM_1_0_FUNCTION
#define LED2_PWM_FUNCTION   AVR32_PWM_2_0_FUNCTION
#define LED3_PWM_FUNCTION   AVR32_PWM_6_0_FUNCTION
//! @}

/*! \name Color Identifiers of LEDs to Use with LED Functions
 */
//! @{
#define LED_MONO0_GREEN   LED0
#define LED_MONO1_GREEN   LED1
#define LED_MONO2_GREEN   LED2
#define LED_MONO3_GREEN   LED3
//! @}


/*! \name GPIO Connections of Push Buttons
 */
//! @{
#define GPIO_PUSH_BUTTON_0            AVR32_PIN_PB02
#define GPIO_PUSH_BUTTON_0_PRESSED    0
#define GPIO_PUSH_BUTTON_1            AVR32_PIN_PB03
#define GPIO_PUSH_BUTTON_1_PRESSED    0
//! @}


/*! \name GPIO Connections of the Joystick
 */
//! @{
#define GPIO_JOYSTICK_PUSH            AVR32_PIN_PA13
#define GPIO_JOYSTICK_PUSH_PRESSED    0
#define GPIO_JOYSTICK_LEFT            AVR32_PIN_PB06
#define GPIO_JOYSTICK_LEFT_PRESSED    0
#define GPIO_JOYSTICK_RIGHT           AVR32_PIN_PB09
#define GPIO_JOYSTICK_RIGHT_PRESSED   0
#define GPIO_JOYSTICK_UP              AVR32_PIN_PB07
#define GPIO_JOYSTICK_UP_PRESSED      0
#define GPIO_JOYSTICK_DOWN            AVR32_PIN_PB08
#define GPIO_JOYSTICK_DOWN_PRESSED    0
//! @}


#endif  // _USER_BOARD_H_
