/*****************************************************************************
 *
 * \file
 *
 * \brief Navigation automatic Services.
 *
 * Automatic navigation has been developed for the player and viewer.
 * The plug-in provides a specific "file list".
 * The "file list" contains all files included in a playlist or it is built with the following user specifications :
 *   - extension filter
 *   - limitation scan (folder, folder and sub folder, one disk, all disks)
 * The plug-in can manage a random feature.
 * Note: The play list limitations is 65535 files.
 *
 *
 * Copyright (c) 2009 Atmel Corporation. All rights reserved.
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
 *****************************************************************************/


#ifndef _NAVAUTO_H_
#define _NAVAUTO_H_

#include "fs_com.h"
#include "fat.h"
#include "navigation.h"
#include "play_list.h"

//! Structure to store the directory limitation of the file list
typedef  uint8_t    Navauto_mode;

//! \name Definition of the directory limitations of the file list
//! @{
#define  NAVAUTO_MODE_DISKS          0
#define  NAVAUTO_MODE_DISK           1
#define  NAVAUTO_MODE_DIRONLY        2
#define  NAVAUTO_MODE_DIRSUB         3
#define  NAVAUTO_MODE_NB             4
//! @}

//! Structure to store the random state
typedef  uint8_t    Navauto_rand;

//! \name Definition of the random states
//! @{
#define  NAVAUTO_RAND_OFF            0
#define  NAVAUTO_RAND_ON             1
//! @}

typedef union
{
  uint8_t all;
  struct
  {
    uint8_t                      : 7;
    uint8_t folder_change        : 1;
  };
} Navauto_mov_bitfield_status;

//! Structure to store the status from function navauto_mov()
typedef struct
{
  uint8_t status;
  Navauto_mov_bitfield_status bitfield;
} Navauto_mov_state;


//! \name Status list of function navauto_mov()
//! @{
#define  NAVAUTO_MOV_OK              0
#define  NAVAUTO_MOV_OK_LOOP         1
#define  NAVAUTO_MOV_EMPTY           2
#define  NAVAUTO_MOV_DISKNOPRESENT   3
#define  NAVAUTO_MOV_DISKERROR       4
//! @}

Navauto_mode   navauto_getmode        ( void );
bool           navauto_setmode        ( Navauto_mode exp_mode );
Navauto_rand   navauto_getrand        ( void );
void           navauto_setrand        ( Navauto_rand rand );
void           navauto_init           ( const FS_STRING sz_filterext );
bool           navauto_open           ( bool b_playlist , uint16_t pos );
uint16_t       navauto_close          ( void );
uint16_t       navauto_getnb          ( void );
uint16_t       navauto_getpos         ( void );
bool           navauto_setpos         ( uint16_t u16_pos );
typedef enum
{
  NAVAUTO_MOV_OPTS_NONE               = 0,
  NAVAUTO_MOV_OPTS_STAY_CURRENT_DIR   = 1
} navauto_mov_options_t;
Navauto_mov_state navauto_mov         ( bool b_direction, navauto_mov_options_t options );

#endif  // _NAVAUTO_H_
