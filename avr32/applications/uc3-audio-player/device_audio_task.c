/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB device Audio task.
 *
 * This file manages the USB device Audio task.
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

