/*****************************************************************************
 *
 * \file
 *
 * \brief QT1081 helper functions
 *
 * This file contains definitions and services related to the features of the
 * QT1081 touch sensor device.
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


#ifndef _QT1081_H_
#define _QT1081_H_

/**
 * \defgroup group_avr32_components_touch_qt1081 TOUCH - QT1081 QTouch sensor
 *
 * The QT1081 sensor is operated by GPIOs and this driver provides a wrapper for the GPIO accesses.
 *
 * \{
 */

#include "compiler.h"
#include "board.h"
#include "gpio.h"


/*! \brief Puts the QT1081 into low power mode
 */
__always_inline static void qt1081_power_down(void)
{
	gpio_set_gpio_pin(QT1081_LP_MODE);
}

/*! \brief Puts the QT1081 into low power mode
 */
__always_inline static void qt1081_power_up(void)
{
	gpio_clr_gpio_pin(QT1081_LP_MODE);
}
#ifdef QT1081_TOUCH_SENSOR_0
/*! \brief This function tests if touch sensor 0 was touched.
 *
 * \return \c true if the sensor is touched, otherwise \c 0.
 */
__always_inline static int is_touch_sensor_0( void )
{
  return (gpio_get_pin_value(QT1081_TOUCH_SENSOR_0)) == QT1081_TOUCH_SENSOR_0_PRESSED;
}
#endif

#ifdef QT1081_TOUCH_SENSOR_1
/*! \brief This function tests if touch sensor 1 was touched.
 *
 * \return \c true if the sensor is touched, otherwise \c 0.
 */
__always_inline static int is_touch_sensor_1( void )
{
  return (gpio_get_pin_value(QT1081_TOUCH_SENSOR_1)) == QT1081_TOUCH_SENSOR_1_PRESSED;
}
#endif

#ifdef QT1081_TOUCH_SENSOR_2
/*! \brief This function tests if touch sensor 2 was touched.
 *
 * \return \c true if the sensor is touched, otherwise \c 0.
 */
__always_inline static int is_touch_sensor_2( void )
{
  return (gpio_get_pin_value(QT1081_TOUCH_SENSOR_2)) == QT1081_TOUCH_SENSOR_2_PRESSED;
}
#endif

#ifdef QT1081_TOUCH_SENSOR_3
/*! \brief This function tests if touch sensor 3 was touched.
 *
 * \return \c true if the sensor is touched, otherwise \c 0.
 */
__always_inline static int is_touch_sensor_3( void )
{
  return (gpio_get_pin_value(QT1081_TOUCH_SENSOR_3)) == QT1081_TOUCH_SENSOR_3_PRESSED;
}
#endif

#ifdef QT1081_TOUCH_SENSOR_4
/*! \brief This function tests if touch sensor 4 was touched.
 *
 * \return \c true if the sensor is touched, otherwise \c 0.
 */
__always_inline static int is_touch_sensor_4( void )
{
  return (gpio_get_pin_value(QT1081_TOUCH_SENSOR_4)) == QT1081_TOUCH_SENSOR_4_PRESSED;
}
#endif

#ifdef QT1081_TOUCH_SENSOR_5
/*! \brief This function tests if touch sensor 5 was touched.
 *
 * \return \c true if the sensor is touched, otherwise \c 0.
 */
__always_inline static int is_touch_sensor_5( void )
{
  return (gpio_get_pin_value(QT1081_TOUCH_SENSOR_5)) == QT1081_TOUCH_SENSOR_5_PRESSED;
}
#endif

#ifdef QT1081_TOUCH_SENSOR_6
/*! \brief This function tests if touch sensor 6 was touched.
 *
 * \return \c true if the sensor is touched, otherwise \c 0.
 */
__always_inline static int is_touch_sensor_6( void )
{
  return (gpio_get_pin_value(QT1081_TOUCH_SENSOR_6)) == QT1081_TOUCH_SENSOR_6_PRESSED;
}
#endif

#ifdef QT1081_TOUCH_SENSOR_7
/*! \brief This function tests if touch sensor 7 was touched.
 *
 * \return \c true if the sensor is touched, otherwise \c 0.
 */
__always_inline static int is_touch_sensor_7( void )
{
  return (gpio_get_pin_value(QT1081_TOUCH_SENSOR_7)) == QT1081_TOUCH_SENSOR_7_PRESSED;
}
#endif

/**
 * \}
 */

#endif  // _QT1081_H_
