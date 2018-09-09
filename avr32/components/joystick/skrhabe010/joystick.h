/*****************************************************************************
 *
 * \file
 *
 * \brief AT32UC3 SKRHABE010 (Joystick) header file.
 *
 * This file contains definitions and services related to the features of the
 * 4-directional with Center Push SKRHABE010 joystick.
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


#ifndef _SKRHABE010_H_
#define _SKRHABE010_H_

/**
 * \defgroup group_avr32_components_joystick_skrhabe010 Joystick interface (5-way)
 *
 * A 5-way joystick (left, right, up, down, press) interface using 5 GPIOs.
 *
 * \{
 */

#include "compiler.h"
#include "board.h"
#include "gpio.h"


/*! \brief This function test it the joystick is on the right.
 *
 * \return \c true if the joystick is on the right, otherwise \c 0.
 */
__always_inline static int is_joystick_right( void )
{
  return (gpio_get_pin_value(GPIO_JOYSTICK_RIGHT)) == GPIO_JOYSTICK_RIGHT_PRESSED;
}


/*! \brief This function test it the joystick is on the left.
 *
 * \return \c true if the joystick is on the left, otherwise \c 0.
 */
__always_inline static int is_joystick_left( void )
{
  return (gpio_get_pin_value(GPIO_JOYSTICK_LEFT)) == GPIO_JOYSTICK_LEFT_PRESSED;
}


/*! \brief This function test it the joystick is up.
 *
 * \return \c true if the joystick is up, otherwise \c 0.
 */
__always_inline static int is_joystick_up( void )
{
  return (gpio_get_pin_value(GPIO_JOYSTICK_UP)) == GPIO_JOYSTICK_UP_PRESSED;
}


/*! \brief This function test it the joystick is down.
 *
 * \return \c true if the joystick is down, otherwise \c 0.
 */
__always_inline static int is_joystick_down( void )
{
  return (gpio_get_pin_value(GPIO_JOYSTICK_DOWN)) == GPIO_JOYSTICK_DOWN_PRESSED;
}


/*! \brief This function test it the joystick is pressed.
 *
 * \return \c true if the joystick is pressed, otherwise \c 0.
 */
__always_inline static int is_joystick_pressed( void )
{
  return (gpio_get_pin_value(GPIO_JOYSTICK_PUSH)) == GPIO_JOYSTICK_PUSH_PRESSED;
}

/**
 * \}
 */

#endif  // _SKRHABE010_H_
