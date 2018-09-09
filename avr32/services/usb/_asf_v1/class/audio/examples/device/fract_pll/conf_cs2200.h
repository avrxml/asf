/*****************************************************************************
 *
 * \file
 *
 * \brief Configuration file for the driver of the CS2200 chip.
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


#ifndef _CONF_CS2200_H_
#define _CONF_CS2200_H_

#include "board.h"


#define CS2200_TWI                  (&AVR32_TWI)

#define CS2200_TWI_SCL_PIN           AVR32_TWI_SCL_0_0_PIN
#define CS2200_TWI_SCL_FUNCTION      AVR32_TWI_SCL_0_0_FUNCTION
#define CS2200_TWI_SDA_PIN           AVR32_TWI_SDA_0_0_PIN
#define CS2200_TWI_SDA_FUNCTION      AVR32_TWI_SDA_0_0_FUNCTION

//! Communication interface
//!  can be :
//!      CS2200_INTERFACE_TWI for TWI interface
//!      CS2200_INTERFACE_SPI for SPI interface (not supported)
#define CS2200_INTERFACE            CS2200_INTERFACE_TWI

//! Reset Pin
#if (defined CS2200_INTERFACE) && (defined CS2200_INTERFACE_TWI) && (defined CS2200_INTERFACE_SPI)
# if   CS2200_INTERFACE == CS2200_INTERFACE_TWI
# elif CS2200_INTERFACE == CS2200_INTERFACE_SPI
#   error The SPI interface is not supported.
# else
#   error CS2200_INTERFACE is not correctly defined
# endif
#endif

//! TWI slave address.
#define CS2200_TWI_SLAVE_ADDRESS    CS2200_TWI_ADDR

//! TWI speed.
#define CS2200_TWI_MASTER_SPEED     1000 // low clock is required during the startup of the CPU on the internal RC oscillator

//! SPI speed.
#define CS2200_SPI_MASTER_SPEED     50000

//! Number of tries to initialize the CS2200 authentication chip.
#define CS2200_NB_TRIES             10


#endif  // _CONF_CS2200_H_
