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

#include <stdio.h>
#include "usart.h"     // Shall be included before FreeRTOS header files, since 'inline' is defined to ''; leading to
// link errors
#include "conf_usb.h"

#if USB_DEVICE_FEATURE == true

#include "board.h"
#include "controller.h"
#include "usb_drv.h"
#include "usb_descriptors.h"
#include "usb_standard_request.h"
#include "device_hid_task.h"
#include "audio_example.h"
#include "cycle_counter.h"
#include "debug.h"
#include "hid.h"
#include "et024006dhu.h"

//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

uint8_t key = 0;

#define TIMER_STARTUP    100
t_cpu_time key_timer;


#define HID_VOL_DN       2
#define HID_VOL_UP       1
#define HID_PLAY_PAUSE   4
#define HID_NEXT         8
#define HID_PREV        16
#define HID_FAST_FWD    32
#define HID_REWIND      64


//_____ D E C L A R A T I O N S ____________________________________________


//!
//! @brief Looks for HID events.
//!
bool is_usb_hid_event(void) {
  static bool b_on_off = false;
  static bool b_prev   = false;
  static bool b_next   = false;
  static bool b_vol_up = false;
  static bool b_vol_dn = false;

  // Management of Play/Pause
  //
  if(is_joystick_pressed() && b_on_off==false)
  {
    b_on_off = true;
    key = HID_PLAY_PAUSE;
    et024006_PrintString("PLAY", (const unsigned char *)&FONT8x8, 153, 70+4, BLUE, WHITE);
    return true;
  }
  if(!is_joystick_pressed() && b_on_off==true)
  {
    b_on_off = false;
    key = 0;
    et024006_PrintString("    ", (const unsigned char *)&FONT8x8, 153, 70+4, BLUE, WHITE);
    return true;
  }

  // Management of Prev
  //
  if (is_joystick_left() && b_prev == false)
  {
    b_prev = true;
    key = HID_PREV;

    et024006_PrintString("PREV", (const unsigned char *)&FONT8x8, 153, 70+4, BLUE, WHITE);
    return true;
  }
  if (!is_joystick_left() && b_prev == true)
  {
    b_prev = false;
    key = 0;

    et024006_PrintString("    ", (const unsigned char *)&FONT8x8, 153, 70+4, BLUE, WHITE);
    return true;
  }

  // Management of Next
  //
  if (is_joystick_right() && b_next == false)
  {
    b_next = true;
    key = HID_NEXT;

    et024006_PrintString("NEXT", (const unsigned char *)&FONT8x8, 153, 70+4, BLUE, WHITE);
    return true;
  }
  if (!is_joystick_right() && b_next == true)
  {
    b_next = false;
    key = 0;

    et024006_PrintString("    ", (const unsigned char *)&FONT8x8, 153, 70+4, BLUE, WHITE);
    return true;
  }


  // Management of Vol -
  //
  if (is_joystick_down() && b_vol_dn == false)
  {
    key = HID_VOL_DN;
    b_vol_dn = true;

    et024006_PrintString("VOL-", (const unsigned char *)&FONT8x8, 153, 70+4, BLUE, WHITE);
    return true;
  }
  if (!is_joystick_down() && b_vol_dn == true)
  {
    b_vol_dn = false;
    key = 0;

    et024006_PrintString("    ", (const unsigned char *)&FONT8x8, 153, 70+4, BLUE, WHITE);
    return true;
  }

  // Management of Vol +
  //
  if (is_joystick_up() && b_vol_up == false)
  {
    key = HID_VOL_UP;
    b_vol_up = true;

    et024006_PrintString("VOL+", (const unsigned char *)&FONT8x8, 153, 70+4, BLUE, WHITE);
    return true;
  }
  if (!is_joystick_up() && b_vol_up == true)
  {
    b_vol_up = false;
    key = 0;

    et024006_PrintString("    ", (const unsigned char *)&FONT8x8, 153, 70+4, BLUE, WHITE);
    return true;
  }

  return false;
}

//!
//! @brief This function initializes the hardware/software resources
//! required for device HID task.
//!
void device_hid_task_init(void)
{
  cpu_set_timeout( cpu_ms_2_cy(TIMER_STARTUP, FCPU_HZ), &key_timer );

#if USB_HOST_FEATURE == true
  // If both device and host features are enabled, check if device mode is engaged
  // (accessing the USB registers of a non-engaged mode, even with load operations,
  // may corrupt USB FIFO data).
  if (Is_usb_device())
#endif  // USB_HOST_FEATURE == true
  Usb_enable_sof_interrupt();
}

  //t_cpu_time timer;

  //!
  //! @brief Entry point of the device HID task management
  //!
void device_hid_task(void)
{
  // First, check the device enumeration state
  if (!Is_device_enumerated()) {
    return;
  }

  if (Is_usb_in_ready(EP_KBD_IN)) {
    if (is_usb_hid_event()) {
      Usb_reset_endpoint_fifo_access(EP_KBD_IN);

      //! Write report
      Usb_write_endpoint_data(EP_KBD_IN, 8, key);
      Usb_ack_in_ready_send(EP_KBD_IN);
    }
  }
}

#endif  // USB_DEVICE_FEATURE == true
