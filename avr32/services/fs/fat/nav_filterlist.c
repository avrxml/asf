/*****************************************************************************
 *
 * \file
 *
 * \brief FAT 12/16/32 Services.
 *
 * This plug-in allow a navigation in filter mode
 * This plug-in is connected on the navigation module.
 * The filter mode includes in the file list directories and only files corresponding at a extension
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


//************************************************************************
//****************** Filter list navigation functions ********************


//! This function affiliates an extension filter at current navigator
//!
//! @param     sz_filterext   extension filter (ASCII format, e.g.: "txt" or "txt,d*,wk" )
//!
void  nav_filterlist_setfilter( const FS_STRING sz_filterext  )
{
   fs_g_nav.sz_filterext = sz_filterext;
   nav_filterlist_reset();
}


//! This function resets the pointer of selection, so "no file selected" in filtered file list
//!
//! @return  false in case of an error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
bool  nav_filterlist_reset( void )
{
   fs_g_nav.u16_pos_filterlist = FS_NO_SEL;
   return nav_filelist_reset();
}


//! This function goes to the next position in the filtered file list
//!
//! @return    false in case of error or no next file, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool  nav_filterlist_next( void )
{
   uint16_t u16_current_pos;
   u16_current_pos = nav_filelist_get();
   while( nav_filelist_set( 0, FS_FIND_NEXT ) )
   {
      if( nav_file_isdir()
      ||  nav_file_checkext( fs_g_nav.sz_filterext ) )
      {
         if( FS_NO_SEL == u16_current_pos )
         {
            fs_g_nav.u16_pos_filterlist = 0;
         }else{
            fs_g_nav.u16_pos_filterlist++;
         }
         return true;
      }
   }
   nav_filelist_goto( u16_current_pos );
   return false;
}


//! This function goes to the previous position in filtered file list
//!
//! @return    false in case of an error or no next file, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool  nav_filterlist_previous( void )
{
   uint16_t u16_current_pos;
   u16_current_pos = nav_filelist_get();
   while( nav_filelist_set( 0, FS_FIND_PREV ) )
   {
      if( nav_file_isdir()
      ||  nav_file_checkext( fs_g_nav.sz_filterext ) )
      {
         fs_g_nav.u16_pos_filterlist--;
         return true;
      }
   }
   nav_filelist_goto( u16_current_pos );
   return false;
}


//! This function returns the position of selected file in filtered file list
//!
//! @return    position of selected file in directory (0 is the first position)
//! @return    FS_NO_SEL, in case of no file selected
//!
uint16_t   nav_filterlist_get( void )
{
   return fs_g_nav.u16_pos_filterlist;
}


//! This function goes to a position in filtered file list
//!
//! @param     u16_newpos     new position to select (0 is the first position)
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool  nav_filterlist_goto( uint16_t u16_newpos )
{
   if (u16_newpos == FS_NO_SEL)
     return nav_filterlist_reset();

   if( u16_newpos < (fs_g_nav.u16_pos_filterlist/2) )
   {
      // Restart at the beginning of list to accelerate search
      if( !nav_filterlist_reset() )
         return false;
   }
   if( FS_NO_SEL == fs_g_nav.u16_pos_filterlist )
      if( !nav_filterlist_next() )
         return false;  // No file available
   while( u16_newpos > fs_g_nav.u16_pos_filterlist )
   {
      if( !nav_filterlist_next() )
         break;
   }
   while( u16_newpos < fs_g_nav.u16_pos_filterlist )
   {
      if( !nav_filterlist_previous() )
         break;
   }
   return (u16_newpos == fs_g_nav.u16_pos_filterlist);
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
bool  nav_filterlist_findname( const FS_STRING sz_name , bool b_match_case )
{
   while( 1 )
   {
      if ( !nav_filterlist_next())
         return false;
      if ( nav_file_name( sz_name , 0 , FS_NAME_CHECK , b_match_case ))
         return true;
   }
}


//! This function computes the number of files and/or directories in filtered file list
//! This function is blocking. It will only return when the final count is computed,
//! which may take some time.
//!
//! @param      fl_type       FL_ALL  To check all types (files and directories). <br>
//!                           FL_DIR  To check the directory presence. <br>
//!                           FL_FILE To check the file presence.
//!
//! @param      sz_filterext  Extension filter (ASCII format, e.g.: "txt" or "txt,d*,wk" ). <br>
//!                           If this argument in NULL, then it uses the default filter used by the filterlist. <br>
//!                           This argument is also ignored for b_type == FS_DIR.
//!
//! @return     number of files and/or directories present in filtered file list.
//!
uint16_t   nav_filterlist_nb( fl_type_t fl_type, const FS_STRING sz_filterext )
{
   uint16_t total=(uint16_t)-1; // Initialization is mandatory
   while( !nav_filterlist_nb_ex( fl_type, sz_filterext, &total, 0 ) );
   return total;
}

//! Update and test the counter
//! @param  counter the counter
//! @return false if the counter reaches the end
//!         true otherwise
static bool update_counter(uint8_t *counter)
{
  if (*counter)
    (*counter)--;
  if (!*counter)
    return false;
  return true;
}

//! This function computes the number of files and/or directories in filtered file list
//! This function is not blocking. It will return after \<retry\> iteration if the result
//! is not yet computed.
//!
//! @param      fl_type       FL_ALL  To check all types (files and directories). <br>
//!                           FL_DIR  To check the directory presence. <br>
//!                           FL_FILE To check the file presence.
//!
//! @param      sz_filterext  Extension filter (ASCII format, e.g.: "txt" or "txt,d*,wk" ). <br>
//!                           If this argument in NULL, then it uses the default filter used by the filterlist. <br>
//!                           This argument is also ignored for b_type == FS_DIR.
//! @param      p_total       Pointer to the result which will be updated by the function (added).
//!                           Initialization shall be done before entering into this function.
//! @param      retry         Number of files seen before leaving the function. 0 means that the
//!                           function is blocking until the result is known.
//!
//! @return     number of files and/or directories present in filtered file list.
//!
bool   nav_filterlist_nb_ex( fl_type_t fl_type, const FS_STRING sz_filterext, uint16_t* p_total, uint8_t retry )
{
   static uint16_t   u16_save_position;
   bool   b_is_dir;

   // save current position
   if( *p_total==(uint16_t) -1 )
   {
      u16_save_position = fs_g_nav.u16_pos_filterlist;

      // Reset position
      if ( !nav_filterlist_reset())
         return true;

      if (fl_type == FL_DIR)
        nav_filelist_single_enable(FS_DIR);
      else if (fl_type == FL_FILE)
        nav_filelist_single_enable(FS_FILE);

      *p_total = 0;
   }

   // Scan all
   while( nav_filelist_set( 0, FS_FIND_NEXT ) )
   {
      b_is_dir = nav_file_isdir();

      // Check if its a file for FL_FILE mode
      if (fl_type == FL_FILE && b_is_dir)
      {
        if (!update_counter(&retry))
          return false;
        continue;
      }
      // Check if its a directory for FL_DIR mode
      if (fl_type == FL_DIR && !b_is_dir)
      {
        if (!update_counter(&retry))
          return false;
        continue;
      }
      // If the selection is on a file
      if (!b_is_dir)
      {
        // Check the extension of the file
        if (!((sz_filterext)?nav_file_checkext(sz_filterext):nav_file_checkext(fs_g_nav.sz_filterext)))
          if (!update_counter(&retry))
            return false;
      }

      (*p_total)++;
      if (!update_counter(&retry))
         return false;
   }

   nav_filelist_single_disable();

   // Restore previous position
   if ( u16_save_position != FS_NO_SEL )
   {
      nav_filterlist_reset();
   }else{
      // After operation, there are a file selected
      nav_filterlist_goto( u16_save_position );
   }
   return true;
}


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
bool  nav_filterlist_mount( void )
{
   fs_g_nav.u16_pos_filterlist = FS_NO_SEL;
   return nav_partition_mount();
}


//! This function initializes the file filtered list on the root directory
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
bool  nav_filterlist_root( void )
{
   fs_g_nav.u16_pos_filterlist = FS_NO_SEL;
   return nav_dir_root();
}


//! This function enters in the selected directory
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
//! @verbatim
//! After this routine the file list changes and contains the files and directories of the new directory.
//! By default no file is selected.
//! @endverbatim
//!
bool  nav_filterlist_cd( void )
{
   fs_g_nav.u16_pos_filterlist = FS_NO_SEL;
   return nav_dir_cd();
}


//! This function goes to at the parent directory
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
//! @verbatim
//! After, the filtered file list changes and contains the files and directories of the new directory.
//! By default, the file selected in filtered file list is the previous (children) directory.
//! @endverbatim
//!
bool  nav_filterlist_gotoparent( void )
{
   uint32_t u32_cluster_old_dir;

   if (!fat_check_mount_noopen())
      return false;

   if (0 == fs_g_nav.u32_cluster_sel_dir)
   {
      fs_g_status = FS_ERR_IS_ROOT;        // There aren't parent
      return false;
   }

   // Select and read information about directory ".."
   fs_g_nav_fast.u16_entry_pos_sel_file = 1;
   if ( !fat_read_dir())
      return false;
   fat_get_entry_info();
   // Save the children directory cluster
   u32_cluster_old_dir = fs_g_nav.u32_cluster_sel_dir;

   // Select the parent directory via information present in the current directory ".."
   fs_g_nav.u32_cluster_sel_dir = fs_g_nav_entry.u32_cluster;

   // Select the children directory in new directory (=parent directory)
   if( false == nav_filterlist_reset())
      return false;
   while( nav_filterlist_next() )
   {
      if (fs_g_nav_entry.u32_cluster == u32_cluster_old_dir)
         return true;   // It is the children directory
   }
   fs_g_status = FS_ERR_FS;
   return false;
}


//! This function selects a file in the navigator via a file index
//!
//! @param     index       structure with information about file to select (disk, partition, dir, file/dir selected )
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
//! @verbatim
//! This routine allow to reinit a navigator quickly via a file index (disk, partition, dir, file/dir selected )
//! To get a file index, you shall used the routine nav_getindex().
//! @endverbatim
//!
bool  nav_filterlist_gotoindex( const Fs_index _MEM_TYPE_SLOW_ *index )
{
   if( !nav_drive_set( index->u8_lun ))
      return false;
#if (FS_MULTI_PARTITION  ==  true)
   if( !nav_partition_set(index->u8_partition))
      return false;
#endif
   if( !nav_partition_mount())
      return false;

    // Initialization of the current entry file with index information
   fs_g_nav.u32_cluster_sel_dir   = index->u32_cluster_sel_dir;

   // Reset position
   if ( !nav_filterlist_reset() )
      return false;

   // Research the index in directory
   while( fs_g_nav_fast.u16_entry_pos_sel_file != index->u16_entry_pos_sel_file )
   {
      if( !nav_filterlist_next() )
      {
         nav_filterlist_reset();
         return false;
      }
   }
   return true;
}
