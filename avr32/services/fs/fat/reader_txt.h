/*****************************************************************************
 *
 * \file
 *
 * \brief FAT 12/16/32 text reader service.
 *
 * This file defines a useful set of functions for the text file accesses on
 * AVR32 devices.
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

#ifndef _READER_TXT_H_
#define _READER_TXT_H_

#include "fs_com.h"
#include "fat.h"

//_____ D E C L A R A T I O N S ____________________________________________


//! Open a selected text file in the current navigator
//!
//! @param     b_readonly     true to open the text file in read only access
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool  reader_txt_open( bool b_readonly );

//! Create a new text file
//!
//! @param     sz_name        contains the file name (ASCII or UNICODE )
//! @param     u8_txt_format  text format to use (UNI_TYPE_UTF8, UNI_TYPE_UTF16BE, UNI_TYPE_UTF16LE, UNI_TYPE_ASCII)
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool  reader_txt_new( const FS_STRING sz_name , uint8_t u8_txt_format );

//! This function goes at the beginning of file and read the text header
//!
void  reader_txt_beg( void );

//! This function allow to select the text format to decode
//!
//! @param     u8_txt_format  text format selected (UNI_TYPE_UTF8, UNI_TYPE_UTF16BE, UNI_TYPE_UTF16LE, UNI_TYPE_ASCII)
//!
void  reader_txt_select_format( uint8_t u8_txt_format );

//! This function fills an ASCII buffer with the current text line of opened file
//!
//! @param     b_unicode      true to return a unicode string (UTF16), ASCII otherwise over there
//! @param     string         string used to store text line
//! @param     u16_str_size   maximum size of string (unit character, 1B for ASCII, 2B for unicode)
//!
//! @return    size of the line (may be < or > at u16_str_size)
//! @return    0, in case of error
//!
//! @verbatim
//! This routine remove the character '\n' '\r'
//! @endverbatim
//!
uint16_t   reader_txt_get_line( bool b_unicode, FS_STRING string , uint16_t u16_str_size );

//! This function jumps text lines
//!
//! @param     nb_line        number of lines to jump
//!
//! @return    false in case of error
//!
bool  reader_txt_jump_line( uint16_t nb_line );

//! Close text file
//!
void  reader_txt_close( void );

#endif  // _READER_TXT_H_
