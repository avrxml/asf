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


//_____  I N C L U D E S ___________________________________________________

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

// Include the file system access
#include "navigation.h"
#include "file.h"
#include "reader_txt.h"
#include LIB_CTRLACCESS
#include LIB_MEM
#include "reader_id3.h"

//_____ D E C L A R A T I O N ______________________________________________

//! Private routines for all ID3 versions
static  St_id3ver   reader_id3_getver     ( void );
static  uint16_t    reader_id3_read_string( FS_STRING string , uint16_t size_string , uint8_t format );
static  uint32_t    reader_id3_strtoul    ( FS_STRING string );

//! functions version 1
static  bool        reader_id3_v1_title   ( FS_STRING sz_title , uint8_t u8_size_max );
static  bool        reader_id3_v1_artist  ( FS_STRING sz_artist , uint8_t u8_size_max );
static  bool        reader_id3_v1_album   ( FS_STRING sz_album , uint8_t u8_size_max );
static  bool        reader_id3_v1_year    ( FS_STRING sz_year , uint8_t u8_size_max );
static  bool        reader_id3_v1_track   ( FS_STRING sz_track , uint8_t u8_size_max );
static  bool        reader_id3_v1_genre   ( FS_STRING sz_genre , uint8_t u8_size_max );
static  bool        reader_id3_v1_string  ( uint8_t pos_seek, uint8_t size, FS_STRING sz_string , uint8_t u8_size_max  );

//! functions version 2
static  bool        reader_id3_v2_title   ( St_id3ver ver, FS_STRING sz_title , uint8_t u8_size_max );
static  bool        reader_id3_v2_artist  ( St_id3ver ver, FS_STRING sz_artist , uint8_t u8_size_max );
static  bool        reader_id3_v2_album   ( St_id3ver ver, FS_STRING sz_album , uint8_t u8_size_max );
static  bool        reader_id3_v2_year    ( St_id3ver ver, FS_STRING sz_year , uint8_t u8_size_max );
static  bool        reader_id3_v2_track   ( St_id3ver ver, FS_STRING sz_track , uint8_t u8_size_max );
static  bool        reader_id3_v2_genre   ( St_id3ver ver, FS_STRING sz_genre , uint8_t u8_size_max );
static  bool        reader_id3_v2_string  ( St_id3ver ver, char* tag, FS_STRING sz_title , uint8_t u8_size_max  );
static  bool        reader_id3_v2_duration( St_id3ver ver, FS_STRING sz_duration , uint8_t u8_size_max );
static  uint32_t    reader_id3_v2_findtag ( St_id3ver version, char *tag );
static bool reader_id3_v2_picture ( St_id3ver ver, FS_STRING mime_type,
   uint8_t u8_size_max, uint8_t *picture_type, uint32_t *offset);

//-------------------------------------------
//---------- ID3 public routines ------------
//-------------------------------------------

//! @brief Return ID3 size from ID3 of current file selected
//!
St_id3size reader_id3_size ( void )
{
  St_id3size size =
  {
    .prepended_tag  = 0,
    .appended_tag   = 0
  };

  if (!file_open(FOPEN_MODE_R))
    return size;

  if (   file_getc() == 'I'
      && file_getc() == 'D'
      && file_getc() == '3')
  {
    file_seek(2, FS_SEEK_CUR_FW);
    size.prepended_tag =   10                               // Header
                         + ((file_getc() & 0x10) ? 10 : 0)  // Footer
                         + (file_getc() << (24 - 3))        // Extended header + frames + padding
                         + (file_getc() << (16 - 2))
                         + (file_getc() << ( 8 - 1))
                         + (file_getc() << 0);
  }

  if (file_seek(128, FS_SEEK_END))
  {
    if (   file_getc() == 'T'
        && file_getc() == 'A'
        && file_getc() == 'G')
    {
      size.appended_tag = 128;
    }
  }

  file_close();
  return size;
}


//! @brief Return ID3 version from ID3 of current file selected
//!
St_id3ver reader_id3_version ( void )
{
   St_id3ver dflt_ver = {0, 0, 0};

   St_id3ver ver = reader_id3_getver();
   if( (0 == ver.ver1)
   || ((2 == ver.ver1) && (4 < ver.ver10)) )
   {
      file_close();
      return dflt_ver;
   }

   file_close();
   return ver;
}


