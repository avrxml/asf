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


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________


//_____ D E C L A R A T I O N S ____________________________________________

static const char log_ms_dev_connected[] = "Mass-storage device connected\n";

static uint16_t sof_cnt;

volatile bool ms_new_device_connected;
volatile bool ms_connected;


//!
//! @brief This function initializes the host mass-storage task.
//!
void host_mass_storage_task_init(void)
{
  sof_cnt = 0;
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
#if BOARD == EVK1100
      // Display Start-of-Frame counter on LEDs
      LED_Display_Field(LED_MONO0_GREEN |
                        LED_MONO1_GREEN |
                        LED_MONO2_GREEN |
                        LED_MONO3_GREEN,
                        sof_cnt >> 5);
#elif BOARD == EVK1101 || BOARD == UC3C_EK || BOARD == EVK1104 || BOARD == EVK1105 || BOARD == UC3C_EK
      // Display Start-of-Frame counter on LEDs
      LED_Display_Field(LED0 |
                        LED1,
                        sof_cnt >> 5);
#else
  #error The display of the SOFs must be defined here.
#endif

      // New device connection (executed only once after device connection)
      if (ms_new_device_connected)
      {
        ms_new_device_connected = false;

        // For all supported interfaces
        for (i = 0; i < Get_nb_supported_interface(); i++)
        {
          // If mass-storage class
          if (Get_class(i) == MS_CLASS)
          {
            ms_connected = true;
            LOG_STR(log_ms_dev_connected);

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

            // Get the number of LUNs in the connected mass-storage device
            max_lun = host_get_lun();

            // Initialize all USB drives
            for (host_selected_lun = 0; host_selected_lun < max_lun; host_selected_lun++)
            {
              if( host_ms_inquiry() != CTRL_GOOD )
                continue;
              if( host_read_capacity(host_selected_lun, &capacity) != CTRL_GOOD )
                continue;
              if( host_ms_request_sense() == CTRL_FAIL )
                continue;
              for (i = 0; i < 3; i++)
              {
                if (host_test_unit_ready(host_selected_lun) == CTRL_GOOD)
                {
                  host_read_capacity(host_selected_lun, &capacity);
                  break;
                }
              }
            }
            break;
          }
        }
      }
    }
#ifdef FREERTOS_USED
  }
#endif
}


//!
//! @brief host_sof_action
//!
//! This function increments the sof_cnt counter each time
//! the USB Start-of-Frame interrupt subroutine is executed (1 ms).
//! Useful to manage time delays
//!
void host_sof_action(void)
{
  sof_cnt++;
}


#endif  // USB_HOST_FEATURE == true
