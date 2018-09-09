/**************************************************************************
 *
 * \file
 *
 * \brief Bufferization player configuration file.
 *
 * Copyright (c) 2006-2018 Microchip Technology Inc. and its subsidiaries.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _CONF_BUFF_PLAYER_H_
#define _CONF_BUFF_PLAYER_H_

//! Maximal number of buffers.
#define BUFF_PLAYER_MAX_BUF_CNT     64
//! The starting address of the memory dedicated for the bufferization.

#if BOARD == EVK1104
//! Initialization function
#define BUFF_INIT() {}
#define BUFF_PLAYER_ADDRESS_START   ((void *) ( AVR32_INTRAM0_ADDRESS))
//! The size of the memory dedicated for the bufferization.
#define BUFF_PLAYER_MEMORY_SIZE     (AVR32_INTRAM0_SIZE+AVR32_INTRAM1_SIZE)
#else
//! Initialization function
#include "sdramc.h"
#define BUFF_INIT() { \
  sdramc_init(FHSB_HZ); \
}
//! Other Board use SDRAM for audio buffer
//! The starting address of the memory dedicated for the bufferization.
#define BUFF_PLAYER_ADDRESS_START   ((void *) (AVR32_EBI_CS1_ADDRESS + (1024*1024*16)))
//! The size of the memory dedicated for the bufferization.
#define BUFF_PLAYER_MEMORY_SIZE     (128*1024)
#endif

#endif // _CONF_BUFF_PLAYER_H_