//! @brief Return title from ID3 of current file selected
//!
bool reader_id3_title( FS_STRING sz_title , uint8_t u8_size_max  )
{
   bool status;
   St_id3ver ver = reader_id3_getver();
   if( (0 == ver.ver1)
   || ((2 == ver.ver1) && (4 < ver.ver10)) )
   {
      file_close();
      return false;
   }

   if( 1 == ver.ver1)
   {
      status = reader_id3_v1_title( sz_title , u8_size_max );
   }else{
      status = reader_id3_v2_title( ver, sz_title , u8_size_max );
   }
   file_close();
   return status;
}


//! @brief Return artist from ID3 of current file selected
//!
bool reader_id3_artist( FS_STRING sz_artist , uint8_t u8_size_max  )
{
   bool status;
   St_id3ver ver = reader_id3_getver();
   if( (0 == ver.ver1)
   || ((2 == ver.ver1) && (4 < ver.ver10)) )
   {
      file_close();
      return false;
   }

   if( 1 == ver.ver1)
   {
      status = reader_id3_v1_artist( sz_artist , u8_size_max );
   }else{
      status = reader_id3_v2_artist( ver, sz_artist , u8_size_max );
   }
   file_close();
   return status;
}


//! @brief Return album from ID3 of current file selected
//!
bool reader_id3_album( FS_STRING sz_album , uint8_t u8_size_max  )
{
   bool status;
   St_id3ver ver = reader_id3_getver();
   if( (0 == ver.ver1)
   || ((2 == ver.ver1) && (4 < ver.ver10)) )
   {
      file_close();
      return false;
   }

   if( 1 == ver.ver1)
   {
      status = reader_id3_v1_album( sz_album , u8_size_max );
   }else{
      status = reader_id3_v2_album( ver, sz_album , u8_size_max );
   }
   file_close();
   return status;
}


//! @brief Return year from ID3 of current file selected
//!
uint16_t  reader_id3_year( void )
{
   UNICODE sz_year[5];
   uint16_t u16_year;
   bool status;
   St_id3ver ver = reader_id3_getver();
   if( (0 == ver.ver1)
   || ((2 == ver.ver1) && (4 < ver.ver10)) )
   {
      file_close();
      return 0;
   }

   nav_string_length_disable();
   if( 1 == ver.ver1)
   {
      status = reader_id3_v1_year( (FS_STRING)sz_year , sizeof(sz_year) / sizeof(sz_year[0]) - 1 );
   }else{
      status = reader_id3_v2_year( ver, (FS_STRING)sz_year , sizeof(sz_year) / sizeof(sz_year[0]) - 1 );
   }
   u16_year = (status) ? reader_id3_strtoul( (FS_STRING)sz_year ) : 0;
   file_close();
   return u16_year;
}


//! @brief Return track from ID3 of current file selected
//!
uint32_t  reader_id3_track( void )
{
   UNICODE sz_track[22];
   uint32_t u32_track;
   bool status;
   St_id3ver ver = reader_id3_getver();
   if( (0 == ver.ver1)
   || ((2 == ver.ver1) && (4 < ver.ver10)) )
   {
      file_close();
      return 0;
   }

   nav_string_length_disable();
   if( 1 == ver.ver1)
   {
      status = reader_id3_v1_track( (FS_STRING)sz_track , sizeof(sz_track) / sizeof(sz_track[0]) - 1 );
   }else{
      status = reader_id3_v2_track( ver, (FS_STRING)sz_track , sizeof(sz_track) / sizeof(sz_track[0]) - 1 );
   }

   if (status)
   {
     if (Is_unicode)
     {
       FS_STR_UNICODE sz_wchar_track = sz_track;
       while (*sz_wchar_track)
       {
         if (*sz_wchar_track == (UNICODE)'/')
         {
           *sz_wchar_track = (UNICODE)'\0';
           break;
         }
         sz_wchar_track++;
       }
     }
     else
     {
       FS_STRING sz_char_track = (FS_STRING)sz_track;
       while (*sz_char_track)
       {
         if (*sz_char_track == '/')
         {
           *sz_char_track = '\0';
           break;
         }
         sz_char_track++;
       }
     }
     u32_track = reader_id3_strtoul( (FS_STRING)sz_track );
   }
   else
     u32_track = 0;

   file_close();
   return u32_track;
}


