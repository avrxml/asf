/**************************************************************************
 *
 * \file
 *
 * \brief SDRAM loader.
 *
 ***************************************************************************/
/**
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _SDRAM_LOADER_H
#define _SDRAM_LOADER_H

#include "string.h"
#include "dlmalloc.h"

#define SDRAM_START_ADDRESS 0xD0000000
/* Size of the MEM_SPACE in SDRAM */
#define MEM_SPACE_SIZE 0x1000000

/* SDRAM memory space */
extern mspace sdram_mspace;

typedef struct {
  const wchar_t *name;
  void *start_addr;
} ram_file_t;

// Image position in ram_files
#define STARTUP_IMAGE       0
#define DISK_NAV_IMAGE      1
#define AUDIO_PLAYER_IMAGE  2
#define COVER               3
#define IN_PROGRESS         4
#define LOADING             5
#define NOT_SUPPORTED       6
#define CONFIG_IMAGE        DISK_NAV_IMAGE
#define NUMBER_OF_IMAGES    10

extern ram_file_t ram_files[];

extern void load_sdram_data(int hsb_hz);


#endif  // _SDRAM_LOADER_H
