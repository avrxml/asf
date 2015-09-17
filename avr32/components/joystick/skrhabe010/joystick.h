/*****************************************************************************
 *
 * \file
 *
 * \brief AT32UC3 SKRHABE010 (Joystick) header file.
 *
 * This file contains definitions and services related to the features of the
 * 4-directional with Center Push SKRHABE010 joystick.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
