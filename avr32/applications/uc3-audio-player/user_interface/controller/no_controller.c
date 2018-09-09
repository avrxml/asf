/**************************************************************************
 *
 * \file
 *
 * \brief No Controller
 *
 * Copyright (c) 2006-2018 Microchip Technology Inc. and its subsidiaries.
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


#include "controller.h"

void controller_init(uint32_t fcpu_hz, uint32_t fhsb_hz, uint32_t fpbb_hz, uint32_t fpba_hz)
{
}

void controller_shutdown(void)
{
}

void controller_clear(void)
{
}

bool controller_navigation_cursor_previous(void)
{
  return false;
}

bool controller_navigation_cursor_next(void)
{
  return false;
}

bool controller_navigation_go_to_parent_directory(void)
{
  return false;
}

bool controller_navigation_change_directory(void)
{
  return false;
}

bool controller_navigation_play(void)
{
  return true;
}

bool controller_switch_to_navigation_view(enum gui_update_view view)
{
  return false;
}

bool controller_switch_to_playback_view(enum gui_update_view view)
{
  return false;
}

bool controller_switch_to_config_view(enum gui_update_view view)
{
  return false;
}

bool controller_playback_toggle_play_pause(void)
{
  return false;
}

bool controller_playback_increase_volume(void)
{
  return false;
}

bool controller_playback_decrease_volume(void)
{
  return false;
}

bool controller_playback_previous_track(void)
{
  return false;
}

bool controller_playback_next_track(void)
{
  return false;
}

bool controller_playback_ffw(bool new_track)
{
  return false;
}

bool controller_playback_frw(bool new_track)
{
  return false;
}

bool controller_config_change_mode(void)
{
  return false;
}

bool controller_config_next_option(void)
{
  return false;
}

bool controller_config_previous_option(void)
{
  return false;
}
