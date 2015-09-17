/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB host keyboard HID task.
 *
 * This file manages the USB host keyboard HID task.
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
#ifdef FREERTOS_USED
#include "FreeRTOS.h"
#include "task.h"
#endif
#include "conf_usb.h"
#include "usb_drv.h"
#include "usb_host_enum.h"
#include "usb_host_task.h"
#include "host_mem.h"
#include "ctrl_access.h"
#include "navigation.h"
#include "nav_utils.h"
#include "host_keyboard_hid_task.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________


//_____ D E C L A R A T I O N S ____________________________________________

static const char log_ms_dev_connected[] = "Mass-storage device connected\n";

static UNICODE ms_str_unicode[MAX_FILE_PATH_LENGTH];
static uint16_t sof_cnt;

volatile bool ms_new_device_connected;
volatile bool ms_connected;


//!
//! @brief This function initializes the host keyboard HID task.
//!
void host_keyboard_hid_task_init(void)
{
  sof_cnt = 0;
  ms_new_device_connected = false;
  ms_connected = false;

#ifdef FREERTOS_USED
  xTaskCreate(host_keyboard_hid_task,
              configTSK_USB_HHID_KBD_NAME,
              configTSK_USB_HHID_KBD_STACK_SIZE,
              NULL,
              configTSK_USB_HHID_KBD_PRIORITY,
              NULL);
#endif  // FREERTOS_USED
}


//!
//! @brief This function manages the host keyboard HID task.
//!
#ifdef FREERTOS_USED
void host_keyboard_hid_task(void *pvParameters)
#else
void host_keyboard_hid_task(void)
#endif
{
  uint8_t i;
  uint8_t max_lun;
  uint32_t capacity;

#ifdef FREERTOS_USED
  portTickType xLastWakeTime;

  xLastWakeTime = xTaskGetTickCount();
  while (true)
  {
    vTaskDelayUntil(&xLastWakeTime, configTSK_USB_HHID_KBD_PERIOD);

#endif  // FREERTOS_USED
    // First, check the host controller is in full operating mode with the
    // B-device attached and enumerated
    if (Is_host_ready())
    {
      // Display Start-of-Frame counter on LEDs
      LED_Display_Field(LED_MONO0_GREEN |
                        LED_MONO1_GREEN |
                        LED_MONO2_GREEN |
                        LED_MONO3_GREEN,
                        sof_cnt >> 5);

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
              for (i = 0; i < 3; i++)
              {
                if (host_test_unit_ready(host_selected_lun) == CTRL_GOOD)
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
#ifdef FREERTOS_USED
  }
#endif
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
//! @brief Synchronize the contents of two directories (limited to files).
//!
//! @param dst_fs_idx   Fs_index *:   File system index for destination navigation path
//! @param dst_dir      const char *: Pointer to destination directory name
//! @param src_fs_idx   Fs_index *:   File system index for source navigation path
//! @param src_dir      const char *: Pointer to source directory name
//!
//! @return bool: true on success
//!
//! @todo Do recursive directory copy...
//!
bool host_mass_storage_task_sync_dir(Fs_index *dst_fs_idx, const char *dst_dir, Fs_index *src_fs_idx, const char *src_dir)
{
  uint8_t nb_file;
  uint8_t i;
  uint32_t free_space;
  uint16_t file_size;

  // First, check the host controller is in full operating mode with the
  // B-device attached and enumerated
  if (!Is_host_ready()) return false;

  // Go to source navigation
  nav_gotoindex(src_fs_idx);
  if (!goto_code_name(src_dir)) return false;   // Check that source directory exists
  nav_dir_cd();                                 // Source directory exists, so go to it
  *src_fs_idx = nav_getindex();                 // Save navigation position
  nav_filelist_first(FS_FILE);                  // Go to first file
  nb_file = nav_filelist_nb(FS_FILE);           // Get the number of files in this directory

  // Go to destination navigation
  nav_gotoindex(dst_fs_idx);
  if (!goto_code_name(dst_dir))                 // Destination directory does not exist, so create it
  {
    str_code_to_unicode_ram(dst_dir, ms_str_unicode);
    nav_dir_make(ms_str_unicode);
    if (!goto_code_name(dst_dir)) return false; // Check that destination directory has been created
  }
  nav_dir_cd();
  *dst_fs_idx = nav_getindex();

  // Get available free space
  free_space = nav_partition_space();
  nav_gotoindex(src_fs_idx);
  nav_filelist_first(FS_FILE);

  // For all files in directory
  for (i = 0; i < nb_file; i++)
  {
    // Get source name to be used as destination name
    nav_file_name(ms_str_unicode, MAX_FILE_PATH_LENGTH, FS_NAME_GET);
    file_size = nav_file_lgtsector();           // Get file size
    if (file_size > free_space) return false;   // Check that there is enough free space left
    // Update free space (to save time, do no call nav_partition_space() again)
    free_space -= file_size;
    // Mark source
    nav_file_copy();
    // Save current source position
    *src_fs_idx = nav_getindex();

    // Go to destination navigation
    nav_gotoindex(dst_fs_idx);
    if (goto_unicode_name(ms_str_unicode))      // If file already exists
    {
      nav_file_del();                           // Delete it
    }

    // Paste
    nav_file_paste_start(ms_str_unicode);
    // Restore previous navigation position
    nav_gotoindex(src_fs_idx);
    // Copy
    while (nav_file_paste_state(false) == COPY_BUSY);

    // Restore previous navigation position
    nav_gotoindex(src_fs_idx);
    nav_filelist_set(0, FS_FIND_NEXT);
  }

  return true;
}


//!
//! @brief Synchronize the contents of two drives (limited to files).
//!
//! @param sync_direction   uint8_t: DEVICE_TO_HOST, HOST_TO_DEVICE or FULL_SYNC
//!
//! @return bool: true on success
//!
//! @todo Do recursive directory copy...
//!
bool host_mass_storage_task_sync_drives(uint8_t sync_direction)
{
  Fs_index local_index;
  Fs_index sav_local_index;
  Fs_index usb_index;
  Fs_index sav_usb_index;

  // First, check the host controller is in full operating mode with the
  // B-device attached and enumerated
  if (!Is_host_ready()) return false;

  nav_reset();

  // Try to mount local drive
  nav_drive_set(LUN_ID_AT45DBX_MEM);
  if (!nav_partition_mount()) return false;
  local_index = nav_getindex();
  sav_local_index = nav_getindex();

  // Try to mount first USB device drive
  nav_drive_set(LUN_ID_MEM_USB);
  if (!nav_partition_mount()) return false;
  usb_index = nav_getindex();
  sav_usb_index = nav_getindex();

  // First synchronization: USB/OUT -> Local/IN
  if (sync_direction & DEVICE_TO_HOST)
  {
    if (!host_mass_storage_task_sync_dir(&local_index, DIR_LOCAL_IN_NAME, &usb_index, DIR_USB_OUT_NAME))
      return false;
  }

  // Restore positions
  nav_gotoindex(&sav_local_index);
  local_index = nav_getindex();
  nav_gotoindex(&sav_usb_index);
  usb_index = nav_getindex();
  nav_gotoindex(&local_index);

  // Second synchronization: Local/OUT -> USB/IN
  if (sync_direction & HOST_TO_DEVICE)
  {
    if (!host_mass_storage_task_sync_dir(&usb_index, DIR_USB_IN_NAME, &local_index, DIR_LOCAL_OUT_NAME))
      return false;
  }

  return true;
}


#endif  // USB_HOST_FEATURE == true
