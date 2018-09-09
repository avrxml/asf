/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB high-level applicative host task.
 *
 * This file manages the USB high-level applicative host task.
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
#include "usb_drv.h"
#include "usb_host_enum.h"
#include "usb_host_task.h"
#include "host_template_task.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

#if BOARD == EVK1100
#  define LED_APPLI_0   LED_BI0_GREEN
#  define LED_APPLI_1   LED_BI0_RED
#elif BOARD == EVK1101 || BOARD == UC3C_EK || BOARD == EVK1104 || BOARD == EVK1105
#  define LED_APPLI_0   LED2
#  define LED_APPLI_1   LED3
#endif

#if !defined(LED_APPLI_0) || \
    !defined(LED_APPLI_1)
#  error The LED configuration to use in this example is missing.
#endif


//_____ D E C L A R A T I O N S ____________________________________________

static            U16 sof_cnt;
static            U8  pipe_in;
static            U8  pipe_out;
static            U8  buf[64];
#if USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE
static  volatile  U8  busy;
#endif

volatile bool tpl_new_device_connected;


//!
//! @brief This function initializes the high-level host application.
//! Here initialize specific hardware resources requirements.
//!
void host_template_task_init(void)
{
  U8 i;

  sof_cnt = 0;
  for (i = 0; i < sizeof(buf); i++) buf[i] = i;
#if USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE
  busy = false;
#endif
  tpl_new_device_connected = false;

#ifdef FREERTOS_USED
  xTaskCreate(host_template_task,
              configTSK_USB_HTP_NAME,
              configTSK_USB_HTP_STACK_SIZE,
              NULL,
              configTSK_USB_HTP_PRIORITY,
              NULL);
#endif  // FREERTOS_USED
}


//!
//! @brief High-level host applicative task entry point
//! Put here the code of your host application.
//! The sample code just sends and receives 64 bytes from
//! IN and OUT pipes.
//!
#ifdef FREERTOS_USED
void host_template_task(void *pvParameters)
#else
void host_template_task(void)
#endif
{
#if USB_HOST_PIPE_INTERRUPT_TRANSFER == DISABLE
  Status_t sta;
  U16 nb;
#endif
  U8 i;

#ifdef FREERTOS_USED
  portTickType xLastWakeTime;

  xLastWakeTime = xTaskGetTickCount();
  while (true)
  {
    vTaskDelayUntil(&xLastWakeTime, configTSK_USB_HTP_PERIOD);

#endif  // FREERTOS_USED
    // First, check the host controller is in full operating mode with the
    // B-device attached and enumerated
    if (Is_host_ready())
    {
      // Put here the code to execute in host mode

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

      // New device connection (executed only once after device connection)
      if (tpl_new_device_connected)
      {
        tpl_new_device_connected = false;

#if USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE
        // No more pipe interrupt transfer pending
        busy = false;
#endif

        // For all supported interfaces
        for (i = 0; i < Get_nb_supported_interface(); i++)
        {
          // If vendor-specific class
          if (Get_class(i) == VENDOR_CLASS)
          {
            // Get correct physical pipes associated with IN/OUT endpoints
            if (Is_ep_in(i, 0))
            { // Yes, associate it with the IN pipe
              pipe_in = Get_ep_pipe(i, 0);
              pipe_out = Get_ep_pipe(i, 1);
            }
            else
            { // No, invert...
              pipe_in = Get_ep_pipe(i, 1);
              pipe_out = Get_ep_pipe(i, 0);
            }
            break;
          }
        }
      }

#if USB_HOST_PIPE_INTERRUPT_TRANSFER == DISABLE
      // The sample task sends 64 bytes through OUT pipe
      LED_On(LED_APPLI_0);
      sta = host_send_data(pipe_out, sizeof(buf), buf);
      LED_Off(LED_APPLI_0);

      // And receives 64 bytes from IN pipe
      nb = sizeof(buf);
      LED_On(LED_APPLI_1);
      sta = host_get_data(pipe_in, &nb, buf);
      LED_Off(LED_APPLI_1);
#else
      // Similar applicative task under interrupt mode...
      if (!busy)
      {
        busy = true;
        LED_On(LED_APPLI_0);
        host_send_data_interrupt(pipe_out, sizeof(buf), buf, host_send_data_callback);
        LED_Off(LED_APPLI_0);
      }
#endif

      // Here is an example of an applicative request to go to USB suspend ...
      if (false/* applicative conditions */)
      {
        host_set_feature_remote_wakeup();
        Host_request_suspend();
      }
    }

    // Here an applicative example of resume request...
    if (Is_host_suspended()/* && applicative conditions */)
    {
      Host_request_resume();
    }
#ifdef FREERTOS_USED
  }
#endif
}


#if USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE

void host_send_data_callback(Status_t status, U16 nb_byte)
{
  if (status == PIPE_GOOD)
  {
    LED_On(LED_APPLI_1);
    host_get_data_interrupt(pipe_in, sizeof(buf), buf, host_get_data_callback);
    LED_Off(LED_APPLI_1);
  }
}


void host_get_data_callback(Status_t status, U16 nb_byte)
{
  if (status == PIPE_GOOD)
  {
    busy = false;
  }
}

#endif  // USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE


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
