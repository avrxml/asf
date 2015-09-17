/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB device HID task.
 *
 * This file manages the USB device HID task.
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


// Put following MACRO in comment if you need debug info in this module
//#define UART_voUart0PrintString(...) /* NO DEBUG INFO ON UART */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

//_____  I N C L U D E S ___________________________________________________
#include "conf_usb.h"

#if USB_DEVICE_FEATURE == true

#include "board.h"
#include "controller.h"
#include "usb_drv.h"
#include "usb_standard_request.h"
#include "device_hid_task.h"
#include "cycle_counter.h"
#include "hid.h"

static uint8_t device_hid_key = HID_NO_KEY;

bool device_hid_is_complete(void)
{
  return (device_hid_key == HID_NO_KEY);
}

bool device_hid_send(uint8_t key)
{
  if (!device_hid_is_complete())
    return false;
  device_hid_key = key;
  return true;
}

//!
//! @brief This function initializes the hardware/software resources
//! required for device HID task.
//!
void device_hid_task_init(void)
{
#if USB_HOST_FEATURE == true
  // If both device and host features are enabled, check if device mode is engaged
  // (accessing the USB registers of a non-engaged mode, even with load operations,
  // may corrupt USB FIFO data).
  if (Is_usb_device())
#endif  // USB_HOST_FEATURE == true
  Usb_enable_sof_interrupt();
}

  //!
  //! @brief Entry point of the device HID task management
  //!
void device_hid_task(void)
{
  // First, check the device enumeration state
  if (!Is_device_enumerated())
    return;

  if (Is_usb_in_ready(EP_KBD_IN))
  {
    if (device_hid_key != HID_NO_KEY)
    {
      Usb_reset_endpoint_fifo_access(EP_KBD_IN);
      //! Write report
      Usb_write_endpoint_data(EP_KBD_IN, 8, device_hid_key);
      // Clear the key or set it ass released
      if (device_hid_key != HID_RELEASE_KEY)
        device_hid_key = HID_RELEASE_KEY;
      else
        device_hid_key = HID_NO_KEY;
      Usb_ack_in_ready_send(EP_KBD_IN);
    }
  }
}

#endif  // USB_DEVICE_FEATURE == true
