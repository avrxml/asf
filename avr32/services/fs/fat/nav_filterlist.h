/*****************************************************************************
 *
 * \file
 *
 * \brief FAT 12/16/32 Services.
 *
 * This plug-in allow a navigation in filter mode
 * This plug-in is connected on the navigation module.
 * The filter mode includes in the file list directories and only files corresponding at a extension
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

#ifndef _NAV_FILTERLIST_H_
#define _NAV_FILTERLIST_H_

#include "fs_com.h"

typedef enum
{
  FL_FILE,
  FL_DIR,
  FL_ALL
} fl_type_t;

void  nav_filterlist_setfilter( const FS_STRING sz_filterext  );

bool  nav_filterlist_reset( void );

bool  nav_filterlist_next( void );

bool  nav_filterlist_previous( void );

uint16_t   nav_filterlist_get( void );

bool  nav_filterlist_goto( uint16_t u16_newpos );

bool  nav_filterlist_findname( const FS_STRING sz_name , bool b_match_case );

uint16_t  nav_filterlist_nb(    fl_type_t fl_type, const FS_STRING sz_filterext );
bool      nav_filterlist_nb_ex( fl_type_t fl_type, const FS_STRING sz_filterext, uint16_t* p_total, uint8_t retry );

bool  nav_filterlist_mount( void );

bool  nav_filterlist_root( void );

bool  nav_filterlist_cd( void );

bool  nav_filterlist_gotoparent( void );

bool  nav_filterlist_gotoindex( const Fs_index _MEM_TYPE_SLOW_ *index );


#endif  // _NAV_FILTERLIST_H_
