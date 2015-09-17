/*****************************************************************************
 *
 * \file
 *
 * \brief Play list Services.
 *
 * This plug-in allow to open and manage a play list file (*.txt,*.m3u,*.m3u8,...) included in text file
 * This plug-in is connected on the reader_txt plug-in and the navigation module.
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

#include <string.h>
#include "conf_explorer.h"
#include "navigation.h"
#include "file.h"
#include "reader_txt.h"
#include "play_list.h"


// Check configuration
#ifndef PL_READONLY
#  define PL_READONLY   true
#endif

#if( PL_READONLY == false )
#   error the modification of play list file is not available
#endif

#if defined(PLAYLIST_USE_NON_BLOCKING_FUNCTION_SET) && \
    PLAYLIST_USE_NON_BLOCKING_FUNCTION_SET == true
#include "audio_interface.h"
#if !defined(AI_ASYNC_STACK_NB_ELEMENTS) || AI_ASYNC_STACK_NB_ELEMENTS < 1
#error This version of the play_list.c file needs a context that can support \
at least 1 element. Therefore, AI_ASYNC_STACK_NB_ELEMENTS must be defined with \
a value greater or equals to 1.
#endif
#endif


//_____ D E F I N I T I O N S ______________________________________________

//! Type of playlist file
typedef enum {
   E_PL_M3U,
   E_PL_PLS,
   E_PL_SMP
} E_pl_type;


                  bool        pl_g_list_is_open = false; //!< play list file is opened
_MEM_TYPE_SLOW_   E_pl_type   pl_g_list_type;            //!< Type of the play list
_MEM_TYPE_SLOW_   uint16_t         pl_g_u16_list_size    = 0; //!< Number of file included in play list
_MEM_TYPE_SLOW_   uint16_t         pl_g_u16_list_sel     = 0; //!< Current file selected in play list (0= no file selected, 1= first file)

#if( PL_READONLY == false )
                  bool        pl_g_list_is_modify;       //!< If true then the current play list file is modified but it is not saved
                  bool        pl_g_list_is_readonly;     //!< If false then the play list modification is authorized
                  bool        pl_g_list_undo;            //!< If true then authorize to copy the play list file in case of an undo on modifications.
_MEM_TYPE_SLOW_   Fs_index    copyfile_index;            //!< File system index on the copy file of play list file (undo option must be ENABLE)
#endif


//_____ D E C L A R A T I O N S ____________________________________________

bool  pl_main_modify       ( void );


//********************* MAIN functions *********************


//! This function checks if a play list file is already opened
//!
//! @return true     a play list file is opened
//!
bool   pl_main_isopen( void )
{
   fs_g_status = FS_ERR_PL_NOT_OPEN;
   return pl_g_list_is_open;
}


//! This function checks if the play list file is modified
//!
//! @return true     the play list file is modified
//!
bool  pl_main_ismodify( void )
{
#if( PL_READONLY == false )
   return pl_g_list_is_modify;
#else
   return false;
#endif
}


#if( PL_READONLY == false )
//! This function creates a new empty play list file
//!
//! @param     sz_name        contains the file name (ASCII or UNICODE )
//! @param     b_undo         true to authorize the undo feature
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool  pl_main_new( const FS_STRING sz_name , bool b_undo )
{
   if( pl_main_isopen() )
      return false;

   // Create an empty text file
   if( !reader_txt_new( sz_name, UNI_TYPE_UTF16BE ))
      return false;
   reader_txt_close();

   // Load the play list file
   return pl_main_open( b_undo );
}
#endif


//! This function opens the current selected file in the current navigator
//!
//! @param     b_undo         true to authorize the undo feature
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool   pl_main_open( bool b_undo )
{
   uint8_t nav_id_save;

   if( pl_main_isopen() )
      return false;

   // Select the play list file in "play list" navigator
   nav_id_save = nav_get();            // Save current navigator
   nav_copy( FS_NAV_ID_PLAYLIST );
   nav_select( FS_NAV_ID_PLAYLIST );

   // Make sure the selected file is a playlist
   if( nav_file_checkext("m3u") )
   {
      pl_g_list_type = E_PL_M3U;
   }else
   if( nav_file_checkext("pls") )
   {
      pl_g_list_type = E_PL_PLS;
   }else
   if( nav_file_checkext("smp") )
   {
      pl_g_list_type = E_PL_SMP;
   }else
      goto pl_main_open_end;

   // Open list
#if( PL_READONLY == false )
   pl_g_list_undo          = b_undo;
   pl_g_list_is_modify     = false;
   pl_g_list_is_readonly   = nav_file_isreadonly();
   if( pl_g_list_is_readonly )
   {
      if( !reader_txt_open( true ))
         goto pl_main_open_end;
   }else{
      if( !reader_txt_open( false ))
         goto pl_main_open_end;
   }
#else
    if( !reader_txt_open( true ))
       goto pl_main_open_end;
#endif

   // Compute the number of file in list
   pl_g_u16_list_size    = 0;
   while( 0 == file_eof() )
   {
      if( pl_main_readline(PL_MAIN_READLINE_OPT_CHECKLINE,0,NULL,NULL) )
         pl_g_u16_list_size++;   // Add a file
   }

   // Go to at the beginning of text file
   reader_txt_beg();

   pl_g_u16_list_sel    = 0;     // Don't select a file
   pl_g_list_is_open    = true;  // Play list file is opened
   nav_select( nav_id_save );    // Re select the previous navigator
   nav_filelist_reset();

pl_main_open_end:
   nav_select( nav_id_save );    // Re select the previous navigator
   return pl_g_list_is_open;
}


//! This function saves the play list file
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
#if( PL_READONLY == false )
bool   pl_main_save( void )
{
   _MEM_TYPE_SLOW_ Fs_index index;
   uint8_t nav_id_save;
   uint16_t u16_pos;

   // Check if the play list file is opened and modified
   if( !pl_main_isopen() )
      return false;
   if( !pl_g_list_is_modify )
      return true;
   pl_g_list_is_modify = false;

   // Check if a undo file exists
   if( !pl_g_list_undo )
      return true;

   // Remove original copy of play list file
   nav_id_save = nav_get();
   nav_select( FS_NAV_ID_PLAYLIST );
   u16_pos = pl_g_u16_list_sel;           // Save position in play list
   file_close();                          // Close list
   index = nav_getindex();                // Save the pointer on play list file
   pl_g_list_is_open = false;
   if( nav_gotoindex( &copyfile_index ) ) // Go to the copy of file
      nav_file_del( true );               // Remove the copy
   nav_gotoindex( &index );               // Re select play list file
   pl_main_open( true );                  // Re open play list file
   pl_nav_setpos(u16_pos);                // Restore position in play list
   nav_select( nav_id_save );

   return true;
}
#endif


//! This function closes the play list and selects this one in current navigator
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool   pl_main_close( void )
{
   _MEM_TYPE_SLOW_ Fs_index index;
   uint8_t nav_id_save;

   if( !pl_main_isopen() )
      return false;

   // Close play list
   nav_id_save = nav_get();
   nav_select( FS_NAV_ID_PLAYLIST );
   file_close();

#if( PL_READONLY == false )
   if( pl_g_list_is_modify )
   {
      if( pl_g_list_undo )
      {
         uint16_t size_name;
         FS_STRING sz_name;

         //** Play list modified but not saved, then restore previous list
         // Get name of play list
         nav_string_length_enable();
         nav_file_name( (FS_STRING)&size_name, 1, FS_NAME_GET, false );
         nav_string_length_disable();
         sz_name = PLAYLIST_BUF_ALLOC( size_name* (Is_unicode? 2 : 1 ) );
         if( NULL != sz_name )
         {
            if( nav_file_name( sz_name, size_name, FS_NAME_GET, false ) )
            {
               // Delete current play list
               if( nav_file_del( true ) )             // Remove copy file
               {
                  nav_gotoindex( &copyfile_index );   // Select copy play list
                  nav_file_rename( sz_name );         // rename copy play list
               }
            }
            PLAYLIST_BUF_FREE( sz_name );
         }
      }
   }
#endif

   // Select the play list file in current navigator
   index = nav_getindex();       // Save a pointer on play list file
   nav_select( nav_id_save );
   nav_gotoindex( &index );
   pl_g_list_is_open = false;
   return true;
}


#if( PL_READONLY == false )
//! This function creates a copy of the play list in case of a restore action
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool   pl_main_modify( void )
{
   _MEM_TYPE_SLOW_ uint8_t name_copyfile[]="~copy.m3u";
   _MEM_TYPE_SLOW_ Fs_index index;
   uint8_t nav_id_save;
   uint16_t u16_pos;
   uint8_t status;
   bool b_copy_finish = false;

   if( !pl_main_isopen() )
      return false;
   if( pl_g_list_is_modify )
      return true;
   // If an error occurs during copy process then the play list file is set in read only mode
   fs_g_status = FS_ERR_PL_READ_ONLY;
   if( pl_g_list_is_readonly )
      return false;
   if( !pl_g_list_undo )
   {
      pl_g_list_is_modify = true;
      return true;
   }

   // Save information about current play list file
   nav_id_save = nav_get();
   nav_select( FS_NAV_ID_PLAYLIST );
   u16_pos = pl_g_u16_list_sel;           // Save position in play list
   file_close();                          // Close list
   pl_g_list_is_open = false;
   index = nav_getindex();                // Get pointer on play list

   // Copy play list file in a temporary file
#if( (FS_ASCII == true) && (FS_UNICODE == true) )
   nav_string_ascii();                    // copy file name stored in ASCII
#endif
   if( !nav_file_copy())
      goto pl_main_modify_end;
   if( !nav_file_paste_start( name_copyfile ) )
   {
      if( FS_ERR_FILE_EXIST != fs_g_status)
         goto pl_main_modify_end;
      // File exist then delete
      if( !nav_file_del(true) )
         goto pl_main_modify_end;
      // Retry paste
      if( !nav_file_paste_start( name_copyfile ) )
         goto pl_main_modify_end;
   }
   // Run paste
   do{
      status = nav_file_paste_state(false);  // Copy running
   }while( COPY_BUSY == status );
   if( COPY_FINISH != status )
      goto pl_main_modify_end;
   // Save pointer on copy file
   copyfile_index = nav_getindex();
   b_copy_finish = true;

pl_main_modify_end:
   // Restore the play list file
#if( (FS_ASCII == true) && (FS_UNICODE == true) )
   nav_string_unicode();
#endif
   nav_gotoindex( &index );               // Go to original play list file
   pl_main_open( true );                  // Re open play list file
   pl_g_list_is_modify = b_copy_finish;
   nav_select( nav_id_save );
   pl_nav_setpos(u16_pos);                // Restore position in play list
   return pl_g_list_is_modify;
}
#endif


//********************* Play list navigation functions *********************


//! Return the number of files in play list
//!
//! @return number of files in play list
//!
uint16_t pl_nav_getnbfile( void )
{
  return pl_g_u16_list_size;
}


//! Return the position in the play list
//!
//! @return position in the play list
//!
uint16_t pl_nav_getpos( void )
{
   return pl_g_u16_list_sel;
}

//! This function checks the end of the play list
//!
//! @return true if the position is the last file of the play list
//!
bool pl_nav_eol( void )
{
   return (pl_g_u16_list_size == pl_g_u16_list_sel);
}

//! This function goes to a position in the play list
//!
//! @param u16_position position in play list to go
//!
bool  pl_nav_setpos( uint16_t u16_position )
{
   uint8_t nav_id_save;

   if( (0 == u16_position )
   ||  (pl_g_u16_list_size < u16_position) )
   {
      fs_g_status = FS_ERR_PL_OUT_LST;
      return false;
   }

   if( pl_g_u16_list_sel == u16_position )
      return true;

   // Reset previous selection
   nav_filelist_reset();

   // Go to play list file navigator
   nav_id_save = nav_get();
   nav_select( FS_NAV_ID_PLAYLIST );

   if( pl_g_u16_list_sel > u16_position )
   {
      // Go to at the beginning of text file
      reader_txt_beg();
      pl_g_u16_list_sel    = 0;
   }
   while( pl_g_u16_list_sel != u16_position )
   {
      FS_STRING sz_path;
      bool next_pos = false;

      if( 0 != file_eof() )
      {
         nav_select( nav_id_save ); // Re select the previous navigator
         return false;
      }
      if (pl_g_u16_list_sel+1 == u16_position)
      {
        next_pos = pl_main_readline(PL_MAIN_READLINE_OPT_GOTOPATH, nav_id_save, &sz_path, NULL);
        if (sz_path)
          PLAYLIST_BUF_FREE(sz_path);
      }
      else
        next_pos = pl_main_readline(PL_MAIN_READLINE_OPT_CHECKLINE, 0, NULL, NULL);
      if (next_pos)
         pl_g_u16_list_sel++; // Add a file
   }
   nav_select( nav_id_save ); // Re select the previous navigator
   return true;
}

//! This function read the playlist's entry at a specific position
//!
//! @param u16_position Position in play list.
//! @param sz_path      The address of the string where is stored the information.
//! @param u16_size_line  The length of the path.
//!
bool pl_nav_readentry(uint16_t u16_position, FS_STRING *sz_path, uint16_t *u16_size_line )
{
  uint8_t nav_id_save;
  uint16_t u16_current_pos;
  uint32_t u32_file_pos;

  // Set output to NULL to specify if there is an error
  *sz_path = NULL;

  // Position out of range
  if ((0 == u16_position)
   || (pl_g_u16_list_size < u16_position))
  {
     fs_g_status = FS_ERR_PL_OUT_LST;
     return false;
  }

  // Go to play list file navigator
  nav_id_save = nav_get();
  nav_select(FS_NAV_ID_PLAYLIST);

  // Save current file position
  u32_file_pos = file_getpos();

  if (pl_g_u16_list_sel > u16_position)
  {
    // Go to the beginning of the text file
    reader_txt_beg();
    u16_current_pos = 0;
  }
  else
  {
    u16_current_pos = pl_g_u16_list_sel;
  }

  while(u16_current_pos != u16_position)
  {
    if (file_eof())
    {
      // Restore file position
      file_seek(u32_file_pos, FS_SEEK_SET);
      // Reselect the previous navigator
      nav_select(nav_id_save);
      return false;
    }
    if (u16_current_pos+1 == u16_position)
    {
      if (pl_main_readline(PL_MAIN_READLINE_OPT_GOTOPATH, nav_id_save, sz_path, u16_size_line))
        break;
    }
    else if (pl_main_readline(PL_MAIN_READLINE_OPT_CHECKLINE, 0, NULL, NULL))
      u16_current_pos++; // Add a file
  }

  // Restore file position
  file_seek(u32_file_pos, FS_SEEK_SET);
  // Reselect the previous navigator
  nav_select(nav_id_save);

  return true;
}

//! This function reads and checks the next line
//!
//! @param opt         PL_MAIN_READLINE_OPT_CHECKLINE to check the next line in the file.\n
//!                     PL_MAIN_READLINE_OPT_READSTRING to read the string in the text file.\n
//!                     PL_MAIN_READLINE_OPT_GOTOPATH to read, check and goto the path.
//! @param id_nav       ID navigator to update with the selected file (ignore if b_gotopatch == false)
//! @param sz_path      Address of the string returned. It is used only if PL_MAIN_READLINE_OPT_READSTRING or
//!                     PL_MAIN_READLINE_OPT_GOTOPATH options are specified.
//! @param u16_size_line  The length of the path. If this parameter is NULL, then the length is not returned.
//!
//! @return    true  if a line with a correct path
//!            false line ignored or end of file
//!
bool  pl_main_readline( readline_opt_t opt, uint8_t id_nav, FS_STRING *sz_path, uint16_t *u16_size_line )
{
   uint16_t line_beg[8];           // Contains first characters of the line
   uint32_t u32_file_pos;
   uint16_t u16_alt_length = 0;
   uint8_t  u8_pos_path_in_line = 0;

   // Set output to NULL to specify if there is an error
   *sz_path = NULL;

   // Read the 3 first char in the line
   u32_file_pos = file_getpos();
   if( E_PL_M3U == pl_g_list_type )
   {
      // M3U path lines = "path\file"
      // M3U comment lines = "#..."
      u16_alt_length = reader_txt_get_line( true, (FS_STRING)line_beg, 2 );
      if( (2 > u16_alt_length)   // The line size is not correct
      ||  ('#' == line_beg[0]) ) // It is a comment line
      {
         return false;
      }
      u8_pos_path_in_line=0;
   }
   if( (E_PL_PLS == pl_g_list_type) || (E_PL_SMP == pl_g_list_type) )
   {
      // PLS path lines = "Filexxx=path\file"
      // SMP path lines = "File=path\file"
      u16_alt_length = reader_txt_get_line( true, (FS_STRING)line_beg, 8 );
      if(6 > u16_alt_length)     // The line size can't contain a path line
         return false;
      // Check if it is a path line
      if( ('F' != line_beg[0])
      ||  ('i' != line_beg[1])
      ||  ('l' != line_beg[2])
      ||  ('e' != line_beg[3]) )
      {
         return false;
      }
      u8_pos_path_in_line=5;
      if('=' != line_beg[4])
      {
         u8_pos_path_in_line++;
         if('=' != line_beg[5])
         {
            u8_pos_path_in_line++;
            if('=' != line_beg[6])
            {
               u8_pos_path_in_line++;
               if('=' != line_beg[7])
                  return false;
            }
         }
      }
   }
   // Here, the line contains a path

   // Store the length of the line if a buffer is set
   if (u16_size_line)
     *u16_size_line = u16_alt_length - u8_pos_path_in_line;
   if (opt == PL_MAIN_READLINE_OPT_CHECKLINE)
      return true;
   // else go to path

   // Read all characters in line
   file_seek( u32_file_pos, FS_SEEK_SET );
   *sz_path = PLAYLIST_BUF_ALLOC( u16_alt_length * (Is_unicode? 2 : 1 ) );
   if( NULL == *sz_path )
      return false;   // no enough memory, impossible to store the path but path present in line
   if( 0 == reader_txt_get_line( (FS_UNICODE==true), *sz_path, u16_alt_length ) )
      return false;  // Error during the read
   if( 0 != u8_pos_path_in_line )
   {
      // Shift the path to the beginning of the buffer
      memmove(*sz_path, *sz_path + u8_pos_path_in_line * ((FS_UNICODE==true) ? 2 : 1),
              (u16_alt_length - u8_pos_path_in_line) * ((FS_UNICODE==true) ? 2 : 1));
   }
   if (opt == PL_MAIN_READLINE_OPT_READSTRING)
     return true;

   // Update the navigator with the path of play list file, because the path is relative at this one
   nav_copy( id_nav );
   nav_select( id_nav );
   // Go to the path included in the line
   if( !nav_setcwd( (FS_STRING) *sz_path , false, false ) )
   {
      // path no found then path is dead -> reset list to deselect a file
      nav_filelist_reset();
   }
   nav_select( FS_NAV_ID_PLAYLIST );

   return true;
}


//********************* Play list control position *********************


//! This function tests the valid path in list
//!
//! @return false = path is not valid in list - DEAD
//!
bool  pl_sel_isvalid( void )
{
   if(!pl_main_isopen())
      return false;
   if(0 == pl_g_u16_list_sel)
      return false;
   return nav_filelist_validpos();
}


//********************* Play list additional functions *********************


#if( PL_READONLY == false )
//! This function adds files in play list
//!
//! @param sz_filterext add file only corresponding to the  extension filter
//! @param u8_mode      PL_ADD_FILE, PL_ADD_DIR, PL_ADD_SUBDIR
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
//! @verbatim
//! It is possible to select a file or all files in a directory
//! @endverbatim
//!
bool   pl_add( const FS_STRING sz_filterext , uint8_t u8_mode )
{
   uint8_t nav_id_save;
   uint8_t u8_folder_level;

   if( !pl_main_modify() )
      return false;
   nav_id_save = nav_get();

   // Check last character of file
   nav_select( FS_NAV_ID_PLAYLIST );
   if( 0!=nav_file_lgt() )
   {
#if( PL_UNICODE == true)
      file_string_unicode();
      file_seek( 2, FS_SEEK_END);
      if( '\n' != file_getc())
      {
         file_seek( 2, FS_SEEK_END);
         file_putc('\n');
      }
      file_string_ascii();
#else
      file_seek( 1, FS_SEEK_END);
      if( '\n' != file_getc())
      {
         file_seek( 1, FS_SEEK_END);
         file_putc('\n');
      }
#endif
   }
   nav_select( nav_id_save );

   // Get path of play list file and check with current to create a relative path

   if( PL_ADD_FILE == u8_mode )
      goto pl_add_file;

   // Add all files valid in current dir
   u8_folder_level = 0;
   nav_filelist_reset();
   while(1)
   {
      while(1)
      {
         if( nav_filelist_set( 0 , FS_FIND_NEXT ) )
            break;   // a next file and directory is found
         // No other dir or file in current dir then go to parent dir
         if( 0 == u8_folder_level )
            goto pl_add_end;  // end of ADD
         // Remark, nav_dir_gotoparent() routine go to in parent dir and select the children dir in list
         u8_folder_level--;
         if( !nav_dir_gotoparent() )
            return false;
      }

      if( nav_file_isdir())
      {
         if( PL_ADD_SUBDIR == u8_mode )
         {  // Enter in sub dir
            if( !nav_dir_cd())
               return false;
            u8_folder_level++;
         }
      }
      else
      {
pl_add_file:
         if( nav_file_checkext( sz_filterext ) )
         {
            // It is a valid file
            // Get name of current file
#if( (FS_ASCII == true) && (FS_UNICODE == true) && (PL_UNICODE == false) )
            nav_string_ascii();
#endif
            nav_getcwd( (FS_STRING)pl_cache_path, PL_CACHE_PATH_MAX_SIZE, true );
#if( (FS_ASCII == true) && (FS_UNICODE == true) && (PL_UNICODE == false) )
            nav_string_unicode();
#endif
            // Write path in file list
            nav_select( FS_NAV_ID_PLAYLIST );
#if( PL_UNICODE == true)
            file_string_unicode();
#endif
            if( file_puts(pl_cache_path))
               file_putc('\n');
#if( PL_UNICODE == true)
            file_string_ascii();
#endif
            nav_select( nav_id_save );
            pl_g_u16_list_size++;
         }
         if( PL_ADD_FILE == u8_mode )
            goto pl_add_end;
      } // if dir OR file
   } // end of first while(1)

pl_add_end:
   // Go to beginning of file AND no file selected
   nav_select( FS_NAV_ID_PLAYLIST );
   file_seek( 0 , FS_SEEK_SET );
   pl_g_u16_list_sel = 0;
   nav_select( nav_id_save );
   return true;
}
#endif


//********************* Play list remove functions *********************

#if( PL_READONLY == false )

//! This function removes all files in the list
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool   pl_rem_sel_all( void )
{
   uint8_t nav_id_save;
   bool b_status;

   if( !pl_main_modify() )
      return false;

   nav_id_save = nav_get();
   nav_select( FS_NAV_ID_PLAYLIST );
   // Go to beginning of file AND set eof to have a size = 0
   file_seek( 0 , FS_SEEK_SET );
   b_status = file_set_eof();
   nav_select( nav_id_save );
   return b_status;
}


//! This function removes the selected file in the list
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool   pl_rem_sel( void )
{
   uint8_t nav_id_save;
   bool b_status = false;

   if( !pl_main_modify() )
      return false;
   if( 0 == pl_g_u16_list_sel )
   {
      fs_g_status = FS_ERR_PL_OUT_LST;
      return false;
   }
   nav_id_save = nav_get();
   nav_select( FS_NAV_ID_PLAYLIST );

   // Jump at the beginning of the current path
#if( PL_UNICODE == true)
   file_string_unicode();
   if( !file_seek( pl_g_u16_path_size*2 , FS_SEEK_CUR_RE ))
      goto pl_rem_sys_err;
#else
   if( !file_seek( pl_g_u16_path_size , FS_SEEK_CUR_RE ))
      goto pl_rem_sys_err;
#endif

   // Comment the line
   if( !file_putc('#'))
      goto pl_rem_sys_err;
   b_status = true;
   pl_g_u16_list_size--;

pl_rem_sys_err:
#if( PL_UNICODE == true)
    file_string_ascii();
#endif
   // Go to beginning of file AND no file selected
   file_seek( 0 , FS_SEEK_SET );
   pl_g_u16_list_sel = 0;
   nav_select( nav_id_save );
   return b_status;
}


//! This function remove the dead file in the list
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool pl_rem_dead( void )
{
   uint8_t nav_id_save;

   pl_g_u16_list_sel = 0;

   nav_id_save = nav_get();
   nav_select( FS_NAV_ID_PLAYLIST );

   while( pl_g_u16_list_sel < pl_g_u16_list_size )
   {
      if( nav_gotoindex( &pl_g_list[ pl_g_u16_list_sel ] ))
      {
         // the file is good -> go to next file in list
         pl_g_u16_list_sel++;
         continue;
      }

      // The file is dead -> remove this one
      pl_rem_sel();
   }
   nav_select( nav_id_save );
   pl_nav_setpos(0);
}

#endif  // ( PL_READONLY == false )

#if defined(PLAYLIST_USE_NON_BLOCKING_FUNCTION_SET) && \
    PLAYLIST_USE_NON_BLOCKING_FUNCTION_SET == true

//********************* Play list non-blocking function set ********************

//! This function opens the current selected file in the current navigator in read only mode.\n
//! It is a non blocking-function that returns every n-file counted.
//!
//! @param     u16_n         The number of files to look for before returning.\n
//!                          If it contains the special value of 0, then, it will not
//!                          process the count of link inside the playlist.
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool pl_main_open_non_blocking(uint16_t u16_n)
{
  ai_async_context_t ctx;
  uint16_t u16_current_count = 0;
  static uint8_t nav_id_save;
  typedef enum
  {
    STATE_INITIALIZATION = 0,
    STATE_GET_LINK_COUNT,
    STATE_INIT_PLAYLIST_CONTEXT,
    STATE_END
  } state_t;

  // If false, means it is the first call
  if (!ai_async_context_pop(&ctx))
    ctx.state = (state_t) STATE_INITIALIZATION;
  // By default set the status to DONE
  ctx.status = CMD_DONE;

  switch((state_t) ctx.state)
  {
  // Initialization
  case STATE_INITIALIZATION:
    if (pl_main_isopen())
      return false;
    // Select the play list file in "play list" navigator
    // Save current navigator
    nav_id_save = nav_get();
    nav_copy(FS_NAV_ID_PLAYLIST);
    nav_select(FS_NAV_ID_PLAYLIST);
    // Make sure the selected file is a playlist
    if (nav_file_checkext("m3u"))
      pl_g_list_type = E_PL_M3U;
    else if (nav_file_checkext("pls"))
      pl_g_list_type = E_PL_PLS;
    else if (nav_file_checkext("smp"))
      pl_g_list_type = E_PL_SMP;
    else
    {
      ctx.status = CMD_EXECUTING;
      ctx.state = (state_t) STATE_END;
      break;
    }
    // Open list
    if (!reader_txt_open(true))
    {
      ctx.status = CMD_EXECUTING;
      ctx.state = (state_t) STATE_END;
      break;
    }
    pl_g_u16_list_size = 0;
    ctx.status = CMD_EXECUTING;
    if (u16_n)
      ctx.state = (state_t) STATE_GET_LINK_COUNT;
    else
      ctx.state = (state_t) STATE_INIT_PLAYLIST_CONTEXT;
    break;

  case STATE_GET_LINK_COUNT:
    u16_current_count = 0;
    nav_select(FS_NAV_ID_PLAYLIST);
    while (!file_eof() && u16_current_count < u16_n)
    {
      if (pl_main_readline(PL_MAIN_READLINE_OPT_CHECKLINE, 0, NULL, NULL))
        pl_g_u16_list_size++;
      // Check if an error occurred during the reading
      else if (fs_g_status == FS_ERR_FS)
      {
        // Error file system
        ctx.status = CMD_EXECUTING;
        ctx.state = (state_t) STATE_END;
        break;
      }
      u16_current_count++;
    }
    ctx.status = CMD_EXECUTING;
    if (file_eof())
      ctx.state = (state_t) STATE_INIT_PLAYLIST_CONTEXT;
    break;

  case STATE_INIT_PLAYLIST_CONTEXT:
    nav_select(FS_NAV_ID_PLAYLIST);
    // Go to at the beginning of text file
    reader_txt_beg();
    // Don't select a file
    pl_g_u16_list_sel = 0;
    // Play list file is opened
    pl_g_list_is_open = true;
    // Re select the previous navigator
    nav_select(nav_id_save);
    nav_filelist_reset();
    break;

  case STATE_END:
    // Re select the previous navigator
    nav_select(nav_id_save);
    break;
  }

  if (ctx.status == CMD_EXECUTING)
  {
    if (!ai_async_context_push(&ctx))
      return false;
  }

  return pl_g_list_is_open;
}

//! This function read the playlist's entry at a specific position.\n
//! It is implemented as a non-blocking function.
//!
//! @param data         This data has to be set to 0 if non-caching options are
//!                     used.
//! @param u16_position Position in play list.
//! @param sz_path      The address of the string where is stored the information.
//! @param u16_size_line  The length of the path.
//! @param u16_n        The number of line to read at one time
//!
bool pl_nav_readentry_non_blocking(pl_nav_readentry_context_t *data, uint16_t u16_position, FS_STRING *sz_path, uint16_t *u16_size_line, uint16_t u16_n)
{
  ai_async_context_t ctx;
  uint16_t u16_current_n;
  typedef enum
  {
    STATE_INITIALIZATION = 0,
    STATE_SET_POS,
    STATE_READ_ENTRY
  } state_t;
  bool b_update;

  // If false, means it is the first call
  if (!ai_async_context_pop(&ctx))
    ctx.state = (state_t) STATE_INITIALIZATION;
  // By default set the status to DONE
  ctx.status = CMD_DONE;

  switch((state_t) ctx.state)
  {
  // Initialization
  case STATE_INITIALIZATION:
    // Set output to NULL to specify if there is an error
    *sz_path = NULL;
    // Position out of range
    if (u16_position == 0)
    {
      fs_g_status = FS_ERR_PL_OUT_LST;
      return false;
    }
    // Go to play list file navigator
    data->nav_id_saved = nav_get();
    nav_select(FS_NAV_ID_PLAYLIST);
    // Save current file position
    data->u32_file_pos_saved = file_getpos();

    // In this case it means the context data is already filled with entry
    // information for this specific playlist. That is the reason why it is
    // important to fill this structure with zeros if non-caching options are
    // used.
    b_update = true;
    if (data->fs_entry.u8_open_mode)
    {
      if (data->u16_current_pos + 1 <= u16_position)
        b_update = false;
    }
    if (b_update)
    {
      // Go to the beginning of the text file
      reader_txt_beg();
      data->u16_current_pos = 0;
      // Store the info of the entry
      memcpy(&(data->fs_entry), &fs_g_nav_entry, sizeof(data->fs_entry));
    }
    ctx.status = CMD_EXECUTING;
    ctx.state = (state_t) STATE_SET_POS;
    break;

  case STATE_SET_POS:
    nav_select(FS_NAV_ID_PLAYLIST);
    // Restore the info of the entry
    memcpy(&fs_g_nav_entry, &(data->fs_entry), sizeof(fs_g_nav_entry));
    // Look for the "u16_n"th next lines on the file
    u16_current_n = 0;
    while ((data->u16_current_pos + 1 != u16_position) &&
           !file_eof() &&
           u16_current_n < u16_n)
    {
      if (pl_main_readline(PL_MAIN_READLINE_OPT_CHECKLINE, 0, NULL, NULL))
        data->u16_current_pos++;
      u16_current_n++;
    }
    // If end of file
    if (file_eof())
    {
      // Restore file position
      file_seek(data->u32_file_pos_saved, FS_SEEK_SET);
      // Reselect the previous navigator
      nav_select(data->nav_id_saved);
      return false;
    }
    // If the file has been found
    ctx.status = CMD_EXECUTING;
    if (data->u16_current_pos + 1 == u16_position)
      ctx.state = (state_t) STATE_READ_ENTRY;
    // Else continue seeking...
    // Store the info of the entry
    memcpy(&(data->fs_entry), &fs_g_nav_entry, sizeof(data->fs_entry));
    break;

  case STATE_READ_ENTRY:
    nav_select(FS_NAV_ID_PLAYLIST);
    u16_current_n = 0;
    b_update = false;
    while (!file_eof() && u16_current_n < u16_n)
    {
      if ((b_update = pl_main_readline(PL_MAIN_READLINE_OPT_GOTOPATH, data->nav_id_saved, sz_path, u16_size_line)))
        break;
      u16_current_n++;
    }
    // Need to retry
    if (u16_current_n >= u16_n)
    {
      ctx.status = CMD_EXECUTING;
      break;
    }
    // If it is the end of the file
    if (file_eof() && !b_update)
    {
      // Restore file position
      file_seek(data->u32_file_pos_saved, FS_SEEK_SET);
      // Reselect the previous navigator
      nav_select(data->nav_id_saved);
      return false;
    }

    // Store positions
    data->u16_current_pos = u16_position;
    // Store the info of the entry
    memcpy(&(data->fs_entry), &fs_g_nav_entry, sizeof(data->fs_entry));

    // Restore file position
    file_seek(data->u32_file_pos_saved, FS_SEEK_SET);
    // Reselect the previous navigator
    nav_select(data->nav_id_saved);
    break;
  }

  if (ctx.status == CMD_EXECUTING)
  {
    if (!ai_async_context_push(&ctx))
      return false;
  }

  return true;
}

#endif
