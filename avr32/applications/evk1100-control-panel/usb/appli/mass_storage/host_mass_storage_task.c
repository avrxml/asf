/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB host mass-storage task.
 *
 * This file manages the USB host mass-storage task.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


//_____  I N C L U D E S ___________________________________________________

#include "conf_usb.h"


#if USB_HOST_FEATURE == true

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "conf_usb.h"
#include "usb_drv.h"
#include "usb_host_enum.h"
#include "usb_host_task.h"
#include "host_mem.h"
#include "ctrl_access.h"
#include "navigation.h"
#include "fsaccess.h"
#include "host_mass_storage_task.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________


//_____ D E C L A R A T I O N S ____________________________________________

#if configCTRLPANEL_TRACE == 1
static const char log_ms_dev_connected[] = "Mass-storage device connected\n";
#endif

static char ms_str[MAX_FILE_PATH_LENGTH];
static U16 sof_cnt;

volatile bool ms_new_device_connected;
volatile bool ms_connected;


//!
//! @brief This function initializes the host mass-storage task.
//!
void host_mass_storage_task_init(void)
{
  sof_cnt = 0;
  ms_new_device_connected = false;
  ms_connected = false;

  xTaskCreate(host_mass_storage_task,
              configTSK_USB_HMS_NAME,
              configTSK_USB_HMS_STACK_SIZE,
              NULL,
              configTSK_USB_HMS_PRIORITY,
              NULL);
}


//!
//! @brief This function manages the host mass-storage task.
//!
//! @param pvParameters Input. Unused.
//!
void host_mass_storage_task(void *pvParameters)
{
  U8 i;
  U8 max_lun;
  U32 capacity;

  portTickType xLastWakeTime;

  xLastWakeTime = xTaskGetTickCount();
  while (true)
  {
    vTaskDelayUntil(&xLastWakeTime, configTSK_USB_HMS_PERIOD);

    // First, check the host controller is in full operating mode with the
    // B-device attached and enumerated
    if (Is_host_ready())
    {
      // Display Start-of-Frame counter on LEDs
      // LED_Display_Field(0x7E, sof_cnt >> 5);

      // New device connection (executed only once after device connection)
      if (ms_new_device_connected)
      {
        ms_new_device_connected = false;

        // For all supported interfaces
        for (i = 0; i < Get_nb_supported_interface(); i++)
        {
          // If mass-storage class
          if (Get_class(i) == MS_CLASS)
          {
            ms_connected = true;
            LOG_STR(log_ms_dev_connected);

            // Get correct physical pipes associated with IN/OUT endpoints
            if (Is_ep_in(i, 0))
            { // Yes, associate it with the IN pipe
              g_pipe_ms_in = Get_ep_pipe(i, 0);
              g_pipe_ms_out = Get_ep_pipe(i, 1);
            }
            else
            { // No, invert...
              g_pipe_ms_in = Get_ep_pipe(i, 1);
              g_pipe_ms_out = Get_ep_pipe(i, 0);
            }

            // Get the number of LUNs in the connected mass-storage device
            max_lun = host_get_lun();

            // Initialize all USB drives
            for (host_selected_lun = 0; host_selected_lun < max_lun; host_selected_lun++)
            {
              host_ms_inquiry();
              host_read_capacity(host_selected_lun, &capacity);
              host_ms_request_sense();
              // while (host_test_unit_ready(host_selected_lun) != CTRL_GOOD);
              for( i=0; i<3; i++)
              {
                 if( host_test_unit_ready(host_selected_lun) == CTRL_GOOD )
                 {
                    host_read_capacity(host_selected_lun, &capacity);
                    break;
                 }
              }
            }
            break;
          }
        }
      }
    }
  }
}


//!
//! @brief host_sof_action
//!
//! This function increments the sof_cnt counter each time
//! the USB Start-of-Frame interrupt subroutine is executed (1 ms).
//! Useful to manage time delays
//!
void host_sof_action(void)
{
  sof_cnt++;
}


