/**************************************************************************
 *
 * \file
 *
 * \brief Controller interface
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
