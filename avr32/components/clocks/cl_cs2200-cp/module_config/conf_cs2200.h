/*****************************************************************************
 *
 * \file
 *
 * \brief Driver for the Cirrus Logic CS2200 clock synthesizer
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


 //! Configuration of the clock synthesizer device driver

#ifndef _CONF_CS2200_H_
#define _CONF_CS2200_H_

#include "board.h"

// Configuration for the hardware module (TWI or TWIM) depending of the UC3 series
#if (UC3A3 || UC3C || UC3L)
#  define CS2200_TWI                  (&AVR32_TWIM0)
#else
#  define CS2200_TWI                  (&AVR32_TWI)
#endif

//! Communication interface
//!  can be :
//!      CS2200_INTERFACE_TWI for TWI interface
//!      CS2200_INTERFACE_SPI for SPI interface (not supported)
#define CS2200_INTERFACE            CS2200_INTERFACE_TWI

//! Reset Pin
#if   CS2200_INTERFACE == CS2200_INTERFACE_TWI
#elif CS2200_INTERFACE == CS2200_INTERFACE_SPI
#  error The SPI interface is not supported.
#else
#  error CS2200_INTERFACE is not correctly defined
#endif

//! TWI slave address.
#define CS2200_TWI_SLAVE_ADDRESS    CS2200_TWI_ADDR

//! TWI speed.
#define CS2200_TWI_MASTER_SPEED     1000 // low clock is required during the startup of the CPU on the internal RC oscillator

//! SPI speed.
#define CS2200_SPI_MASTER_SPEED     50000

//! Number of tries to initialize the CS2200
#define CS2200_NB_TRIES             10

#endif  // _CONF_CS2200_H_
