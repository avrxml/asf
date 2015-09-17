/**************************************************************************
 *
 * \file
 *
 * \brief Audio interface configuration file.
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


#ifndef _CONF_AUDIO_INTERFACE_H_
#define _CONF_AUDIO_INTERFACE_H_

#include "conf_audio_player.h"

//! Defines default repeat and shuffle modes
#define AI_DEFAULT_REPEAT_MODE                  AUDIO_REPEAT_ALL
#define AI_DEFAULT_SHUFFLE_MODE                 AUDIO_SHUFFLE_OFF

//! Automatically pause new track played.
//! This option allows to read metadata before starting playback in order to
//! avoid skipping audio while reading metadata.
#define PAUSE_ON_TRACK_CHANGED                  ENABLE

//! Select and play a new track once shuffle is on.
#define CHANGE_TRACK_ON_SHUFFLE                 DISABLE

//! Maximal string length in bytes.
#define STRING_MAX_LENGTH                       64

//! Audio Interface call back used for synchronous commands. When using synchronous
//! commands, the audio interface API will wait for the full completion of the command.
//! During that active wait, the call back is called. This allows respecting real time
//! constraints depending the application.
#if !defined(AI_SYNC_TASK_CALL_BACK)
  #define AI_SYNC_TASK_CALL_BACK()
#endif

//! Number of elements in the context stack for asynchronous commands
#define AI_ASYNC_STACK_NB_ELEMENTS    1

#endif  // _CONF_AUDIO_INTERFACE_H_
