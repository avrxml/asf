/*****************************************************************************
 *
 * \file
 *
 * \brief AT32UC3A EVK1105 board LEDs support package.
 *
 * This file contains definitions and services related to the LED features of
 * the EVK1105 board.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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


#ifndef _LED_H_
#define _LED_H_

#include "compiler.h"


/*! \name Identifiers of LEDs to Use with LED Functions
 */
//! @{
#define LED0  0x01
#define LED1  0x02
#define LED2  0x04
#define LED3  0x08
//! @}


/*! \brief Gets the last state of all LEDs set through the LED API.
 *
 * \return State of all LEDs (1 bit per LED).
 *
 * \note The GPIO pin configuration of all LEDs is left unchanged.
 */
extern uint32_t LED_Read_Display(void);

/*! \brief Sets the state of all LEDs.
 *
 * \param leds New state of all LEDs (1 bit per LED).
 *
 * \note The pins of all LEDs are set to GPIO output mode.
 */
extern void LED_Display(uint32_t leds);

/*! \brief Gets the last state of the specified LEDs set through the LED API.
 *
 * \param mask LEDs of which to get the state (1 bit per LED).
 *
 * \return State of the specified LEDs (1 bit per LED).
 *
 * \note The GPIO pin configuration of all LEDs is left unchanged.
 */
extern uint32_t LED_Read_Display_Mask(uint32_t mask);

/*! \brief Sets the state of the specified LEDs.
 *
 * \param mask LEDs of which to set the state (1 bit per LED).
 *
 * \param leds New state of the specified LEDs (1 bit per LED).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
extern void LED_Display_Mask(uint32_t mask, uint32_t leds);

/*! \brief Tests the last state of the specified LEDs set through the LED API.
 *
 * \param leds LEDs of which to test the state (1 bit per LED).
 *
 * \return \c true if at least one of the specified LEDs has a state on, else
 *         \c false.
 *
 * \note The GPIO pin configuration of all LEDs is left unchanged.
 */
extern bool LED_Test(uint32_t leds);

/*! \brief Turns off the specified LEDs.
 *
 * \param leds LEDs to turn off (1 bit per LED).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
extern void LED_Off(uint32_t leds);

/*! \brief Turns on the specified LEDs.
 *
 * \param leds LEDs to turn on (1 bit per LED).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
extern void LED_On(uint32_t leds);

/*! \brief Toggles the specified LEDs.
 *
 * \param leds LEDs to toggle (1 bit per LED).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
extern void LED_Toggle(uint32_t leds);

/*! \brief Gets as a bit-field the last state of the specified LEDs set through
 *         the LED API.
 *
 * \param field LEDs of which to get the state (1 bit per LED).
 *
 * \return State of the specified LEDs (1 bit per LED, beginning with the first
 *         specified LED).
 *
 * \note The GPIO pin configuration of all LEDs is left unchanged.
 */
extern uint32_t LED_Read_Display_Field(uint32_t field);

/*! \brief Sets as a bit-field the state of the specified LEDs.
 *
 * \param field LEDs of which to set the state (1 bit per LED).
 * \param leds New state of the specified LEDs (1 bit per LED, beginning with
 *             the first specified LED).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
extern void LED_Display_Field(uint32_t field, uint32_t leds);

/*! \brief Gets the intensity of the specified LED.
 *
 * \param led LED of which to get the intensity (1 bit per LED; only the least
 *            significant set bit is used).
 *
 * \return Intensity of the specified LED (0x00 to 0xFF).
 *
 * \warning The PWM channel of the specified LED is supposed to be used only by
 *          this module.
 *
 * \note The GPIO pin configuration of all LEDs is left unchanged.
 */
extern uint8_t LED_Get_Intensity(uint32_t led);

/*! \brief Sets the intensity of the specified LEDs.
 *
 * \param leds LEDs of which to set the intensity (1 bit per LED).
 * \param intensity New intensity of the specified LEDs (0x00 to 0xFF).
 *
 * \warning The PWM channels of the specified LEDs are supposed to be used only
 *          by this module.
 *
 * \note The pins of the specified LEDs are set to PWM output mode.
 */
extern void LED_Set_Intensity(uint32_t leds, uint8_t intensity);


#endif  // _LED_H_
