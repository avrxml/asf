/*****************************************************************************
 *
 * \file
 *
 * \brief QT60168 configuration file.
 *
 * This file contains the possible external configuration of the QT60168.
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


#ifndef _CONF_QT60168_H_
#define _CONF_QT60168_H_

#include "qt60168.h"


//_____ D E F I N I T I O N S ______________________________________________

//! First chip select used by QT60168 components on the SPI module instance.

#define QT60168_SPI_FIRST_NPCS      QT60168_SPI_NCPS

//! SPI master speed in Hz.
#define QT60168_SPI_MASTER_SPEED    1000000

//! Number of bits in each SPI transfer.
#define QT60168_SPI_BITS            8

#define QT60168_NTHR        0x0F
#define QT60168_NDRIFT      0x0F
#define QT60168_NDIL        0x02
#define QT60168_FDIL        0x04
#define QT60168_NRD         0x14
#define QT60168_BL          0x02
#define QT60168_AKS         0x00
#define QT60168_SSYNC       0x00
#define QT60168_MSYNC       0x00
#define QT60168_BS          0x01
#define QT60168_LSL         0x0064
#define QT60168_EEPROM_CRC  0x0A

//! if define, DRDY is not checked, a delay function to wait chip to be ready
//#define QT60168_DISABLE_DRDY

#if (!defined _QT60168_C_)
extern
#endif
const qt60168_setups_block_t qt60168_setups_block
#if (defined _QT60168_C_)
=
{
#define QT60168_KEY_CONFIG(KEY_NUMBER, FIRST_KEY) \
    .key_cfg[FIRST_KEY + KEY_NUMBER].ndrift_nthr   = QT60168_NTHR + (QT60168_NDRIFT << 4), \
    .key_cfg[FIRST_KEY + KEY_NUMBER].fdil_ndil     = QT60168_NDIL + (QT60168_FDIL << 4), \
    .key_cfg[FIRST_KEY + KEY_NUMBER].nrd           = QT60168_NRD, \
    .key_cfg[FIRST_KEY + KEY_NUMBER].ssync_aks_bl  = (QT60168_BL << 4) + (QT60168_AKS <<6) + (QT60168_SSYNC << 7),

#define QT60168_KEY_NOT_ACTIVATED(KEY_NUMBER, FIRST_KEY) \
    .key_cfg[FIRST_KEY + KEY_NUMBER].ndrift_nthr   = 0, \
    .key_cfg[FIRST_KEY + KEY_NUMBER].fdil_ndil     = 0, \
    .key_cfg[FIRST_KEY + KEY_NUMBER].nrd           = 0, \
    .key_cfg[FIRST_KEY + KEY_NUMBER].ssync_aks_bl  = 0,

    MREPEAT(16, QT60168_KEY_CONFIG, 0)
    MREPEAT( 8, QT60168_KEY_NOT_ACTIVATED, 16)

#undef QT60168_KEY_CONFIG
#undef QT60168_KEY_NOT_ACTIVATED

  .msync= QT60168_MSYNC <<6,
  .bs   = QT60168_BS       ,
  .lsl  = QT60168_LSL      ,
  .crc  = QT60168_EEPROM_CRC
}
#endif
;


#endif  // _CONF_QT60168_H_
