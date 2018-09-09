/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB device HID task.
 *
 * This file manages the USB device HID task.
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


// Put following MACRO in comment if you need debug info in this module
//#define UART_voUart0PrintString(...) /* NO DEBUG INFO ON UART */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
