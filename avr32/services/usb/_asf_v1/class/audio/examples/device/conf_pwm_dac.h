/**
 * \file
 *
 * \brief PWM mono audio DAC driver configuration file for AVR32.
 *
 * This file contains the possible external configuration of the PWM DAC.
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _CONF_PWM_DAC_H_
#define _CONF_PWM_DAC_H_

#include <avr32/io.h>
#include "intc.h"
#include "board.h"


/*! \name PWM Connections
 */
//! @{
#define PWM_DAC_PWM                   (&AVR32_PWM)
#define PWM_DAC_PWM_IRQ               AVR32_PWM_IRQ
#define PWM_DAC_PWM_INT_LEVEL         AVR32_INTC_INT3
#if BOARD==EVK1100 || BOARD==EVK1105
#  define PWM_DAC_PWM_CHANNEL_LSB       1             //!< Least significant 8-bit PWM channel.
#  define PWM_DAC_PWM_CHANNEL_LSB_PIN                 //!< Least significant 8-bit PWM channel pin.
#  define PWM_DAC_PWM_CHANNEL_MSB       0             //!< Most significant 8-bit PWM channel.
#  define PWM_DAC_PWM_CHANNEL_MSB_PIN                 //!< Most significant 8-bit PWM channel pin.
#elif BOARD==EVK1101
#  define PWM_DAC_PWM_CHANNEL_LSB       6             //!< Least significant 8-bit PWM channel.
#  define PWM_DAC_PWM_CHANNEL_LSB_PIN   _0            //!< Least significant 8-bit PWM channel pin.
#  define PWM_DAC_PWM_CHANNEL_MSB       4             //!< Most significant 8-bit PWM channel.
#  define PWM_DAC_PWM_CHANNEL_MSB_PIN   _1            //!< Most significant 8-bit PWM channel pin.
#endif
//! @}


#endif  // _CONF_PWM_DAC_H_
