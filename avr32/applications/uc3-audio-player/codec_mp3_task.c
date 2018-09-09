/**************************************************************************
 *
 * \file
 *
 * \brief Audio codec task.
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


#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "com_task.h"
#include "audio_interface.h"
#include "audio_mixer.h"
#include "mp3_libmad_player.h"

extern Ai_player_flag_t g_player_flag;
extern bool g_player_flag_run_mp3;
extern bool g_player_flag_error;

void codec_mp3_task(void);

/*! \brief codec task.
 */
void codec_mp3_task(void)
{
  if (g_player_flag_run_mp3)
  {
    g_player_flag_error = !play_mp3(NULL, false);   // Start playing MP3.
    // Here, the player is stopped.
    g_player_flag_run_mp3 = false;
    if (g_player_flag.status != PLAYER_FLAG_STOP)  // No STOP command.
    {
      // Makes sure the pending commands are done.
      while (!is_ai_async_cmd_finished())
      {
        AI_SYNC_TASK_CALL_BACK();
        ai_async_cmd_task();
      }
      // It is a end of file
#if (PAUSE_ON_TRACK_CHANGED == true)
      ai_audio_nav_eof_occur();
#else
      if (ai_audio_nav_eof_occur())
        ai_audio_ctrl_resume();
#endif
    }
  }
}
