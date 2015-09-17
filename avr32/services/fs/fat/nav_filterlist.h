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
