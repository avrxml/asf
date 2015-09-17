/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB host mass-storage task.
 *
 * This file manages the USB host mass-storage task.
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


#if USB_HOST_FEATURE == true

#include "board.h"
#ifdef FREERTOS_USED
#include "FreeRTOS.h"
#include "task.h"
#endif
#include "conf_usb.h"
#include "usb_drv.h"
#include "usb_host_enum.h"
#include "usb_host_task.h"
#include "host_mem.h"
#include "ctrl_access.h"
#include "navigation.h"
#include "host_mass_storage_task.h"
#include "ai_usb_ms.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________


//_____ D E C L A R A T I O N S ____________________________________________

static const char log_ms_dev_connected[] = "Mass-storage device connected\r\n";

volatile bool ms_new_device_connected;
volatile bool ms_connected;


//!
//! @brief This function initializes the host mass-storage task.
//!
void host_mass_storage_task_init(void)
{
  ms_new_device_connected = false;
  ms_connected = false;

#ifdef FREERTOS_USED
  xTaskCreate(host_mass_storage_task,
              configTSK_USB_HMS_NAME,
              configTSK_USB_HMS_STACK_SIZE,
              NULL,
              configTSK_USB_HMS_PRIORITY,
              NULL);
#endif  // FREERTOS_USED
}


//!
//! @brief This function manages the host mass-storage task.
//!
#ifdef FREERTOS_USED
void host_mass_storage_task(void *pvParameters)
#else
void host_mass_storage_task(void)
#endif
{
  uint8_t i;
  uint8_t max_lun;
  uint32_t capacity;
  extern volatile uint8_t device_state;
  uint32_t total_capacity;
  uint8_t status = CTRL_GOOD;

#ifdef FREERTOS_USED
  portTickType xLastWakeTime;

  xLastWakeTime = xTaskGetTickCount();
  while (true)
  {
    vTaskDelayUntil(&xLastWakeTime, configTSK_USB_HMS_PERIOD);

#endif  // FREERTOS_USED
    // First, check the host controller is in full operating mode with the
    // B-device attached and enumerated
    if (Is_host_ready())
    {
      // New device connection (executed only once after device connection)
      if (ms_new_device_connected)
      {
        // For all supported interfaces
        for (i = 0; i < Get_nb_supported_interface(); i++)
        {
          // If mass-storage class
          if (Get_class(i) == MS_CLASS)
          {
            total_capacity = 0;

            // Get correct physical pipes associated with IN/OUT endpoints
            if (Is_ep_in(i, 0))
            { // Yes, associate it with the IN pipe
              g_pipe_ms_in = Get_ep_pipe(i, 0);
              g_pipe_ms_out = Get_ep_pipe(i, 1);
            }
            else
            { // No, invert...
              g_pipe_ms_in = Get_ep_pipe(i, 1);
              g_pipe_ms_out = Get_ep_pipe(i, 0);
            }

            ms_new_device_connected = false;
            ms_connected = true;

            // Get the number of LUNs in the connected mass-storage device
            max_lun = host_get_lun();

            // Initialize all USB drives
            for (host_selected_lun = 0; host_selected_lun < max_lun; host_selected_lun++)
            {
              uint32_t retry;
              if ((status = host_ms_inquiry()) != CTRL_GOOD)
                break;
              if ((status = host_ms_request_sense()) != CTRL_GOOD)
                break;
              for (retry = 0; retry < 3; retry++)
              {
                if ((status = host_test_unit_ready(host_selected_lun)) == CTRL_GOOD)
                {
                  if ((status = host_read_capacity(host_selected_lun, &capacity)) == CTRL_GOOD)
                    total_capacity += capacity;
                  break;
                }
              }
            }
            // If busy then restart
            if (status == CTRL_BUSY || status == CTRL_NO_PRESENT)
            {
              ms_connected = false;
              ms_new_device_connected = true;
              return;
            }
            // If failed then report device not supported
            if (status == CTRL_FAIL)
            {
              device_state = DEVICE_ERROR;
              Host_device_class_not_supported_action();
              ms_connected = false;
              return;
            }

#ifdef AI_MAXIMAL_CAPACITY_SUPPORTED_MB
            if (total_capacity > (AI_MAXIMAL_CAPACITY_SUPPORTED_MB*2*1024))
            {
              device_state = DEVICE_ERROR;
              Host_device_class_not_supported_action();
              ms_connected = false;
              return;
            }
#endif

            LOG_STR(log_ms_dev_connected);

            // Notify the Audio Interface of the new connection
            ai_usb_ms_new_connection();
            break;
          }
        }
      }
    }
#ifdef FREERTOS_USED
  }
#endif
}



#endif  // USB_HOST_FEATURE == true
