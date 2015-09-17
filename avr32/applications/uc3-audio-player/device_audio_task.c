/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB device Audio task.
 *
 * This file manages the USB device Audio task.
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

#include <stdio.h>
#include "conf_usb.h"


#if USB_DEVICE_FEATURE == true

#include "board.h"
#include "controller.h"
#include "usb_drv.h"
#include "usb_descriptors.h"
#include "usb_standard_request.h"
#include "device_audio_task.h"
#include "usb_stream_player.h"
#include "cycle_counter.h"

device_audio_task_data_t *device_audio_task_data = NULL;

static uint16_t  sof_cnt;
//!
//! Public : (bit) mute
//! mute is set to true when ACTIVE
//! mute is set to false otherwise
//!/
bool   mute;

//!
//! Public : (int16_t) volume
//! volume represents the current volume of audio stream
//!/
int16_t volume;

//!
//! @brief This function initializes the hardware/software resources
//! required for device Audio task.
//!
void device_audio_task_init(void)
{
  sof_cnt   =0;

  player_init();

  if (!device_audio_task_data)
    device_audio_task_data = calloc(1, sizeof(*device_audio_task_data));

  usb_stream_init(
    DEFAULT_DAC_SAMPLE_RATE_HZ
  , 2
  , 16
  , false
  );
}

//!
//! @brief Entry point of the device Audio task management
//!
void device_audio_task(void)
{
  uint32_t  fifo_cnt;

  // Handle incoming audio samples
  //
  if (Is_usb_out_received(EP_AUDIO_OUT))
  {
    LED_On(LED0);
    usb_stream_input(USB_STREAM_DEVICE, EP_AUDIO_OUT, &fifo_cnt);
    Usb_ack_out_received_free(EP_AUDIO_OUT);
    LED_Off(LED0);
  }
#if USB_RESYNC_AUDIO_STREAM && (USB_RESYNC_AUDIO_STREAM == true)
  usb_stream_resync();
#endif
}

//!
//! @brief usb_sof_action
//!
//! This function increments the sof_cnt counter each time
//! the USB Start-of-Frame interrupt subroutine is executed (1 ms).
//! Useful to manage time delays
//!
void usb_sof_action(void)
{
  sof_cnt++;
}

#endif  // USB_DEVICE_FEATURE == true