//!
//! @brief Synchronize the contents of two directories between the USB drive and
//! the AT45DBX dataflash file systems.
//!
//! @param pcdir_name      const char *: directory name null-terminated string
//! @param sync_direction  U8: DEVICE_TO_HOST, HOST_TO_DEVICE or FULL_SYNC
//! @param bDeleteSrc   bool: if true delete the src directory content.
//!
//! @return bool: true on success else false
//!
bool host_mass_storage_task_copy_tree( const char *pcdir_name, U8 sync_direction,
                                       bool bDeleteSrc )
{
   U8   u8_folder_level = 0;
   long nav_src = 0; // A navigator for the source
   long nav_dst = 0; // A navigator for the destination
   // DEVICE_TO_HOST default sync direction
   U8   u8SrcDrv = LUN_ID_MEM_USB;
   U8   u8DstDrv = LUN_ID_AT45DBX_MEM;
   U8   u8Status;


   // Eventually change the sync direction.
   if( sync_direction & HOST_TO_DEVICE )
   {
      u8SrcDrv = LUN_ID_AT45DBX_MEM;
      u8DstDrv = LUN_ID_MEM_USB;
   }

   // Use three navigators (0 to explore the src, 1 to explore the dst,
   // 2 used by the copy file routine)
   nav_src = FS_NAV_ID_COPYFILE_TREE_SRC;
   nav_dst = FS_NAV_ID_COPYFILE_TREE_DEST;

   // Src init.
   nav_select( nav_src );
   if( !nav_drive_set( u8SrcDrv ))
      return( false );
   if( !nav_partition_mount() )
      return( false );
   if( !nav_filelist_findname( (FS_STRING )pcdir_name , false) )   // search dir pcdir_name
      return( false );
   if( !nav_dir_cd())
      return( false );

   // Dst init.
   nav_select( nav_dst );
   if( !nav_drive_set( u8DstDrv ))
      return( false );
   if( !nav_partition_mount() )
      return( false );
   // Create folder on the destination disk
   if( !nav_dir_make( (FS_STRING )pcdir_name ))
   {
      if( FS_ERR_FILE_EXIST != fs_g_status ) // !!!! Check available only on last version of FileSystem module package >=1.0.32
         return( false );
      // here, error the name exist
   }
   // Here the navigator have selected the folder on destination
   if( !nav_dir_cd())
      return( false );


   // Loop to scan and create ALL folders and files
   while(1)
   {
      // No dir in current dir then go to parent dir on src and dst
      while(1)
      {              // Search files or dir
         // Reselect src
         nav_select( nav_src );
         if( nav_filelist_set( 0 , FS_FIND_NEXT ) )
            break;   // a next file and directory is found

         // No other dir or file in current dir then go to parent dir on src and dst
         if( 0 == u8_folder_level ) // end of update folder
            return true;   //********* END OF COPY **************

                     // Go to parent
         // Remark, nav_dir_gotoparent() routine go to in parent dir and select
         // the children dir in list
         u8_folder_level--;
         if( !nav_dir_gotoparent() )
            return( false );
         // Select dst navigator and go to the same dir of src
         nav_select( nav_dst );
         if( !nav_dir_gotoparent() )
            return( false );
      } // end of while (1)

      if( nav_file_isdir())
      {              // Dir found - create dir & CD
         //** here, a new directory is found and is selected
         // Get name of current selection (= dir name on src)
         if( !nav_file_name( (FS_STRING )ms_str , MAX_FILE_PATH_LENGTH  , FS_NAME_GET, false  ))
            return( false );
         // Enter in dir (on src)
         if( !nav_dir_cd())
            return( false );
         u8_folder_level++;
         // Select dst
         nav_select( nav_dst );
         // Create folder in dst
         if( !nav_dir_make( (FS_STRING )ms_str ))
         {
            if( FS_ERR_FILE_EXIST != fs_g_status )
               return( false );
            // here, error the name exist
         }
         // Here the navigator have selected the folder on dst
         if( !nav_dir_cd())
         {
            if( FS_ERR_NO_DIR == fs_g_status )
            {
               // Copy impossible, because a file has the same name as folder
            }
            return( false );
         }
         // here, the folder is created and the navigator has entered in this dir
      }
      else
      {              // File found - copy file
         //** here, a new file is found and is selected
         // Get name of current selection (= file name on src)
         if( !nav_file_name( (FS_STRING )ms_str , MAX_FILE_PATH_LENGTH  , FS_NAME_GET , false  ))
            return( false );
         if( !nav_file_copy())
            return( false );

         // Paste file in current dir of dst
         nav_select( nav_dst );
         while( !nav_file_paste_start( (FS_STRING)ms_str ) )
         {
            // Error
            if( fs_g_status != FS_ERR_FILE_EXIST )
               return( false );
            // del file
            // File exist then del this one
            if( !nav_file_del( true ) )
               return( false );
            // here, retry PASTE
         }
         // Copy running
         do{
            u8Status = nav_file_paste_state(false);
         }while( COPY_BUSY == u8Status );

         if( COPY_FINISH != u8Status )
            return( false );

         if( true == bDeleteSrc )
         {
            nav_select( nav_src );
            nav_file_del( false ); // Delete the source file
         }
      } // if dir OR file
   } // end of first while(1)
}


//!
//! @brief Synchronize the contents of two drives (limited to files).
//!
//! @param FsNavId          signed short: the file system navigator id to use.
//! @param sync_direction   U8: DEVICE_TO_HOST, HOST_TO_DEVICE or FULL_SYNC
//! @param pcdir_name       const char *: directory name to consider.
//! @param bDeleteSrc       bool: if true delete the src directory content.
//!
//! @return bool: true on success
//!
//! @todo Do recursive directory copy...
//!
bool host_mass_storage_task_sync_drives( signed short FsNavId, U8 sync_direction,
                                         const char *pcdir_name, bool bDeleteSrc )
{
  // First, check the host controller is in full operating mode with the
  // B-device attached and enumerated
  if (!Is_host_ready()) return false;

  fsaccess_take_mutex();   // Take the fs resource.

  // First synchronization: USB/OUT -> Local/IN
  if (sync_direction & DEVICE_TO_HOST)
  {
     if( false == host_mass_storage_task_copy_tree( pcdir_name, DEVICE_TO_HOST, bDeleteSrc ) )
     {

        fsaccess_give_mutex();   // Release the fs resource.
        return false;
     }
  }

  // Second synchronization: Local/pcdir_name -> USB/pcdir_name
  if (sync_direction & HOST_TO_DEVICE)
  {
     if( false == host_mass_storage_task_copy_tree( pcdir_name, HOST_TO_DEVICE, bDeleteSrc ) )
     {
        fsaccess_give_mutex();   // Release the fs resource.
        return false;
     }
  }

  fsaccess_give_mutex();   // Release the fs resource.
  return true;
}


#endif  // USB_HOST_FEATURE == true

