/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB high-level applicative device task.
 *
 * This file manages the USB high-level applicative device task.
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
