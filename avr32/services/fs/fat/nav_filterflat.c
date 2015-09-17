/*****************************************************************************
 *
 * \file
 *
 * \brief FAT 12/16/32 Services.
 *
 * This plug-in allow a navigation in filter and flat mode
 * This plug-in is connected on the navigation module.
 * The filter mode includes in the file list directories, sub directories and only files and sub files corresponding at a extension
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

//_____  I N C L U D E S ___________________________________________________
#include "conf_explorer.h"
#include "navigation.h"
#include "nav_filterlist.h"
#include "file.h"
#include LIB_CTRLACCESS


//_____ D E C L A R A T I O N S ____________________________________________

//**********************************************************************
//****************** File list navigation FLAT functions ***************


//! This function mounts the selected partition
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
//! @verbatim
//! If the FS_MULTI_PARTITION option is disabled
//! then the mount routine selects the first partition supported by file system. <br>
//! After mount, the file list contains files and directories of ROOT directory
//! @endverbatim
//!
bool  nav_filterflat_mount( void )
{
   fs_g_nav.u8_flat_dir_level = 0;
   fs_g_nav.u16_flat_pos_offset = 0;
   return nav_filterlist_mount();
}


//! This function initializes the file filtered list on the root directory
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
bool  nav_filterflat_root( void )
{
   fs_g_nav.u8_flat_dir_level = 0;
   fs_g_nav.u16_flat_pos_offset = 0;
   return nav_filterlist_root();
}


//! This function resets the pointer of selection, so "no file selected" in filtered file list
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
bool  nav_filterflat_reset( void )
{
   while( 0 != fs_g_nav.u8_flat_dir_level )
   {
      fs_g_nav.u8_flat_dir_level--;
      nav_dir_gotoparent();
   }
   fs_g_nav.u16_flat_pos_offset = 0;
   return nav_filterlist_reset();
}


//! This function goes to the next position in the filtered file list
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
bool  nav_filterflat_next( void )
{
   uint16_t u16_save_current_pos;

   u16_save_current_pos = nav_filterlist_get();
   if( nav_file_isdir() )
   {
      // The current file is a dir then enter in this
      if( !nav_dir_cd() )
         return false;
      if( nav_filterlist_next() )
      {
         // File present in this dir then valid the new position
         fs_g_nav.u8_flat_dir_level++;
         fs_g_nav.u16_flat_pos_offset += u16_save_current_pos+1;
         return true;
      }else{
         // No file then return in parent dir
         if( !nav_filterlist_gotoparent() )
            return false;
      }
   }
   // Find next file in current dir or parent dir
   while( !nav_filterlist_next() )
   {
      // End of current directory then goes to parent
      if( 0 == fs_g_nav.u8_flat_dir_level )
         return false;  // End of list FLAT
      if( !nav_filterlist_gotoparent() )
         return false;
      fs_g_nav.u8_flat_dir_level--;
   }
   fs_g_nav.u16_flat_pos_offset = (fs_g_nav.u16_flat_pos_offset +u16_save_current_pos +1) -nav_filterlist_get();
   return true;
}


//! This function goes to the previous position in filtered file list
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
bool  nav_filterflat_previous( void )
{
   uint16_t u16_save_current_pos;

   u16_save_current_pos = nav_filterlist_get();
   if( nav_filterlist_previous() )
   {
      while( 1 )
      {
         if( !nav_file_isdir() )
         {
            fs_g_nav.u16_flat_pos_offset = ((fs_g_nav.u16_flat_pos_offset +u16_save_current_pos) -nav_filterlist_get()) -1;
            return true;   // New position valid
         }
         // The file is a dir then enter in this
         if( !nav_dir_cd() )
            return false;
         if( !nav_filterlist_next() )
         {
            // Dir empty then goes to parent and dir is the new selection
            nav_filterlist_gotoparent();
            fs_g_nav.u16_flat_pos_offset = ((fs_g_nav.u16_flat_pos_offset +u16_save_current_pos) -nav_filterlist_get()) -1;
            return true;
         }
         fs_g_nav.u8_flat_dir_level++;
         // Go to end of the dir
         while( nav_filterlist_next() );
      }
   }
   // Beginning of current directory then goes to parent
   if( 0 == fs_g_nav.u8_flat_dir_level )
      return false;  // beginning of list FLAT
   if( !nav_filterlist_gotoparent() )
      return false;
   fs_g_nav.u8_flat_dir_level--;
   fs_g_nav.u16_flat_pos_offset -= (nav_filterlist_get()+1);
   return true;
}


//! This function returns the position of selected file in filtered file list
//!
//! @return    position of selected file in directory (0 is the first position)
//! @return    FS_NO_SEL, in case of no file selected
//!
//!
uint16_t   nav_filterflat_get( void )
{
   return (fs_g_nav.u16_flat_pos_offset + nav_filterlist_get());
}


//! This function goes to a position in filtered file list
//!
//! @param     u16_newpos     new position to select (0 is the first position)
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool  nav_filterflat_goto( uint16_t u16_newpos )
{
   if (u16_newpos < (nav_filterflat_get()/2))
   {
      // Restart at the beginning of list to accelerate search
      if (!nav_filterflat_reset())
         return false;
   }
   if( FS_NO_SEL == nav_filterflat_get() )
      if( !nav_filterflat_next() )
         return false;  // No file available
   while( u16_newpos > nav_filterflat_get())
   {
      if( !nav_filterflat_next())
         return false; // End of list, position is bad
   }
   while(u16_newpos < nav_filterflat_get())
   {
      if( !nav_filterflat_previous())
         return false; // Position error
   }
   return true;
}


//! This function searches a file name in file list filtered
//!
//! @param     sz_name        name to search (UNICODE or ASCII) <br>
//!                           The name must end with NULL or '*' value
//! @param     b_match_case   false to ignore the case
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
//! @verbatim
//! This function starts a search at the next position of the current in filtered file list
//! @endverbatim
//!
bool  nav_filterflat_findname( const FS_STRING sz_name , bool b_match_case )
{
   while( 1 )
   {
      if ( !nav_filterflat_next() )
         return false;
      if ( nav_file_name( sz_name , 0 , FS_NAME_CHECK , b_match_case ))
         return true;
   }
}


//! This function computes the number of files and directories in filtered file list
//!
//! @return    number of files and directories present in filtered file list
//!
uint16_t   nav_filterflat_nb( void )
{
   uint16_t   u16_save_position;
   uint16_t   u16_number;

   // Save current position
   u16_save_position = nav_filterflat_get();

   // Reset position
   if ( !nav_filterflat_reset())
      return 0;

   // Scan all directory
   u16_number  = 0;
   while( nav_filterflat_next() )
   {
      u16_number++;
   }
   // Restore previous position
   nav_filterflat_goto( u16_save_position );

   return u16_number;
}


//! This function enters in the selected directory
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
bool  nav_filterflat_cd( void )
{
   if( !nav_dir_cd() )
      return false;
   fs_g_nav.u8_flat_dir_level   = 0;
   fs_g_nav.u16_flat_pos_offset = 0;
   return true;
}


//! This function goes to at the parent directory
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
//! @verbatim
//! After the selected file is the first entry
//! @endverbatim
//!
bool  nav_filterflat_gotoparent( void )
{
   _MEM_TYPE_SLOW_ Fs_index index;
   index = nav_getindex();
   while( 0 != fs_g_nav.u8_flat_dir_level )
   {
      fs_g_nav.u8_flat_dir_level--;
      nav_dir_gotoparent();
   }
   if( !nav_dir_gotoparent() )
   {
      nav_gotoindex( &index );
      return false;
   }
   // go to the beginning of FLAT list
   fs_g_nav.u8_flat_dir_level   = 0;
   fs_g_nav.u16_flat_pos_offset = 0;
   nav_filterlist_reset();
   nav_filterflat_next();
   return true;
}

