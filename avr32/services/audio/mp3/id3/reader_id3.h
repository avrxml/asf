/*****************************************************************************
 *
 * \file
 *
 * \brief ID3 metadata parser.
 *
 * This file contains the algorithms to read ID3 information.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