//! @brief Return genre from ID3 of current file selected
//!
bool reader_id3_genre( FS_STRING sz_genre , uint8_t u8_size_max  )
{
   bool status;
   St_id3ver ver = reader_id3_getver();
   if( (0 == ver.ver1)
   || ((2 == ver.ver1) && (4 < ver.ver10)) )
   {
      file_close();
      return false;
   }

   if( 1 == ver.ver1)
   {
      status = reader_id3_v1_genre( sz_genre , u8_size_max );
   }else{
      status = reader_id3_v2_genre( ver, sz_genre , u8_size_max );
   }
   file_close();
   return status;
}

bool reader_id3_picture( FS_STRING mime_type, uint8_t u8_size_max, uint8_t *picture_type,
   uint32_t *offset)
{
   bool status;
   St_id3ver ver = reader_id3_getver();
   // there is no picture tag in v1
   if(2  > ver.ver1)
   {
      file_close();
      return false;
   }

     status = reader_id3_v2_picture(ver, mime_type, u8_size_max, picture_type,
      offset);

   file_close();
   return status;
}

//! @brief Return the length of the audiofile in milliseconds
//!
uint32_t  reader_id3_duration( void )
{
   UNICODE sz_duration[11];
   uint32_t u32_duration;
   bool status;
   St_id3ver ver = reader_id3_getver();
   if( (0 == ver.ver1)
   || ((2 == ver.ver1) && (4 < ver.ver10)) )
   {
      file_close();
      return 0;
   }

   nav_string_length_disable();
   if( 1 == ver.ver1)
   {
      u32_duration = 0;
   }else{
      status = reader_id3_v2_duration( ver, (FS_STRING)sz_duration , sizeof(sz_duration) / sizeof(sz_duration[0]) - 1 );
      u32_duration = (status) ? reader_id3_strtoul( (FS_STRING)sz_duration ) : 0;
   }
   file_close();
   return u32_duration;
}


//-------------------------------------------
//----- ID3 private & common routines -------
//-------------------------------------------

//! @brief Open the file and return major version of ID3
//!
//! @return version, 0 in case of error (the file is close in this case)
//!
static St_id3ver   reader_id3_getver( void )
{
   St_id3ver version;
   version.ver1 = 0; // By default no ID3
   version.ver10 = 0;
   version.ver100 = 0;

   if( !file_open(FOPEN_MODE_R) )
      return version;   // Error open file

   // Version 2 is storage at beginning
   if( ('I' == file_getc())
   &&  ('D' == file_getc())
   &&  ('3' == file_getc()) )
   {
      // It is a version 2
      version.ver1   = 2;
      version.ver10  = file_getc();
      version.ver100 = file_getc();
      return version;
   }

   // Version 1 is storage at the end
   if( file_seek( 128 , FS_SEEK_END ) )
   {
      if( ('T' == file_getc())
      &&  ('A' == file_getc())
      &&  ('G' == file_getc()) )
      {
         // It is a version 1
         version.ver1   = 1;
         // read version 1/10
         file_seek( 3 , FS_SEEK_END );
         if( 0 == file_getc())
         {
            // version 1.10
            version.ver10  = 1;
         }else{
            // version 1.00
            version.ver10  = 0;
         }
         version.ver100 = 0;
         return version;
      }
   }

   // Version no found or no supported
   file_close();
   return version;
}


//! Read an ASCII string in an ID3 field
//!
//! @return number of char contained in tag
//!
static uint16_t reader_id3_read_string( FS_STRING string , uint16_t size_string , uint8_t format )
{
   reader_txt_select_format( format );
   return reader_txt_get_line( Is_unicode, string , size_string );
}


//! Converts a string to an unsigned long
//!
//! @return number resulting from string conversion
//!
static uint32_t reader_id3_strtoul( FS_STRING string )
{
  uint32_t number = 0;

  if (Is_unicode)
  {
    FS_STR_UNICODE sz_wchar_str = (FS_STR_UNICODE)string;
    while (*sz_wchar_str)
      number = number * 10 + (uint16_t)*sz_wchar_str++ - (uint16_t)'0';
  }
  else
  {
    FS_STRING sz_char_str = string;
    while (*sz_char_str)
      number = number * 10 + (uint8_t)*sz_char_str++ - (uint8_t)'0';
  }

  return number;
}


//-------------------------------------------
//------------- ID3 version 1 ---------------
//-------------------------------------------

//! @brief Return title from ID3 V1
//!
static bool reader_id3_v1_title( FS_STRING sz_title , uint8_t u8_size_max  )
{
   return reader_id3_v1_string( 128-3, 30, sz_title , u8_size_max );
}


