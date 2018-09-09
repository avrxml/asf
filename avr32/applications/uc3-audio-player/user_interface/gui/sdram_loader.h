/**************************************************************************
 *
 * \file
 *
 * \brief SDRAM loader.
 *
 ***************************************************************************/
/**
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
