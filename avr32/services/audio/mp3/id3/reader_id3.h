/*****************************************************************************
 *
 * \file
 *
 * \brief ID3 metadata parser.
 *
 * This file contains the algorithms to read ID3 information.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _READER_ID3_H_
#define _READER_ID3_H_

#include <stddef.h>
#include "compiler.h"


//_____ I N C L U D E S ____________________________________________________


//_____ D E F I N I T I O N ________________________________________________

typedef struct
{
   uint8_t    ver1;
   uint8_t    ver10;
   uint8_t    ver100;
} St_id3ver;

typedef struct
{
  size_t prepended_tag;
  size_t appended_tag;
} St_id3size;


//! @brief Return ID3 size from ID3 of current file selected
//!
extern St_id3size reader_id3_size ( void );

//! @brief Return ID3 version from ID3 of current file selected
//!
extern St_id3ver reader_id3_version ( void );

//! @brief Return title from ID3 of current file selected
//!
extern bool reader_id3_title   ( FS_STRING sz_title , uint8_t u8_size_max );

//! @brief Return artist from ID3 of current file selected
//!
extern bool reader_id3_artist ( FS_STRING sz_artist , uint8_t u8_size_max );

//! @brief Return album from ID3 of current file selected
//!
extern bool reader_id3_album   ( FS_STRING sz_album , uint8_t u8_size_max );

//! @brief Return year from ID3 of current file selected
//!
extern uint16_t reader_id3_year    ( void );

//! @brief Return track from ID3 of current file selected
//!
extern uint32_t  reader_id3_track   ( void );

//! @brief Return genre from ID3 of current file selected
//!
extern bool reader_id3_genre   ( FS_STRING sz_genre , uint8_t u8_size_max );

//! @brief Return the length of the audiofile in milliseconds
//!
extern uint32_t  reader_id3_duration ( void );

//! @brief Return embedded image mime type, image type and offset into the file.
//
//  This function returns only the first picture that was found in the file!
//
//  @mime_type is "image/jpeg" for an attached jpeg file
//
//  @u8_size_max specifies the maximum string to get for mime_type
//
//  @picture_type specifies what kind of picture it is
//  (e.g. 0x03 for front cover) more types are listed in the ID3 specification.
//
//  @offset is the offset of the raw picture data from the file beginning
//
//  @returns TURE on success
//  @returns false when no attached picture was found or an error occurred.
extern bool reader_id3_picture ( FS_STRING mime_type, uint8_t u8_size_max,
   uint8_t *picture_type, uint32_t *offset);



#endif  // _READER_ID3_H_
