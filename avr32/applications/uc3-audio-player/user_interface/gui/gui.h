/*****************************************************************************
 *
 * \file
 *
 * \brief Graphical User Interface.
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


#ifndef _GUI_H_
#define _GUI_H_

#include "com_task.h"

/*! \name View to update
 */
//! \{
enum gui_update_view
{
  //! \brief No view selected
  GUI_UPDATE_VIEW_NONE,
  //! \brief Idle view
  GUI_UPDATE_VIEW_IDLE,
  //! \brief Transition to the idle view
  GUI_UPDATE_VIEW_IDLE_TRANSITION,
  //! \brief Navigation view
  GUI_UPDATE_VIEW_NAVIGATION,
  //! \brief Transition to the navigation view
  GUI_UPDATE_VIEW_NAVIGATION_TRANSITION,
  //! \brief Playback view
  GUI_UPDATE_VIEW_PLAYBACK,
  //! \brief Transition to the playback view
  GUI_UPDATE_VIEW_PLAYBACK_TRANSITION,
  //! \brief Configuration view
  GUI_UPDATE_VIEW_CONFIG,
  //! \brief Transition to the configuration view
  GUI_UPDATE_VIEW_CONFIG_TRANSITION
};
//! \}

/*! \name Elements of the view to update
 */
//! \{
enum gui_update_elt
{
  //! \brief No element selected
  GUI_UPDATE_ELT_NONE                     = 0x00000000,
  //! \brief In progress element
  GUI_UPDATE_ELT_IN_PROGRESS              = 0x80000000,
  //! \brief Device connected, mounted and ready to operate
  GUI_UPDATE_ELT_CONNECTED                = 0x40000000,
  //! \brief Device disconnected
  GUI_UPDATE_ELT_DISCONNECTED             = 0x20000000,

  //! \brief Device not supported message in the idle view
  GUI_UPDATE_ELT_IDLE_NOT_SUPPORTED       = 0x00000001,
  //! \brief Device present message in the idle view
  GUI_UPDATE_ELT_IDLE_DEVICE_PRESENT      = 0x00000002,
  //! \brief No device present message in the idle view
  GUI_UPDATE_ELT_IDLE_NO_DEVICE_PRESENT   = 0x00000004,

  //! \brief File list update in the navigation view
  GUI_UPDATE_ELT_NAVIGATION_FILE_LIST     = 0x00000001,
  //! \brief Cursor display in the navigation view
  GUI_UPDATE_ELT_NAVIGATION_CURSOR        = 0x00000002,
  //! \brief Raised when a directory with no files/folders is opened
  GUI_UPDATE_ELT_NAVIGATION_NO_FILES      = 0x00000004,

  //! \brief Play key display in the playback view
  GUI_UPDATE_ELT_PLAYBACK_PLAY            = 0x00000001,
  //! \brief Pause key display in the playback view
  GUI_UPDATE_ELT_PLAYBACK_PAUSE           = 0x00000002,
  //! \brief Stop key display in the playback view
  GUI_UPDATE_ELT_PLAYBACK_STOP            = 0x00000004,
  //! \brief Fast forward key display in the playback view
  GUI_UPDATE_ELT_PLAYBACK_FFW             = 0x00000008,
  //! \brief Fast rewind key display in the playback view
  GUI_UPDATE_ELT_PLAYBACK_FRW             = 0x00000010,
  //! \brief Elapsed time display in the playback view
  GUI_UPDATE_ELT_PLAYBACK_ELAPSED_TIME    = 0x00000020,
  //! \brief Volume bar display in the playback view
  GUI_UPDATE_ELT_PLAYBACK_VOLUME          = 0x00000040,
  //! \brief Artist name display in the playback view
  GUI_UPDATE_ELT_PLAYBACK_ARTIST          = 0x00000080,
  //! \brief Track title display in the playback view
  GUI_UPDATE_ELT_PLAYBACK_TITLE           = 0x00000100,
  //! \brief Total time display in the playback view
  GUI_UPDATE_ELT_PLAYBACK_TOTAL_TIME      = 0x00000200,
  //! \brief Cover art display in the playback view
  GUI_UPDATE_ELT_PLAYBACK_COVER_ART       = 0x00000400,
  //! \brief Current song file name display in the playback view
  GUI_UPDATE_ELT_PLAYBACK_FILE_NAME       = 0x00000800,
  //! \brief Apple Logo (in case on Local UI mode)
  GUI_UPDATE_ELT_APPLE_LOGO               = 0x00001000,

  //! \brief Repeat option display in the configuration view
  GUI_UPDATE_ELT_CONFIG_REPEAT            = 0x00000001,
  //! \brief Shuffle option display in the configuration view
  GUI_UPDATE_ELT_CONFIG_SHUFFLE           = 0x00000002,
  //! \brief Cursor display in the configuration view
  GUI_UPDATE_ELT_CONFIG_CURSOR            = 0x00000004
};
//! \}

/*! \brief Initialization function of the GUI interface.
 *
 * \param fcpu_hz CPU frequency.
 * \param fhsb_hz HSB frequency.
 * \param fpbb_hz PBB frequency.
 * \param fpba_hz PBA frequency.
 */
void gui_init(uint32_t fcpu_hz, uint32_t fhsb_hz, uint32_t fpbb_hz, uint32_t fpba_hz);

/*! \brief Shutdown the GUI
 */
void gui_shutdown(void);

/*! \brief GUI main task update
 *
 * \param view The view to update.
 * \param element The elements of the view to be update.
 * \param lst_str The list of file in the current directory.
 * \param info Information of the selected file.
 * \param player Information about the player itself.
 */
void gui_update(enum gui_update_view view, enum gui_update_elt *element,
                struct display_file_list *lst_str, struct file_info *info,
                struct player_info *player);

/*! \brief Make the display darker in order to simulate an idle mode and reduce
 *         power consumption.
 */
void gui_enter_idle(void);

/*! \brief Restore the display in the running mode.
 *
 */
void gui_leave_idle(void);

#endif // _GUI_H_
