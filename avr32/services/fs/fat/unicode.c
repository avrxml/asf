/*****************************************************************************
 *
 * \file
 *
 * \brief FAT 12/16/32 Unicode Service.
 *
 * This file defines a useful set of functions to decode unicode string from ASCII, UTF8, UTF16
 *
 *
 * Copyright (c) 2009 Atmel Corporation. All rights reserved.
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


//_____  I N C L U D E S ___________________________________________________

#include "compiler.h"
#include "unicode.h"


//_____ D E F I N I T I O N S ______________________________________________


//! Detect the text format via a header (3 bytes)
//!
//! @param  header      header to analyze (array of 3B)
//!
//! @return text format identifier
//!
uint8_t    unicode_header_scan( uint8_t* header )
{
   if( (header[0] == 0xEF)
   &&  (header[1] == 0xBB)
   &&  (header[2] == 0xBF) )
   {
      return UNI_TYPE_UTF8;
   }
   if( (header[0] == 0xFE)
   &&  (header[1] == 0xFF) )
   {
      return UNI_TYPE_UTF16BE;
   }
   if( (header[0] == 0xFF)
   &&  (header[1] == 0xFE) )
   {
      return UNI_TYPE_UTF16LE;
   }
   return UNI_TYPE_ASCII;
}


//! Return the header corresponding at text format identifier
//!
//! @param  txt_format  UNI_TYPE_UTF8, UNI_TYPE_UTF16BE, UNI_TYPE_UTF16LE, UNI_TYPE_ASCII
//! @param  header      header to fill (array of 3B)
//!
//! @return the size of header
//!
uint8_t    unicode_header_get( uint8_t* header, uint8_t txt_format )
{
   switch( txt_format )
   {
      case UNI_TYPE_UTF8:
      header[0] = 0xEF;
      header[1] = 0xBB;
      header[2] = 0xBF;
      return 3;

      case UNI_TYPE_UTF16BE:
      header[0] = 0xFE;
      header[1] = 0xFF;
      return 2;

      case UNI_TYPE_UTF16LE:
      header[0] = 0xFF;
      header[1] = 0xFE;
      return 2;
   }
   return 0;
}

//! Conversion UTF-8 to unicode
//!
//! @param  utf8     array with utf8 codes (array of 3B max)
//! @param  unicode  pointer used to store the character unicode decoded
//!
//! @return the size of UTF8 decoded
//!
uint8_t    utf8_to_unicode( uint8_t* utf8, uint16_t* unicode )
{
   uint8_t c0,c1,c2;

   // Take 3 bytes
   c0 = utf8[0];
   c1 = utf8[1];
   c2 = utf8[2];

   if( 0x00 == (c0 & 0x80) )
   {
      *unicode = c0;
      return 1;
   }
   if( 0xC0 == (c0 & 0xE0) )
   {
      *unicode = ((uint16_t)(c0 & 0x1F)<<6) | ((uint16_t)(c1 & 0x3F));
      return 2;
   }
   if( 0xE0 == (c0 & 0xF0) )
   {
      *unicode = ((uint16_t)(c0 & 0x0F)<<(6+6)) | ((uint16_t)(c1 & 0x3F)<<(6)) | ((uint16_t)(c2 & 0x3F));
      return 3;
   }
   // Error !
   *unicode = c0;
   return 1;
}

