/* This source file is part of the ATMEL QTouch Library Release 5.1 */

/**
 * \file
 *
 * \brief QTouch debug settings.
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
 */

#ifndef QDEBUG_SETTINGS_H
#define QDEBUG_SETTINGS_H

#ifdef __cplusplus
extern "C"
{
#endif

//---------- Do not edit --------------------

/*! \name Project Constants.
 * \brief Values from 0xF000->0xFFFF are reserved for Atmel Kits.
 * Values from 0x0000->0xEFFF are available for other projects.
 */
//! @{

#define   QT8           0xF001
#define   QT16          0xF002
#define   QM64          0xF003
#define   UC3L_EK_REV2  0xF005
#define   SAMD20_XPLAINED_PRO_SELFCAP_EXT    0xF010
#define   SAMD20_XPLAINED_PRO_MUTLCAP_EXT    0xF011

//! @}

/*! \name Interface constants.
 */
//! @{

#define   TWI         1
#define   SPI1W       2
#define   SPI2W       3
#define   UART        4

//! @}

//---------- Edit Project Info -------------

/*! \name Select the type of interface to use for the debug protocol.
 * \brief Comment out the interface not used.
 * Only one interface should be active.
 */
//! @{

//#define QDEBUG_SPI
//#define QDEBUG_TWI
//#define QDEBUG_SERIAL
#define QDEBUG_BITBANG_SPI
#define QDEBUG_SPI_BB

// The definitions below should not clash with the SNS/SNSK port pins
#define QDEBUG_SPI_BB_SS_PIN        31
#define QDEBUG_SPI_BB_SCK_PIN       23
#define QDEBUG_SPI_BB_MOSI_PIN      22
#define QDEBUG_SPI_BB_MISO_PIN      16

#define QDEBUG_SPI_BB_SS_PORT       B
#define QDEBUG_SPI_BB_SCK_PORT      B
#define QDEBUG_SPI_BB_MOSI_PORT     B
#define QDEBUG_SPI_BB_MISO_PORT     B

//! @}

/*! \name Set up project info.
 */
//! @{

#define  PROJECT_ID             SAMD20_XPLAINED_PRO_SELFCAP_EXT
#define  INTERFACE              SPI2W

//! @}

#ifdef __cplusplus
}
#endif

#endif  /* QDEBUG_SETTINGS_H */

