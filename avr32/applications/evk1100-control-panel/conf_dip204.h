/*****************************************************************************
 *
 * \file
 *
 * \brief Configuration template file for the AVR UC3 LCD DIP204 driver.
 *
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef __CONF_DIP204_H__
#define __CONF_DIP204_H__

#include "board.h"

/*! \name SPI Connections of the DIP204 LCD
 */
//! @{
#if !defined(DIP204_SPI)           || \
!defined(DIP204_SPI_NPCS)          || \
!defined(DIP204_SPI_SCK_PIN)       || \
!defined(DIP204_SPI_SCK_FUNCTION)  || \
!defined(DIP204_SPI_MISO_PIN)      || \
!defined(DIP204_SPI_MISO_FUNCTION) || \
!defined(DIP204_SPI_MOSI_PIN)      || \
!defined(DIP204_SPI_MOSI_FUNCTION) || \
!defined(DIP204_SPI_NPCS_PIN)      || \
!defined(DIP204_SPI_NPCS_FUNCTION) 
#  if UC3A
#    define DIP204_SPI                  (&AVR32_SPI1)
#    define DIP204_SPI_NPCS             1
#    define DIP204_SPI_SCK_PIN          AVR32_SPI1_SCK_0_0_PIN
#    define DIP204_SPI_SCK_FUNCTION     AVR32_SPI1_SCK_0_0_FUNCTION
#    define DIP204_SPI_MISO_PIN         AVR32_SPI1_MISO_0_0_PIN
#    define DIP204_SPI_MISO_FUNCTION    AVR32_SPI1_MISO_0_0_FUNCTION
#    define DIP204_SPI_MOSI_PIN         AVR32_SPI1_MOSI_0_0_PIN
#    define DIP204_SPI_MOSI_FUNCTION    AVR32_SPI1_MOSI_0_0_FUNCTION
#    define DIP204_SPI_NPCS_PIN         AVR32_SPI1_NPCS_2_0_PIN
#    define DIP204_SPI_NPCS_FUNCTION    AVR32_SPI1_NPCS_2_0_FUNCTION
#  else
#    define DIP204_SPI_NPCS             1
#    define DIP204_SPI                  (&AVR32_SPI)
#    define DIP204_SPI_SCK_PIN          AVR32_SPI_SCK_0_0_PIN
#    define DIP204_SPI_SCK_FUNCTION     AVR32_SPI_SCK_0_0_FUNCTION
#    define DIP204_SPI_MISO_PIN         AVR32_SPI_MISO_0_0_PIN
#    define DIP204_SPI_MISO_FUNCTION    AVR32_SPI_MISO_0_0_FUNCTION
#    define DIP204_SPI_MOSI_PIN         AVR32_SPI_MOSI_0_0_PIN
#    define DIP204_SPI_MOSI_FUNCTION    AVR32_SPI_MOSI_0_0_FUNCTION
#    define DIP204_SPI_NPCS_PIN         AVR32_SPI_NPCS_2_0_PIN
#    define DIP204_SPI_NPCS_FUNCTION    AVR32_SPI_NPCS_2_0_FUNCTION
#  endif
#  warning The DIP204 SPI configuration does not exist in the board definition file. Using default settings.
#endif
//! @}

/*! \name GPIO and PWM Connections of the DIP204 LCD Backlight
 */
//! @{
#if !defined(DIP204_BACKLIGHT_PIN)
#  if UC3A
#    define DIP204_BACKLIGHT_PIN        AVR32_PIN_PB18
#  else
#    define DIP204_BACKLIGHT_PIN        AVR32_PWM_6_1_PIN
#  endif
#  warning The DIP204 Backlight configuration does not exist in the board definition file. Using default settings.
#endif


#if !defined(DIP204_PWM_CHANNEL) || \
!defined(DIP204_PWM_PIN)         || \
!defined(DIP204_PWM_FUNCTION)
#  if UC3A
#    define DIP204_PWM_CHANNEL          6
#    define DIP204_PWM_PIN              AVR32_PWM_6_PIN
#    define DIP204_PWM_FUNCTION         AVR32_PWM_6_FUNCTION
#  else
#    define DIP204_PWM_CHANNEL          6
#    define DIP204_PWM_PIN              AVR32_PWM_6_1_PIN
#    define DIP204_PWM_FUNCTION         AVR32_PWM_6_1_FUNCTION
#  endif
#  warning The DIP204 PWM configuration does not exist in the board definition file. Using default settings.
#endif
//! @}

#endif // __CONF_DIP204_H__
