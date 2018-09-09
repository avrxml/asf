/*****************************************************************************
 *
 * \file
 *
 * \brief FAT 12/16/32 Text reader service.
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


//_____  I N C L U D E S ___________________________________________________

#include "conf_explorer.h"
#include LIB_MEM
#include "file.h"
#include "navigation.h"
#include "reader_txt.h"
#include "unicode.h"


//_____ D E F I N I T I O N S ______________________________________________


//! Open a selected text file in the current navigator
//!
//! @param     b_readonly     true to open the text file in read only access
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool  reader_txt_open( bool b_readonly )
{
   if( !file_open( (b_readonly)? FOPEN_MODE_R : FOPEN_MODE_R_PLUS ) )
      return false; // Open error
   reader_txt_beg();
   return true;
}


//! Create a new text file
//!
//! @param     sz_name        contains the file name (ASCII or UNICODE )
//! @param     u8_txt_format  text format to use (UNI_TYPE_UTF8, UNI_TYPE_UTF16BE, UNI_TYPE_UTF16LE, UNI_TYPE_ASCII)
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool  reader_txt_new( const FS_STRING sz_name , uint8_t u8_txt_format )
{
   // Table to store the header of text file
   uint8_t    string_header[UNI_MAX_HEADER_SIZE];
   uint8_t    u8_header_size;

   // Create empty file
   if ( !nav_file_create( sz_name ))
      return false;

   fs_g_nav_entry.u8_txt_format = u8_txt_format;

   // Write at the beginning of file the text header
   u8_header_size = unicode_header_get( string_header, fs_g_nav_entry.u8_txt_format );
   file_write_buf( string_header , u8_header_size );
   return true;
}


//! This function goes at the beginning of file and read the text header
//!
void  reader_txt_beg( void )
{
   // Table to store the header of text file
   uint8_t    string_header[UNI_MAX_HEADER_SIZE];

   file_seek( 0, FS_SEEK_SET );  // go to at the beginning of file

   // Read the header to detect the text format (ASCII, UNICODE LE, UNICODE BE, UTF8)
   memset( string_header, 0, UNI_MAX_HEADER_SIZE );     // By default ASCII
   file_read_buf( string_header , UNI_MAX_HEADER_SIZE );
   fs_g_nav_entry.u8_txt_format = unicode_header_scan( string_header );
   // Set cursor after header
   file_seek( unicode_header_get( string_header, fs_g_nav_entry.u8_txt_format ), FS_SEEK_SET );
}


//! This function allow to select the text format to decode
//!
//! @param     u8_txt_format  text format selected (UNI_TYPE_UTF8, UNI_TYPE_UTF16BE, UNI_TYPE_UTF16LE, UNI_TYPE_ASCII)
//!
void  reader_txt_select_format( uint8_t u8_txt_format )
{
   fs_g_nav_entry.u8_txt_format = u8_txt_format;
}


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
uint16_t   reader_txt_get_line( bool b_unicode, FS_STRING string , uint16_t u16_str_size )
{
   uint8_t    utf8[UNI_MAX_UTF8_SIZE];
   uint8_t    size_utf8_buf=0;
   uint8_t    size_utf8_dec=0;
   uint16_t   u16_size_line = 1;   // 1 to compute null terminator
   uint16_t   u16_unicode;
   bool  b_error = false;

   nav_checkdisk_disable();   // To optimize speed

   while( 0 == file_eof() && !b_error )
   {
      // Get a unicode value
      switch( fs_g_nav_entry.u8_txt_format )
      {
         case UNI_TYPE_UTF8:
         // Remove UTF8 codes used
         if( 0 != size_utf8_dec )
         {
            uint8_t u8_i;
            for( u8_i=0; u8_i<(UNI_MAX_UTF8_SIZE-size_utf8_dec); u8_i++ )
            {
               utf8[u8_i]=utf8[u8_i+size_utf8_dec];
            }
            size_utf8_buf -= size_utf8_dec;
            size_utf8_dec =0;
         }
         // Complete UTF8 array
         size_utf8_buf += file_read_buf( &utf8[size_utf8_buf], (UNI_MAX_UTF8_SIZE-size_utf8_buf) );
         // Decode UTF8 to unicode
         size_utf8_dec = utf8_to_unicode( &utf8[0], &u16_unicode );
         break;

         case UNI_TYPE_UTF16BE:
         MSB(u16_unicode) = file_getc();
         LSB(u16_unicode) = file_getc();
         if (LSB(u16_unicode) == (FS_EOF & 0xFF) && fs_g_status != FS_ERR_EOF)
           b_error = true;
         break;

         case UNI_TYPE_UTF16LE:
         LSB(u16_unicode) = file_getc();
         MSB(u16_unicode) = file_getc();
         if (MSB(u16_unicode) == (FS_EOF & 0xFF) && fs_g_status != FS_ERR_EOF)
           b_error = true;
         break;

         default: // ASCII or other
         u16_unicode = file_getc();
         if (u16_unicode == FS_EOF && fs_g_status != FS_ERR_EOF)
           b_error = true;
         break;
      }

      // Check character to remove
      if( '\r' == u16_unicode )
         continue;   // Ignore character

      // Check end of line
      if((  0  == u16_unicode )
      || ('\n' == u16_unicode ) )
         break;      // End of line

      u16_size_line++;
      // Fill string
      if( 1 < u16_str_size )
      {
         if( b_unicode )
         {
            ((FS_STR_UNICODE)string)[0] = u16_unicode;
         }else{
            string[0] = u16_unicode;
         }
         string += (b_unicode? 2 : 1 );
         u16_str_size--;
      }
   }
   // HERE, the line is read

   if( UNI_TYPE_UTF8 == fs_g_nav_entry.u8_txt_format )
   {
      // Re position cursor on next UTF8
      file_seek( (size_utf8_buf-size_utf8_dec) , FS_SEEK_CUR_RE );
   }

   // Handle error cases
   if (b_error)
   {
     nav_checkdisk_enable();
     return 0;
   }

   // Add Null terminate
   if( 0 != u16_str_size )
   {
      if( b_unicode )
      {
         ((FS_STR_UNICODE)string)[0] = 0;
      }else{
         string[0] = 0;
      }
   }
   nav_checkdisk_enable();
   return u16_size_line;
}


//! This function jumps text lines
//!
//! @param     nb_line        number of lines to jump
//!
//! @return    false in case of error
//!
bool  reader_txt_jump_line( uint16_t nb_line )
{
   while( 0 != nb_line )
   {
      if( 0 != file_eof() )
         return false;
      reader_txt_get_line( false, NULL, 0 );
      nb_line--;
   }
   return true;
}


//! Close text file
//!
void  reader_txt_close( void )
{
   file_close();
}

