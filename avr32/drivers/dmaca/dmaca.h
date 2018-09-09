/*****************************************************************************
 *
 * \file
 *
 * \brief DMACA driver for AVR UC3.
 *
 * This file contains basic functions for the DMACA, with support for all
 * modes and settings.
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

#ifndef _DMACA_H_
#define _DMACA_H_

/**
 * \defgroup group_avr32_drivers_dmaca MEMORY - DMACA - DMA Controller A
 *
 * The DMA (Direct Memory Access Controller) Controller transfers data from a
 * source peripheral to a destination peripheral over one or more System Bus.
 *
 * \{
 */

#include <avr32/io.h>
#include "compiler.h"

//! Transfer width
#define BYTE        AVR32_DMACA_8_BITS
#define HALF_WORD   AVR32_DMACA_16_BITS
#define WORD        AVR32_DMACA_32_BITS

//! Chunk size
#define CHUNK_1     AVR32_DMACA_1_DATA_ITEM
#define CHUNK_4     AVR32_DMACA_4_DATA_ITEMS
#define CHUNK_8     AVR32_DMACA_8_DATA_ITEMS
#define CHUNK_16    AVR32_DMACA_16_DATA_ITEMS
#define CHUNK_32    AVR32_DMACA_32_DATA_ITEMS

// !Addressing mode
#define INC_MODE    AVR32_DMACA_INCREMENT
#define DEC_MODE    AVR32_DMACA_DECREMENT
#define FIX_MODE    AVR32_DMACA_NO_CHANGE

//! Flow controller mode
#define DMACA_M2M   AVR32_DMACA_MEM_TO_MEM_BY_DMACA
#define DMACA_M2P   AVR32_DMACA_MEM_TO_PRPH_BY_DMACA
#define DMACA_P2M   AVR32_DMACA_PRPH_TO_MEM_BY_DMACA
#define DMACA_P2P   AVR32_DMACA_PRPH_TO_PRPH_BY_DMACA

//! General
#define false    0
#define true     1

//! Hardware/Software Handshaking select
#define SOFTWARE    AVR32_DMACA_SOFTWARE
#define HARDWARE    AVR32_DMACA_HARDWARE

//! HSB interfaces
#define DMACA1      0
#define DMACA2      1

//! Handshake interfaces
#define HIF0    0
#define HIF1    1
#define HIF2    2
#define HIF3    3
#define HIF4    4
#define HIF5    5

#define DW_DMAC_CHAN_SAR      0x000
#define DW_DMAC_CHAN_DAR      0x008
#define DW_DMAC_CHAN_LLP      0x010
#define DW_DMAC_CHAN_CTL      0x018
#define DW_DMAC_CHAN_CFG      0x040

typedef struct {
  int   channel_no;
  unsigned long src_address;
  unsigned long dst_address;
  char    dst_width;
  char    src_width;
  char    dst_add_mode;
  char    src_add_mode;
  char    dst_chunk_size;
  char    src_chunk_size;
  char    flow_controller;
  char    dst_hsb_if;
  char    src_hsb_if;
  unsigned int  transfer_size;
  char    dst_handshake;
  char    src_handshake;
  char    src_handshake_if;
  char    dst_handshake_if;
} dmaca_channel_options_t;


typedef struct {
  unsigned char Tfr;
  unsigned char Block;
  unsigned char SrcTran;
  unsigned char DstTran;
  unsigned char Err;
}dmaca_interrupt_t;

/**
 * \}
 */

#endif  // _DMACA_H_
