/**************************************************************************
 *
 * \file
 *
 * \brief FAT configuration file.
 *
 * This file contains the possible external configuration of the FAT.
 *
 * Information about file-system limitations:
 *  - Only 2-FAT FSs supported by this navigator.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _CONF_EXPLORER_H_
#define _CONF_EXPLORER_H_

#include "compiler.h"

//! Include files and compile options (do not change these settings).
#define LIB_MEM               <string.h>
#define LIB_CTRLACCESS        "ctrl_access.h"

//! Supported FATs (\c true or \c false).
#define FS_FAT_12             true
#define FS_FAT_16             true
#define FS_FAT_32             true

//! The explorer may support either the ASCII or the UNICODE string format, or both.
#define FS_ASCII              false
#define FS_UNICODE            true

//! The navigator may support only the first partition (\c false), or multiple partitions (\c true).
#define FS_MULTI_PARTITION    false

//! Maximal number of characters in file path.
#define MAX_FILE_PATH_LENGTH  30

//! Maximal size of configuration file.
#define MAX_CONFIG_FILE_SIZE  110

//! Level of features.
//! Select among:
//!  - \c FSFEATURE_READ:           All read functions.
//!  - \c FSFEATURE_WRITE:          nav_file_copy(), nav_file_paste(), nav_file_del(), file_create(), file_open(MODE_WRITE), file_write(), file_putc().
//!  - \c FSFEATURE_WRITE_COMPLET:  FSFEATURE_WRITE functions and nav_drive_format(), nav_dir_make(), nav_file_rename(), nav_file_dateset(), nav_file_attributset().
//!  - \c FSFEATURE_ALL:             All functions.
#define FS_LEVEL_FEATURES           (FSFEATURE_READ | FSFEATURE_WRITE_COMPLET)

//! Number of caches used to store a cluster list of files (interesting in case of many `open file').
//! In player mode, 1 is OK (shall be > 0).
#define FS_NB_CACHE_CLUSLIST        3

//! Maximal number of simultaneous navigators.
#define FS_NB_NAVIGATOR             4

#define FS_NAV_AUTOMATIC_NBFILE     DISABLE


/*! \name Navigator Affiliations
 *
 * Define the affiliations of the navigators.
 *
 * \note The explorer always has the navigator ID 0.
 *
 * \note The update and status files use a different navigator (2) because they are enabled at the same time.
 */
//! @{

//! Navigator used to explore disks
#define FS_NAV_ID_FILESYSTEM        0

//! Navigator used to manage song file (navigation + open file)
#define FS_NAV_ID_AUDIO             1

//! Navigator used to open the file playlist
#define FS_NAV_ID_PLAYLIST          2

#define FS_NAV_ID_COVER	3
//! The explorer uses the navigator ID 2 to open the `copy file' and the ID 0 to open the `paste file'.
#define FS_NAV_ID_COPYFILE          2 // No used in demo player

#define FS_NAV_ID_COVER             3

//! @}

/*! \name Playlist Configuration
 */
//! @{

#define  PLAYLIST_BUF_ALLOC( size )    malloc( size )
#define  PLAYLIST_BUF_FREE( buf )      free( buf )
/*
#define  PLAYLIST_BUF_ALLOC( size )    ((sizeof(g_buffer)>512)? NULL : g_buffer)
#define  PLAYLIST_BUF_FREE( buf )
*/

//! Playlist mode read only (modification impossible)
#define PL_READONLY                    true

//! @}


/*! \name Navigation Automatic Configuration
 */
//! @{
// Number maximum of file include in each random (Unit 8 files)
#define  NAVAUTO_MAX_RANGE_RAND  8     // 8*8= 64 file
// Random value method (byte value)
#include "cycle_counter.h"
#define NAVAUTO_GET_RAND()       (Get_sys_count())

#define NAV_AUTO_FILE_IN_FIRST   1

//! Enable speed optimization
#define NAVAUTO_SPEED_OPTIMIZATION              false
//! It defines the number of level in the tree architecture that will be impacted by this optimization.\n
//! If this number is equal to 1, then only the 1st folder level will be impacted by this optimization.\n
//! In counter part, the higher this number is, the more RAM memory it will need.
#define NAVAUTO_SPEED_OPTIMIZATION_LEVEL        8
//! @}

//! This define enables or disables the use of non-blocking functions for
//! playlist management.
#define PLAYLIST_USE_NON_BLOCKING_FUNCTION_SET  false

//! \name Cache feature
//! @{
//! This define enables the use of a 1-entry-cache for some specific mass
//! storage functions.
#define AI_USB_MS_USE_CACHE                     false
//! The following values have to be defined in order to enable the cache feature
//! for the associated functions.
//! - AI_USB_MS_CACHE_SPECIFIC_NAV_PLAYLIST_LINK_NB
//! - AI_USB_MS_CACHE_SPECIFIC_NAV_PLAYLIST_LINK_NAME
//! @}

#endif  // _CONF_EXPLORER_H_
