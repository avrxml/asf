/*****************************************************************************
 *
 * \file
 *
 * \brief FAT 12/16/32 Services.
 *
 * This plug-in allow a navigation in FLAT mode
 * This plug-in is connected on the navigation module.
 * The flat mode includes in the file list all directories and all files of the current directory AND the sub directories
 * Before use the flat mode, you must call nav_partition_mount() to reset navigation
 *
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _NAV_FLAT_H_
#define _NAV_FLAT_H_

#include "fs_com.h"

//**********************************************************************
//****************** File list navigation FLAT functions ***************

//! This function mounts the selected partition
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
//! @verbatim
//! If the FS_MULTI_PARTITION option is disabled
//! then the mount routine selects the first partition supported by file system. <br>
//! After mount, the file list contains files and directories of ROOT directory
//! @endverbatim
//!
bool  nav_flat_mount( void );

//! This function initializes the file list on the root directory
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
bool  nav_flat_root( void );

//! This function resets the selection pointer, so "no file is selected" in file list FLAT
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
bool  nav_flat_reset( void );

//! This function checks if a file is selected
//!
//! @return    true  if a file is selected
//! @return    false if no file is selected
//!
bool  nav_flat_validpos( void );

//! This function checks if no file is opened
//!
//! @return    true  if no file is opened
//! @return    false if a file is opened
//!
bool  nav_flat_fileisnotopen( void );

//! Go to next file or directory in file list FLAT
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
bool  nav_flat_next( void );

//! Go to previous file or directory in file list FLAT
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
bool  nav_flat_previous( void );

//! This function returns the position of selected file in file list FLAT
//!
//! @return    position of selected file (0 is the first position)
//! @return    FS_NO_SEL, in case of no file selected
//!
uint16_t   nav_flat_get( void );

//! This function goes to a position in file list FLAT
//!
//! @param     u16_newpos     new position to select (0 is the first position)
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool  nav_flat_goto( uint16_t u16_newpos );

//! This function searches a file name in file list FLAT
//!
//! @param     sz_name        name to search (UNICODE or ASCII) <br>
//!                           It must be terminated by NULL or '*' value
//! @param     b_match_case   false to ignore the case
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
//! @verbatim
//! This function starts a search at the next position of the current in file list
//! @endverbatim
//!
bool  nav_flat_findname( const FS_STRING sz_name , bool b_match_case );

//! This function computes the number of files and directories present in the file list FLAT
//!
//! @return    number of files and directories present in the file list FLAT
//!
uint16_t   nav_flat_nb( void );

//! This function enters in the selected directory in file list FLAT
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
bool  nav_flat_cd( void );

//! This function goes to the parent directory
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
//! @verbatim
//! After the selected file is the first entry of the new file list FLAT
//! @endverbatim
//!
bool  nav_flat_gotoparent( void );

#endif  // _NAV_FLAT_H_
