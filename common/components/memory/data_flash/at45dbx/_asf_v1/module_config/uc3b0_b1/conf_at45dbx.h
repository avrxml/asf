/*****************************************************************************
 *
 * \file
 *
 * \brief AT45DBX template configuration file.
 *
 * This file contains the possible external configuration of the AT45DBX.
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
#ifndef _CONF_AT45DBX_H_
#define _CONF_AT45DBX_H_

#include "conf_access.h"
#include "at45dbx.h"

#ifdef __cplusplus
extern "C" {
#endif


#if !defined(AT45DBX_SPI)                || \
    !defined(AT45DBX_SPI_NPCS)           || \
    !defined(AT45DBX_SPI_SCK_PIN)        || \
    !defined(AT45DBX_SPI_SCK_FUNCTION)   || \
    !defined(AT45DBX_SPI_MISO_PIN)       || \
    !defined(AT45DBX_SPI_MISO_FUNCTION)  || \
    !defined(AT45DBX_SPI_MOSI_PIN)       || \
    !defined(AT45DBX_SPI_MOSI_FUNCTION)  || \
    !defined(AT45DBX_SPI_NPCS0_PIN)      || \
    !defined(AT45DBX_SPI_NPCS0_FUNCTION) 

#  warning The AT45DBX configuration does not exist in the board definition file. Using default settings.
/*! \name SPI Connections of the AT45DBX Data Flash Memory
 */
//! @{
#  define AT45DBX_SPI                 (&AVR32_SPI)
#  define AT45DBX_SPI_NPCS            0
#  define AT45DBX_SPI_SCK_PIN         AVR32_SPI_SCK_0_0_PIN
#  define AT45DBX_SPI_SCK_FUNCTION    AVR32_SPI_SCK_0_0_FUNCTION
#  define AT45DBX_SPI_MISO_PIN        AVR32_SPI_MISO_0_0_PIN
#  define AT45DBX_SPI_MISO_FUNCTION   AVR32_SPI_MISO_0_0_FUNCTION
#  define AT45DBX_SPI_MOSI_PIN        AVR32_SPI_MOSI_0_0_PIN
#  define AT45DBX_SPI_MOSI_FUNCTION   AVR32_SPI_MOSI_0_0_FUNCTION
#  define AT45DBX_SPI_NPCS0_PIN       AVR32_SPI_NPCS_0_0_PIN
#  define AT45DBX_SPI_NPCS0_FUNCTION  AVR32_SPI_NPCS_0_0_FUNCTION
//! @}
#endif


#if AT45DBX_MEM == DISABLE
#  error conf_at45dbx.h is #included although AT45DBX_MEM is disabled
#endif

//_____ D E F I N I T I O N S ______________________________________________

//! Size of AT45DBX data flash memories to manage.
#define AT45DBX_MEM_SIZE            AT45DBX_8MB

//! Number of AT45DBX components to manage.
#define AT45DBX_MEM_CNT             1

//! First chip select used by AT45DBX components on the SPI module instance.
//! AT45DBX_SPI_NPCS0_PIN always corresponds to this first NPCS, whatever it is.
#define AT45DBX_SPI_FIRST_NPCS      AT45DBX_SPI_NPCS

//! SPI master speed in Hz.
#define AT45DBX_SPI_MASTER_SPEED    12000000

//! Number of bits in each SPI transfer.
#define AT45DBX_SPI_BITS            8

#ifdef __cplusplus
}
#endif

#endif  // _CONF_AT45DBX_H_