//! @brief Return artist from ID3 V1
//!
static bool reader_id3_v1_artist( FS_STRING sz_artist , uint8_t u8_size_max  )
{
   return reader_id3_v1_string( 128-3-30, 30, sz_artist , u8_size_max );
}


//! @brief Return album from ID3 V1
//!
static bool reader_id3_v1_album( FS_STRING sz_album , uint8_t u8_size_max  )
{
   return reader_id3_v1_string( 128-3-30-30, 30, sz_album , u8_size_max );
}

//! @brief Return year from ID3 V1
//!
static bool reader_id3_v1_year( FS_STRING sz_year , uint8_t u8_size_max )
{
   return reader_id3_v1_string( 128-3-30-30-30, 4, sz_year , u8_size_max );
}


//! @brief Return a string from ID3 V1
//!
static bool reader_id3_v1_string( uint8_t pos_seek, uint8_t size, FS_STRING sz_string , uint8_t u8_size_max  )
{
   uint16_t u16_size;

   if( !file_seek( pos_seek, FS_SEEK_END ))
      return false;

   if( g_b_string_length )
   {
      u16_size = reader_id3_read_string( NULL, 0, 0 );   // Get only the size of string
      // In case of no NULL terminator, the size may be too long
      if( u16_size > size )
        u16_size = size+1;   // Max size of field + null terminate
      // return only the length in buffer
      *(uint16_t *)sz_string = u16_size;
      return true;
   }

   //** Get string ASCII
   u16_size = reader_id3_read_string( sz_string, u8_size_max, 0 );
   // In case of no NULL terminator, the size may be too long
   if((u16_size > size)
   && (u8_size_max > size) )
   {
       // Set a NULL terminator to cut string at the end of field
       if( Is_unicode )
       {
          ((FS_STR_UNICODE)sz_string)[size] = 0;
       }else{
          sz_string[size] = 0;
       }
   }
   return true;
}


//! @brief Return a track from ID3 V1
//!
static bool reader_id3_v1_track( FS_STRING sz_track , uint8_t u8_size_max  )
{
   uint8_t track = 0;
   if( g_b_string_length )
   {
      // return only the length in buffer
      *(uint16_t *)sz_track = 4;  // + null terminate
      return true;
   }
   // read track
   file_seek( 3 , FS_SEEK_END );
   if( 0 == file_getc())
   {
      // version 1.10, then read track
      track = file_getc();
   }  // else version 1.00, no track

   if( 3 < u8_size_max ) // Check size of buffer
   {
      sprintf( (char*) sz_track , "%u", track );
   }
   return true;
}


static _CONST_TYPE_ uint8_t tab_genre_id3_v1[126][18]={
   {"Blues"},
   {"Classic"},
   {"Country"},
   {"Dance"},
   {"Disco"},
   {"Funk"},
   {"Grunge"},
   {"Hip-Hop"},
   {"Jazz"},
   {"Metal"},
   {"New"},
   {"Oldies"},
   {"Other"},
   {"Pop"},
   {"R&B"},
   {"Rap"},
   {"Reggae"},
   {"Rock"},
   {"Techno"},
   {"Industrial"},
   {"Alternative"},
   {"Ska"},
   {"Death"},
   {"Pranks"},
   {"Soundtrack"},
   {"Euro-Techno"},
   {"Ambient"},
   {"Trip-Hop"},
   {"Vocal"},
   {"Jazz+Funk"},
   {"Fusion"},
   {"Trance"},
   {"Classical"},
   {"Instrumental"},
   {"Acid"},
   {"House"},
   {"Game"},
   {"Sound"},
   {"Gospel"},
   {"Noise"},
   {"AlternRock"},
   {"Bass"},
   {"Soul"},
   {"Punk"},
   {"Space"},
   {"Meditative"},
   {"Instrumental"},
   {"Instrumental"},
   {"Ethnic"},
   {"Gothic"},
   {"Darkwave"},
   {"Techno-Industrial"},
   {"Electronic"},
   {"Pop-Folk"},
   {"Eurodance"},
   {"Dream"},
   {"Southern"},
   {"Comedy"},
   {"Cult"},
   {"Gangsta"},
   {"Top"},
   {"Christian"},
   {"Pop/Funk"},
   {"Jungle"},
   {"Native"},
   {"Cabaret"},
   {"New"},
   {"Psychadelic"},
   {"Rave"},
   {"Showtunes"},
   {"Trailer"},
   {"Lo-Fi"},
   {"Tribal"},
   {"Acid"},
   {"Acid"},
   {"Polka"},
   {"Retro"},
   {"Musical"},
   {"Rock"},
   {"Hard"},
   {"Folk"},
   {"Folk-Rock"},
   {"National"},
   {"Swing"},
   {"Fast"},
   {"Bebob"},
   {"Latin"},
   {"Revival"},
   {"Celtic"},
   {"Bluegrass"},
   {"Avantgarde"},
   {"Gothic"},
   {"Progressive"},
   {"Psychedelic"},
   {"Symphonic"},
   {"Slow"},
   {"Big"},
   {"Chorus"},
   {"Easy"},
   {"Acoustic"},
   {"Humour"},
   {"Speech"},
   {"Chanson"},
   {"Opera"},
   {"Chamber"},
   {"Sonata"},
   {"Symphony"},
   {"Booty"},
   {"Primus"},
   {"Porn"},
   {"Satire"},
   {"Slow"},
   {"Club"},
   {"Tango"},
   {"Samba"},
   {"Folklore"},
   {"Ballad"},
   {"Power"},
   {"Rhythmic"},
   {"Freestyle"},
   {"Duet"},
   {"Punk"},
   {"Drum"},
   {"Acapella"},
   {"Euro-House"},
   {"Dance"},
};


