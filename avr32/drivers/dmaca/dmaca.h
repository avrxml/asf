/*****************************************************************************
 *
 * \file
 *
 * \brief DMACA driver for AVR UC3.
 *
 * This file contains basic functions for the DMACA, with support for all
 * modes and settings.
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
