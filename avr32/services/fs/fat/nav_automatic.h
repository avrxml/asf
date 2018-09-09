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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


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