//! @brief Return genre from ID3 V1
//!
static bool reader_id3_v1_genre( FS_STRING sz_genre , uint8_t u8_size_max  )
{
   if( g_b_string_length )
   {
      // return only the length in buffer
      *(uint16_t *)sz_genre = 18+1;  // + null terminate
      return true;
   }
   if( 19 > u8_size_max )
      return false;
   // read genre
   file_seek( 1 , FS_SEEK_END );
   memcpy( sz_genre, tab_genre_id3_v1[ file_getc() ], 18 );
   return true;
}


//-------------------------------------------
//------------- ID3 version 2 ---------------
//-------------------------------------------

//! @brief Return title from ID3 V2
//!
static bool reader_id3_v2_title( St_id3ver ver, FS_STRING sz_title , uint8_t u8_size_max  )
{
   return reader_id3_v2_string( ver, ( 2 == ver.ver10 ) ? "TT2" : "TIT2", sz_title , u8_size_max  );
}

//! @brief Return artist from ID3 V2
//!
static bool reader_id3_v2_artist( St_id3ver ver, FS_STRING sz_artist , uint8_t u8_size_max  )
{
   return reader_id3_v2_string( ver, ( 2 == ver.ver10 ) ? "TP1" : "TPE1", sz_artist , u8_size_max  );
}

//! @brief Return album from ID3 V2
//!
static bool reader_id3_v2_album( St_id3ver ver, FS_STRING sz_album , uint8_t u8_size_max  )
{
   return reader_id3_v2_string( ver, ( 2 == ver.ver10 ) ? "TAL" : "TALB", sz_album , u8_size_max  );
}

//! @brief Return year from ID3 V2
//!
static bool reader_id3_v2_year( St_id3ver ver, FS_STRING sz_year , uint8_t u8_size_max )
{
   return reader_id3_v2_string( ver, ( 2 == ver.ver10 ) ? "TYR" : "TYER", sz_year , u8_size_max  );
}

//! @brief Return track from ID3 V2
//!
static bool reader_id3_v2_track( St_id3ver ver, FS_STRING sz_track , uint8_t u8_size_max  )
{
   return reader_id3_v2_string( ver, ( 2 == ver.ver10 ) ? "TRK" : "TRCK", sz_track , u8_size_max  );
}

//! @brief Return genre from ID3 V2
//!
static bool reader_id3_v2_genre( St_id3ver ver, FS_STRING sz_genre , uint8_t u8_size_max  )
{
   return reader_id3_v2_string( ver, ( 2 == ver.ver10 ) ? "TCO" : "TCON", sz_genre , u8_size_max  );
}

