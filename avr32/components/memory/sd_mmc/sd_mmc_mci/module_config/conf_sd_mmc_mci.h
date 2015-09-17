/*****************************************************************************
 *
 * \file
 *
 * \brief SD/MMC configuration file.
 *
 * This file contains the possible external configuration of the SD/MMC.
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


#ifndef _CONF_SD_MMC_MCI_H_
#define _CONF_SD_MMC_MCI_H_


#include "conf_access.h"
#include "sd_mmc_mci.h"

#define SD_4_BIT                  ENABLE    //!> Enable the SD 4-bit or MMC 8-bit interface

#define MCI_DEFAULT_SPEED_HZ      200000    //!> Default card speed in Hz

#define MCI_DEFAULT_PWSDIV          0x07    //!< Power Saving Divider.

/*! \name MCI Connections of the SD/MMC Slots
 */
//! @{
#if !defined(SD_SLOT_MCI)
# define SD_SLOT_MCI                       (&AVR32_MCI)
# warning The SD/MMC Slots register configuration does not exist in the board definition file. Using default settings.
#endif

//! 4-bits connector pin
#if !defined(SD_SLOT_4BITS)                 || \
!defined(SD_SLOT_4BITS_CLK_PIN)             || \
!defined(SD_SLOT_4BITS_CLK_FUNCTION)        || \
!defined(SD_SLOT_4BITS_CMD_PIN)             || \
!defined(SD_SLOT_4BITS_CMD_FUNCTION)        || \
!defined(SD_SLOT_4BITS_DATA0_PIN)           || \
!defined(SD_SLOT_4BITS_DATA0_FUNCTION)      || \
!defined(SD_SLOT_4BITS_DATA1_PIN)           || \
!defined(SD_SLOT_4BITS_DATA1_FUNCTION)      || \
!defined(SD_SLOT_4BITS_DATA2_PIN)           || \
!defined(SD_SLOT_4BITS_DATA2_FUNCTION)      || \
!defined(SD_SLOT_4BITS_DATA3_PIN)           || \
!defined(SD_SLOT_4BITS_DATA3_FUNCTION)      || \
!defined(SD_SLOT_4BITS_CARD_DETECT)         || \
!defined(SD_SLOT_4BITS_CARD_DETECT_VALUE)   || \
!defined(SD_SLOT_4BITS_WRITE_PROTECT)       || \
!defined(SD_SLOT_4BITS_WRITE_PROTECT_VALUE) 
# define SD_SLOT_4BITS                      1
# define SD_SLOT_4BITS_CLK_PIN              AVR32_MCI_CLK_0_PIN
# define SD_SLOT_4BITS_CLK_FUNCTION         AVR32_MCI_CLK_0_FUNCTION
# define SD_SLOT_4BITS_CMD_PIN              AVR32_MCI_CMD_1_0_PIN
# define SD_SLOT_4BITS_CMD_FUNCTION         AVR32_MCI_CMD_1_0_FUNCTION
# define SD_SLOT_4BITS_DATA0_PIN            AVR32_MCI_DATA_8_0_PIN
# define SD_SLOT_4BITS_DATA0_FUNCTION       AVR32_MCI_DATA_8_0_FUNCTION
# define SD_SLOT_4BITS_DATA1_PIN            AVR32_MCI_DATA_9_0_PIN
# define SD_SLOT_4BITS_DATA1_FUNCTION       AVR32_MCI_DATA_9_0_FUNCTION
# define SD_SLOT_4BITS_DATA2_PIN            AVR32_MCI_DATA_10_0_PIN
# define SD_SLOT_4BITS_DATA2_FUNCTION       AVR32_MCI_DATA_10_0_FUNCTION
# define SD_SLOT_4BITS_DATA3_PIN            AVR32_MCI_DATA_11_0_PIN
# define SD_SLOT_4BITS_DATA3_FUNCTION       AVR32_MCI_DATA_11_0_FUNCTION
# define SD_SLOT_4BITS_CARD_DETECT          AVR32_PIN_PB08
# define SD_SLOT_4BITS_CARD_DETECT_VALUE    0
# define SD_SLOT_4BITS_WRITE_PROTECT        AVR32_PIN_PB06
# define SD_SLOT_4BITS_WRITE_PROTECT_VALUE  1
# warning The SD/MMC 4bits Slots register configuration does not exist in the board definition file. Using default settings.
#endif

