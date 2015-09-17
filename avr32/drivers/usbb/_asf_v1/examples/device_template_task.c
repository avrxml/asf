/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB high-level applicative device task.
 *
 * This file manages the USB high-level applicative device task.
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

#include "conf_usb.h"


#if USB_DEVICE_FEATURE == true

#include "board.h"
#ifdef FREERTOS_USED
#include "FreeRTOS.h"
#include "task.h"
#endif
#include "usb_drv.h"
#include "usb_descriptors.h"
#include "usb_standard_request.h"
#include "device_template_task.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

#if BOARD == EVK1100
#  define LED_APPLI_0   LED_BI0_GREEN
#  define LED_APPLI_1   LED_BI0_RED
#elif BOARD == EVK1101 || BOARD == EVK1104 || BOARD == UC3C_EK || BOARD == EVK1105
#  define LED_APPLI_0   LED2
#  define LED_APPLI_1   LED3
#endif

#if !defined(LED_APPLI_0) || \
    !defined(LED_APPLI_1)
#  error The LED configuration to use in this example is missing.
#endif


//_____ D E C L A R A T I O N S ____________________________________________

static U16  sof_cnt;
static U8   data_length;


//!
//! @brief This function initializes the hardware/software resources required for device applicative task.
//!
void device_template_task_init(void)
{
  sof_cnt = 0;
  data_length = 0;
#ifndef FREERTOS_USED
  #if USB_HOST_FEATURE == true
  // If both device and host features are enabled, check if device mode is engaged
  // (accessing the USB registers of a non-engaged mode, even with load operations,
  // may corrupt USB FIFO data).
  if (Is_usb_device())
  #endif  // USB_HOST_FEATURE == true
    Usb_enable_sof_interrupt();
#endif  // FREERTOS_USED

#ifdef FREERTOS_USED
  xTaskCreate(device_template_task,
              configTSK_USB_DTP_NAME,
              configTSK_USB_DTP_STACK_SIZE,
              NULL,
              configTSK_USB_DTP_PRIORITY,
              NULL);
#endif  // FREERTOS_USED
}


//!
//! @brief Entry point of the device applicative task management
//!
//! This function links the device application to the USB bus.
//!
#ifdef FREERTOS_USED
void device_template_task(void *pvParameters)
#else
void device_template_task(void)
#endif
{
  static U8 buf[EP_SIZE_TEMP2];

#ifdef FREERTOS_USED
  portTickType xLastWakeTime;

  xLastWakeTime = xTaskGetTickCount();
  while (true)
  {
    vTaskDelayUntil(&xLastWakeTime, configTSK_USB_DTP_PERIOD);

    // First, check the device enumeration state
    if (!Is_device_enumerated()) continue;
#else
    // First, check the device enumeration state
    if (!Is_device_enumerated()) return;
#endif  // FREERTOS_USED

    // HERE STARTS THE USB DEVICE APPLICATIVE CODE
    // The example below just performs a loopback transmission/reception.
    // All data received with the OUT endpoint is stored in a RAM buffer and
    // sent back to the IN endpoint.

#if BOARD == EVK1100
    // For example, display Start-of-Frame counter on LEDs
    LED_Display_Field(LED_MONO0_GREEN |
                      LED_MONO1_GREEN |
                      LED_MONO2_GREEN |
                      LED_MONO3_GREEN,
                      sof_cnt >> 5);
#elif BOARD == EVK1101 || BOARD == UC3C_EK || BOARD == EVK1104 || BOARD == EVK1105
    // For example, display Start-of-Frame counter on LEDs
    LED_Display_Field(LED0 |
                      LED1,
                      sof_cnt >> 5);
#else
  #error The display of the SOFs must be defined here.
#endif

    // If we receive something in the OUT endpoint, just store it in the RAM buffer
    if (Is_usb_out_received(EP_TEMP_OUT))
    {
      LED_On(LED_APPLI_1);
      Usb_reset_endpoint_fifo_access(EP_TEMP_OUT);
      data_length = Usb_byte_count(EP_TEMP_OUT);
      usb_read_ep_rxpacket(EP_TEMP_OUT, buf, data_length, NULL);
      Usb_ack_out_received_free(EP_TEMP_OUT);
      LED_Off(LED_APPLI_1);
    }

    // Load the IN endpoint with the contents of the RAM buffer
    if (data_length && Is_usb_in_ready(EP_TEMP_IN))
    {
      LED_On(LED_APPLI_0);
      Usb_reset_endpoint_fifo_access(EP_TEMP_IN);
      usb_write_ep_txpacket(EP_TEMP_IN, buf, data_length, NULL);
      data_length = 0;
      Usb_ack_in_ready_send(EP_TEMP_IN);
      LED_Off(LED_APPLI_0);
    }
#ifdef FREERTOS_USED
  }
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