//! @brief Get attached picture from ID3 V2
//!
static bool reader_id3_v2_picture(St_id3ver ver, FS_STRING mime_type,
   uint8_t u8_size_max, uint8_t*  picture_type, uint32_t *offset)
{
   uint16_t u16_size;
   bool need_to_swap_bytes;
   uint8_t  format;
   uint8_t b1, b2;

   uint32_t u32_size_tag = reader_id3_v2_findtag( ver,
      (2 == ver.ver10) ? "PIC" : "APIC");

   if( 0 == u32_size_tag )
     return false;  // Tag no found

/*
     $00   ISO-8859-1 [ISO-8859-1]. Terminated with $00.
     $01   UTF-16 [UTF-16] encoded Unicode [UNICODE] with BOM.
     $02   UTF-16BE [UTF-16] encoded Unicode [UNICODE] without BOM.
     $03   UTF-8 [UTF-8] encoded Unicode [UNICODE].
*/
   format = file_getc();
   need_to_swap_bytes = false;
   switch( format )
   {
      case 1:  // UTF-16
      case 2:  // UTF-16BE
      b1 = file_getc();
      b2 = file_getc();
      u32_size_tag-=(1+2); // don't store the previous byte and unicode header
      u32_size_tag /= 2;   // number of char (unicode = 2B)
      if (b1 == 0xFE && b2 == 0xFF)
        need_to_swap_bytes = true;
      break;

      case 0:  // ASCII
      case 3:  // UTF-8
      default:
      u32_size_tag--;      // don't store the previous byte
      break;
   }
   // Here u32_size_tag = size of string (unit char format),
   // except UTF8 because unknown size but a null terminator must be include in TAG

   // Let's start by reading the mime_type
   if( g_b_string_length )
   {
      u16_size = reader_id3_read_string( NULL, 0, format );
      // In case of no NULL terminator, the size may be too long
      if( u16_size > u32_size_tag )
        u16_size = u32_size_tag+1;   // Max size of field + null terminate
      // return only the length in buffer
      *(uint16_t *)mime_type = u16_size;
   }
   else
   {
      u16_size = reader_id3_read_string( mime_type, u8_size_max, format );
      // In case of no NULL terminator, the size may be too long
      if((u16_size > u32_size_tag)
      && (u8_size_max > u32_size_tag) )
      {
         // Set a NULL terminator to cut string at the end of field
         if( Is_unicode )
         {
            ((FS_STR_UNICODE)mime_type)[u32_size_tag] = 0;
         }else{
            mime_type[u32_size_tag] = 0;
         }
      }

      if (need_to_swap_bytes && Is_unicode)
      {
         size_t i;
         for (i=0; i<u8_size_max-1; i+=2)
         {
            b1 = mime_type[i];
            b2 = mime_type[i+1];
            mime_type[i] = b2;
            mime_type[i+1] = b1;
         }
      }
      u32_size_tag -= u16_size;
   }
   // read picture type
   // e.g. 0x03 for front cover, more information is available in the ID3 spec
   *picture_type = file_getc();

   // Read image description. We just ignore this here but we need to read to
   // until the end of the string to get the start of the image data.
   u16_size = reader_id3_read_string(NULL, 0, format);

   // now we reached the start of the image data
   *offset = file_getpos();

   return true;

}
//! @brief Return a string corresponding at TAG V2
//!
static bool reader_id3_v2_string( St_id3ver ver, char* tag, FS_STRING sz_string , uint8_t u8_size_max  )
{
   uint16_t u16_size;
   bool need_to_swap_bytes;
   uint8_t  format;
   uint8_t b1, b2;

   uint32_t u32_size_tag = reader_id3_v2_findtag( ver, tag );
   if( 0 == u32_size_tag )
     return false;  // Tag no found
/*
     $00   ISO-8859-1 [ISO-8859-1]. Terminated with $00.
     $01   UTF-16 [UTF-16] encoded Unicode [UNICODE] with BOM.
     $02   UTF-16BE [UTF-16] encoded Unicode [UNICODE] without BOM.
     $03   UTF-8 [UTF-8] encoded Unicode [UNICODE].
*/
   format = file_getc();
   need_to_swap_bytes = false;
   switch( format )
   {
      case 1:  // UTF-16
      case 2:  // UTF-16BE
      b1 = file_getc();
      b2 = file_getc();
      u32_size_tag-=(1+2); // don't store the previous byte and unicode header
      u32_size_tag /= 2;   // number of char (unicode = 2B)
      if (b1 == 0xFE && b2 == 0xFF)
        need_to_swap_bytes = true;
      break;

      case 0:  // ASCII
      case 3:  // UTF-8
      default:
      u32_size_tag--;      // don't store the previous byte
      break;
   }
   // Here u32_size_tag = size of string (unit char format),
   // execept UTF8 because unknown size but a null terminator must be include in TAG

   if( g_b_string_length )
   {
      u16_size = reader_id3_read_string( NULL, 0, format );
      // In case of no NULL terminator, the size may be too long
      if( u16_size > u32_size_tag )
        u16_size = u32_size_tag+1;   // Max size of field + null terminate
      // return only the length in buffer
      *(uint16_t *)sz_string = u16_size;
   }
   else
   {
      u16_size = reader_id3_read_string( sz_string, u8_size_max, format );
      // In case of no NULL terminator, the size may be too long
      if((u16_size > u32_size_tag)
      && (u8_size_max > u32_size_tag) )
      {
         // Set a NULL terminator to cut string at the end of field
         if( Is_unicode )
         {
            ((FS_STR_UNICODE)sz_string)[u32_size_tag] = 0;
         }else{
            sz_string[u32_size_tag] = 0;
         }
      }

      if (need_to_swap_bytes && Is_unicode)
      {
         size_t i;
         for (i=0; i<u8_size_max-1; i+=2)
         {
            b1 = sz_string[i];
            b2 = sz_string[i+1];
            sz_string[i] = b2;
            sz_string[i+1] = b1;
   }
      }
   }

   return true;
}


