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
