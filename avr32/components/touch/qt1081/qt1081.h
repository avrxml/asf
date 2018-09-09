/*****************************************************************************
 *
 * \file
 *
 * \brief QT1081 helper functions
 *
 * This file contains definitions and services related to the features of the
 * QT1081 touch sensor device.
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