//! @brief Return duration from ID3 V2
//!
static bool reader_id3_v2_duration( St_id3ver ver, FS_STRING sz_duration , uint8_t u8_size_max )
{
   return reader_id3_v2_string( ver, ( 2 == ver.ver10 ) ? "TLE" : "TLEN", sz_duration , u8_size_max  );
}


//! Get informations of ID3 version 2
//!
static uint32_t   reader_id3_v2_findtag( St_id3ver version, char *tag )
{
   uint32_t u32_size_tag;
   uint32_t u32_i;
   uint32_t u32_size_frame;
   char idframe[5];
   bool b_extendedheader;

   // Read "Extended headers" option
   file_seek( 5 , FS_SEEK_SET );
   b_extendedheader = (0x40 & file_getc());

   // read tag size
   MSB0(u32_size_tag) = file_getc();
   MSB1(u32_size_tag) = file_getc();
   MSB2(u32_size_tag) = file_getc();
   MSB3(u32_size_tag) = file_getc();
   u32_size_tag   = ((uint32_t)MSB0(u32_size_tag)<<(24-3))
                  + ((uint32_t)MSB1(u32_size_tag)<<(16-2))
                  + ((uint32_t)MSB2(u32_size_tag)<<( 8-1))
                  + MSB3(u32_size_tag);

   // jump the extended header
   if( b_extendedheader )
   {
      if( 3 == version.ver10 )
      {
         // always 10B on version 2.3
         file_seek( 10 , FS_SEEK_CUR_FW );
      }
      if( 4 == version.ver10 )
      {
         // read size for version 2.4
         MSB0(u32_size_frame) = file_getc();
         MSB1(u32_size_frame) = file_getc();
         MSB2(u32_size_frame) = file_getc();
         MSB3(u32_size_frame) = file_getc();
         file_seek( u32_size_frame-4 , FS_SEEK_CUR_FW );
      }
   }

   u32_i = 0;
   while( u32_i < u32_size_tag )
   {
      // Read frame ID
      idframe[0] = file_getc();
      idframe[1] = file_getc();
      idframe[2] = file_getc();
      if( 2 != version.ver10 )
      {
         idframe[3]           = file_getc();
         // Read size frame
         MSB0(u32_size_frame) = file_getc();
      }else{
         idframe[3]           = '\0';
         MSB0(u32_size_frame) = 0;
      }
      idframe[4]           = '\0';
      MSB1(u32_size_frame) = file_getc();
      MSB2(u32_size_frame) = file_getc();
      MSB3(u32_size_frame) = file_getc();

      if( '\0' == idframe[0])
         break; // end of frame

      if( 2 != version.ver10 )
      {
         // ignore flag
         file_getc();
         file_getc();
         // Update position in tag
         u32_i += 4+4+2+u32_size_frame;
      }else{
         u32_i += 3+3+u32_size_frame;
      }

      if( 0 == strcmp( idframe, tag ))
         return u32_size_frame;  // TAG find
      file_seek( u32_size_frame , FS_SEEK_CUR_FW );
   }
   return 0;   // Error
}
