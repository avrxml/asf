/*****************************************************************************
 *
 * \file
 *
 * \brief FAT 12/16/32 Services.
 *
 * This plug-in allow a navigation in filter and flat mode
 * This plug-in is connected on the navigation module.
 * The filter mode includes in the file list directories, sub directories and only files and sub files corresponding at a extension
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

#ifndef _NAV_FILTERFLAT_H_
#define _NAV_FILTERFLAT_H_

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
bool  nav_filterflat_mount( void );

//! This function initializes the file filtered list on the root directory
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
bool  nav_filterflat_root( void );

//! This function resets the pointer of selection, so "no file selected" in filtered file list
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
bool  nav_filterflat_reset( void );

//! This function goes to the next position in the filtered file list
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
bool  nav_filterflat_next( void );

//! This function goes to the previous position in filtered file list
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
bool  nav_filterflat_previous( void );

//! This function returns the position of selected file in filtered file list
//!
//! @return    position of selected file in directory (0 is the first position)
//! @return    FS_NO_SEL, in case of no file selected
//!
//!
uint16_t   nav_filterflat_get( void );

//! This function goes to a position in filtered file list
//!
//! @param     u16_newpos     new position to select (0 is the first position)
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool  nav_filterflat_goto( uint16_t u16_newpos );

//! This function searches a file name in file list filtered
//!
//! @param     sz_name        name to search (UNICODE or ASCII) <br>
//!                           The name must end with NULL or '*' value
//! @param     b_match_case   false to ignore the case
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
//! @verbatim
//! This function starts a search at the next position of the current in filtered file list
//! @endverbatim
//!
bool  nav_filterflat_findname( const FS_STRING sz_name , bool b_match_case );

//! This function computes the number of files and directories in filtered file list
//!
//! @return    number of files and directories present in filtered file list
//!
uint16_t   nav_filterflat_nb( void );

//! This function enters in the selected directory
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
bool  nav_filterflat_cd( void );

//! This function goes to at the parent directory
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
//! @verbatim
//! After the selected file is the first entry
//! @endverbatim
//!
bool  nav_filterflat_gotoparent( void );

#endif  // _NAV_FILTERFLAT_H_
