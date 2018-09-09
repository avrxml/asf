/*****************************************************************************
 *
 * \file
 *
 * \brief FAT 12/16/32 Unicode Service.
 *
 * This file defines a useful set of functions to decode unicode string from ASCII, UTF8, UTF16
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

