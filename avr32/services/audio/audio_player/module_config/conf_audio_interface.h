/**************************************************************************
 *
 * \file
 *
 * \brief Audio interface configuration file.
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
