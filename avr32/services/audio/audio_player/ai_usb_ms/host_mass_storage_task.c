/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB host mass-storage task.
 *
 * This file manages the USB host mass-storage task.
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