//! 8-bits connector pin
#if !defined(SD_SLOT_8BITS)                 || \
!defined(SD_SLOT_8BITS_CLK_PIN)             || \
!defined(SD_SLOT_8BITS_CLK_FUNCTION)        || \
!defined(SD_SLOT_8BITS_CMD_PIN)             || \
!defined(SD_SLOT_8BITS_CMD_FUNCTION)        || \
!defined(SD_SLOT_8BITS_DATA0_PIN)           || \
!defined(SD_SLOT_8BITS_DATA0_FUNCTION)      || \
!defined(SD_SLOT_8BITS_DATA1_PIN)           || \
!defined(SD_SLOT_8BITS_DATA1_FUNCTION)      || \
!defined(SD_SLOT_8BITS_DATA2_PIN)           || \
!defined(SD_SLOT_8BITS_DATA2_FUNCTION)      || \
!defined(SD_SLOT_8BITS_DATA3_PIN)           || \
!defined(SD_SLOT_8BITS_DATA3_FUNCTION)      || \
!defined(SD_SLOT_8BITS_DATA4_PIN)           || \
!defined(SD_SLOT_8BITS_DATA4_FUNCTION)      || \
!defined(SD_SLOT_8BITS_DATA5_PIN)           || \
!defined(SD_SLOT_8BITS_DATA5_FUNCTION)      || \
!defined(SD_SLOT_8BITS_DATA6_PIN)           || \
!defined(SD_SLOT_8BITS_DATA6_FUNCTION)      || \
!defined(SD_SLOT_8BITS_DATA7_PIN)           || \
!defined(SD_SLOT_8BITS_DATA7_FUNCTION)      || \
!defined(SD_SLOT_8BITS_CARD_DETECT)         || \
!defined(SD_SLOT_8BITS_CARD_DETECT_VALUE)   || \
!defined(SD_SLOT_8BITS_WRITE_PROTECT)       || \
!defined(SD_SLOT_8BITS_WRITE_PROTECT_VALUE) 
# define SD_SLOT_8BITS                      0
# define SD_SLOT_8BITS_CLK_PIN              AVR32_MCI_CLK_0_PIN
# define SD_SLOT_8BITS_CLK_FUNCTION         AVR32_MCI_CLK_0_FUNCTION
# define SD_SLOT_8BITS_CMD_PIN              AVR32_MCI_CMD_0_PIN
# define SD_SLOT_8BITS_CMD_FUNCTION         AVR32_MCI_CMD_0_FUNCTION
# define SD_SLOT_8BITS_DATA0_PIN            AVR32_MCI_DATA_0_PIN
# define SD_SLOT_8BITS_DATA0_FUNCTION       AVR32_MCI_DATA_0_FUNCTION
# define SD_SLOT_8BITS_DATA1_PIN            AVR32_MCI_DATA_1_PIN
# define SD_SLOT_8BITS_DATA1_FUNCTION       AVR32_MCI_DATA_1_FUNCTION
# define SD_SLOT_8BITS_DATA2_PIN            AVR32_MCI_DATA_2_PIN
# define SD_SLOT_8BITS_DATA2_FUNCTION       AVR32_MCI_DATA_2_FUNCTION
# define SD_SLOT_8BITS_DATA3_PIN            AVR32_MCI_DATA_3_PIN
# define SD_SLOT_8BITS_DATA3_FUNCTION       AVR32_MCI_DATA_3_FUNCTION
# define SD_SLOT_8BITS_DATA4_PIN            AVR32_MCI_DATA_4_PIN
# define SD_SLOT_8BITS_DATA4_FUNCTION       AVR32_MCI_DATA_4_FUNCTION
# define SD_SLOT_8BITS_DATA5_PIN            AVR32_MCI_DATA_5_PIN
# define SD_SLOT_8BITS_DATA5_FUNCTION       AVR32_MCI_DATA_5_FUNCTION
# define SD_SLOT_8BITS_DATA6_PIN            AVR32_MCI_DATA_6_PIN
# define SD_SLOT_8BITS_DATA6_FUNCTION       AVR32_MCI_DATA_6_FUNCTION
# define SD_SLOT_8BITS_DATA7_PIN            AVR32_MCI_DATA_7_PIN
# define SD_SLOT_8BITS_DATA7_FUNCTION       AVR32_MCI_DATA_7_FUNCTION
# define SD_SLOT_8BITS_CARD_DETECT          AVR32_PIN_PB11
# define SD_SLOT_8BITS_CARD_DETECT_VALUE    0
# define SD_SLOT_8BITS_WRITE_PROTECT        AVR32_PIN_PX57
# define SD_SLOT_8BITS_WRITE_PROTECT_VALUE  1
# warning The SD/MMC 8bits Slots register configuration does not exist in the board definition file. Using default settings.
#endif

//! @}

#endif  // _CONF_SD_MMC_MCI_H_
