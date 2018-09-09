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
