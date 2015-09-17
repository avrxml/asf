/**************************************************************************
 *
 * \file
 *
 * \brief Controller interface
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

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "compiler.h"
#include "gui.h"

/*! \name Constructor/Destructor
 */
//! \{
/*! \brief Initialization function of the controller interface.
 *
 * \param fcpu_hz CPU frequency.
 * \param fhsb_hz HSB frequency.
 * \param fpbb_hz PBB frequency.
 * \param fpba_hz PBA frequency.
 */
void controller_init(uint32_t fcpu_hz, uint32_t fhsb_hz, uint32_t fpbb_hz, uint32_t fpba_hz);
/*! \brief Shutdown the controller.
 */
void controller_shutdown(void);
// \}

/*! \brief Reset the key state.
 */
void controller_clear(void);

/*! \name Navigation view
 */
//! \{
/*! \brief This function is called to check if the user has pressed the previous
 *         file selection key.
 *
 * \return true if the key has been pressed.
 *         false otherwise.
 */
bool controller_navigation_cursor_previous(void);

/*! \brief This function is called to check if the user has pressed the next
 *         file selection key.
 *
 * \return true if the key has been pressed.
 *         false otherwise.
 */
bool controller_navigation_cursor_next(void);

/*! \brief This function is called to check if the user has pressed the
 *         go to parent directory key.
 *
 * \return true if the key has been pressed.
 *         false otherwise.
 */
bool controller_navigation_go_to_parent_directory(void);

/*! \brief This function is called to check if the user has pressed the
 *         change directory key.
 *
 * \return true if the key has been pressed.
 *         false otherwise.
 */
bool controller_navigation_change_directory(void);

/*! \name Playback view
 */
//! \{
/*! \brief This function is called to check if the user has pressed the
 *         play/pause key.
 *
 * \return true if the key has been pressed.
 *         false otherwise.
 */
bool controller_playback_toggle_play_pause(void);

/*! \brief This function is called to check if the user has pressed the
 *         increase volume key.
 *
 * \return true if the key has been pressed.
 *         false otherwise.
 */
bool controller_playback_increase_volume(void);

/*! \brief This function is called to check if the user has pressed the
 *         decrease volume key.
 *
 * \return true if the key has been pressed.
 *         false otherwise.
 */
bool controller_playback_decrease_volume(void);

/*! \brief This function is called to check if the user has pressed the
 *         previous track key.
 *
 * \return true if the key has been pressed.
 *         false otherwise.
 */
bool controller_playback_previous_track(void);

/*! \brief This function is called to check if the user has pressed the
 *         next track key.
 *
 * \return true if the key has been pressed.
 *         false otherwise.
 */
bool controller_playback_next_track(void);

/*! \brief This function is called to check if the user has pressed the
 *         fast forward key.
 *
 * \return true if the key has been pressed.
 *         false otherwise.
 */
bool controller_playback_ffw(bool new_track);

/*! \brief This function is called to check if the user has pressed the
 *         fast rewind key.
 *
 * \return true if the key has been pressed.
 *         false otherwise.
 */
bool controller_playback_frw(bool new_track);

// \}

/*! \name Configuration view
 */
//! \{
/*! \brief This function is called to check if the user has pressed the
 *         configuration change key.
 *
 * \return true if the key has been pressed.
 *         false otherwise.
 */
bool controller_config_change_mode(void);

/*! \brief This function is called to check if the user has pressed the
 *         next option key.
 *
 * \return true if the key has been pressed.
 *         false otherwise.
 */
bool controller_config_next_option(void);

/*! \brief This function is called to check if the user has pressed the
 *         previous option key.
 *
 * \return true if the key has been pressed.
 *         false otherwise.
 */
bool controller_config_previous_option(void);

/*! \brief This function is called to check if the user has pressed the
 *         play selected file key.
 *
 * \return true if the key has been pressed.
 *         false otherwise.
 */
bool controller_navigation_play(void);
//! \}

/*! \name View switching
 */
//! \{
/*! \brief This function is called to check if the user has requested to move to
 *         the navigation view.
 *
 * \param view The current view.
 *
 * \return true if the key has been pressed.
 *         false otherwise.
 */
bool controller_switch_to_navigation_view(enum gui_update_view view);

/*! \brief This function is called to check if the user has requested to move to
 *         the playback view.
 *
 * \param view The current view.
 *
 * \return true if the key has been pressed.
 *         false otherwise.
 */
bool controller_switch_to_playback_view(enum gui_update_view view);

/*! \brief This function is called to check if the user has requested to move to
 *         the configuration view.
 *
 * \param view The current view.
 *
 * \return true if the key has been pressed.
 *         false otherwise.
 */
bool controller_switch_to_config_view(enum gui_update_view view);
// \}

#endif // _CONTROLLER_H_
