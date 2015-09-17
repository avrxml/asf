/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB host CDC task.
 *
 * This file manages the USB host CDC task.
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

#include <stdio.h>
#include "compiler.h"
#include "host_cdc_task.h"
#if (defined CDC_USE_UART)&& (CDC_USE_UART== true)
# include "usart.h"     // Shall be included before FreeRTOS header files, since 'inline' is defined to ''; leading to
                        // link errors
#endif
#include "conf_usb.h"


#if USB_HOST_FEATURE == true

#include "board.h"
#ifdef FREERTOS_USED
#include "FreeRTOS.h"
#include "task.h"
#endif
#include "conf_usb.h"
#include "cdc.h"
#include "usb_drv.h"
#include "usb_host_enum.h"
#include "usb_host_task.h"



//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________


//_____ D E C L A R A T I O N S ____________________________________________

uint8_t  cdc_interface_comm;

uint8_t  pipe_cdc_comm_int;
uint8_t  pipe_cdc_data_bulkin;
uint8_t  pipe_cdc_data_bulkout;

uint8_t  host_tx_cnt;
uint8_t  host_rx_cnt;
uint8_t  cdc_stream_out_array[CDC_STREAM_OUT_SIZE];
uint8_t  cdc_stream_in_array[CDC_STREAM_IN_SIZE];

volatile bool cdc_new_device_connected;
volatile bool cdc_connected;
static   volatile uint16_t  sof_cnt;


//!
//! @brief This function initializes the host CDC task.
//!
void host_cdc_task_init(void)
{
  sof_cnt = 0;
  host_tx_cnt = 0;
  host_rx_cnt = 0;
  cdc_interface_comm = 0;
  cdc_new_device_connected = false;
  cdc_connected = false;

#ifdef FREERTOS_USED
  xTaskCreate(host_cdc_task,
              configTSK_USB_HCDC_NAME,
              configTSK_USB_HCDC_STACK_SIZE,
              NULL,
              configTSK_USB_HCDC_PRIORITY,
              NULL);
#endif  // FREERTOS_USED
}


//!
//! @brief This function manages the host CDC task.
//!
#ifdef FREERTOS_USED
void host_cdc_task(void *pvParameters)
#else
void host_cdc_task(void)
#endif
{
  uint8_t i;
  int c;
  static bool startup=true;

#ifdef FREERTOS_USED
  portTickType xLastWakeTime;

  xLastWakeTime = xTaskGetTickCount();
  while (true)
  {
    vTaskDelayUntil(&xLastWakeTime, configTSK_USB_HCDC_PERIOD);

#endif  // FREERTOS_USED
    // First, check the host controller is in full operating mode with the
    // B-device attached and enumerated
    if( Is_host_ready() )
    {
      // New device connection (executed only once after device connection)
      if( cdc_new_device_connected )
      {
        cdc_new_device_connected = false;

        // For all supported interfaces
        for( i=0 ; i<Get_nb_supported_interface() ; i++ )
        {
          // Data Interface
          if( Get_class(i)==CDC_DATA_CLASS && Get_protocol(i)==NO_PROTOCOL )
          {
            cdc_connected=true;

            // Get correct physical pipes associated with IN/OUT endpoints
            if (Is_ep_in(i, 0))
            { // Yes, associate it with the IN pipe
              pipe_cdc_data_bulkin = Get_ep_pipe(i, 0);
              pipe_cdc_data_bulkout = Get_ep_pipe(i, 1);
            }
            else
            { // No, invert...
              pipe_cdc_data_bulkin = Get_ep_pipe(i, 1);
              pipe_cdc_data_bulkout = Get_ep_pipe(i, 0);
            }
            Host_enable_continuous_in_mode(pipe_cdc_data_bulkin);
            Host_unfreeze_pipe(pipe_cdc_data_bulkin);
            continue;
          }

          // Management Interface
#if (CDC_USE_MANAGEMENT_INTERFACE==true)
          if( Get_class(i)==CDC_COMM_CLASS && Get_protocol(i)==CDC_COMM_V25ter_PROTOCOL)
          {
             cdc_interface_comm = i;      // store interface number
             pipe_cdc_comm_int = Get_ep_pipe(i, 0);
             Host_enable_continuous_in_mode(pipe_cdc_comm_int);
             Host_unfreeze_pipe(pipe_cdc_comm_int);
          }
#endif
        }
      }

      if( Is_host_cdc_configured() )
      {
        if( startup )
        {
           printf("\r\nUSB HOST Communications Device Class demo.\r\n");
           startup=false;
        }

        // Check DATA_PIPE_IN for incoming data
        // ************************************
        // 1 -> UART-USB Mode
        //   Data received is automatically sent on the UART
        // 2 -> Normal Mode
        //   Data received is stored in the "cdc_stream_in_array[CDC_STREAM_IN_SIZE]" array and may be read by firmware
        //   When firmware reads the array it must do it entirely and then set "host_rx_cnt" variable to 0
        //   It must not partially read the array and decrement host_rx_cnt (will lead to array overflow)
        if( (Is_host_in_received(pipe_cdc_data_bulkin)) && (Is_host_stall(pipe_cdc_data_bulkin)==false) )
        {
          Host_reset_pipe_fifo_access(pipe_cdc_data_bulkin);
#if (CDC_USE_UART==true)
          while( Host_byte_count(pipe_cdc_data_bulkin) != 0 )
          {
            usart_putchar(DBG_USART, Host_read_pipe_data(pipe_cdc_data_bulkin, 8) );
            LED_Toggle(LED_HOST_CDC_B0);
          }
          Host_ack_in_received(pipe_cdc_data_bulkin);    // pipe is empty
          Host_free_in(pipe_cdc_data_bulkin);            // ready to receive more data
#else
          while( (host_rx_cnt != CDC_STREAM_IN_SIZE) && (Host_byte_count(pipe_cdc_data_bulkin) != 0) )
          {
            cdc_stream_in_array[host_rx_cnt] = Host_read_pipe_data(pipe_cdc_data_bulkin, 8);
            host_rx_cnt++;
            LED_Toggle(LED_HOST_CDC_B0);
          }
          if( Host_byte_count(pipe_cdc_data_bulkin) == 0 )
          {
            Host_ack_in_received(pipe_cdc_data_bulkin);    // pipe is empty
            Host_free_in(pipe_cdc_data_bulkin);            // ready to receive more data
          }
#endif
        }


        // Check if data to send on DATA_PIPE_OUT state
        // ********************************************
        // Data to be sent is stored in the "cdc_stream_out_array[CDC_STREAM_OUT_SIZE]" array
        // Data may come indifferently from neither UART or RAM (firmware)
        // Data is sent to CDC Device when :
        //    - user requires it, by calling "cdc_pipe_out_usb_flush()" function
        //    - time-out period is elapsed (CDC_NB_MS_BEFORE_FLUSH = number of SOF seen)
        //    - buffer is full (host_tx_cnt = CDC_STREAM_OUT_SIZE)
#if (CDC_USE_UART==true)
        // Check if new byte in USART, to be stored for USB
        if( usart_test_hit(DBG_USART) && (host_tx_cnt != CDC_STREAM_OUT_SIZE) )
        {
          if( USART_SUCCESS==usart_read_char(DBG_USART, &c) )
          {
            cdc_stream_out_array[host_tx_cnt] = c;
            LED_Toggle(LED_HOST_CDC_B1);
            host_tx_cnt++;
          }
          else
            usart_reset_status( DBG_USART );
        }
#endif

        // Check if pipe flush is needed (buffer full or time-out period elapsed)
        if(((sof_cnt>=CDC_NB_MS_BEFORE_FLUSH) && (host_tx_cnt!=0)) || (host_tx_cnt == CDC_STREAM_OUT_SIZE))  //Flush buffer by Timeout
        {
           sof_cnt=0;
           cdc_pipe_out_usb_flush();
        }


        // Check in COMM_PIPE_IN for incoming notifications
        // ************************************************
        if (Is_host_in_received(pipe_cdc_comm_int))
        {
           // Handle here notification messages sent by device
           // Notifications messages have the following structure :
           //  bmRequestType - bNotification - wValue - wIndex - wLength - Data     (wLength is the number of bytes of the Data field)

           //   - NETWORK_CONNECTION : indicates that device has connected to network
           //   - RESPONSE_AVAILABLE : indicates that device has a ready encapsulated response (wait for host request)
           //   - SERIAL_STATE : indicates state of device' UART (errors, carriers and misc. signals)
           //   - etc...

           // ...and now...just coding...
           Host_ack_in_received(pipe_cdc_comm_int);
           Host_free_in(pipe_cdc_comm_int);
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


//! @brief cdc_pipe_out_usb_flush
//!
//! This function sends the data stored in the temporary transmit buffer.
//! This function does nothing if there is no data in the buffer.
//!
void cdc_pipe_out_usb_flush(void)
{
   Host_freeze_pipe(pipe_cdc_data_bulkin);
   if (PIPE_GOOD == host_send_data(pipe_cdc_data_bulkout, host_tx_cnt, cdc_stream_out_array))
     host_tx_cnt = 0;    // if frame not sent, will try again next time (no data loss)

   Host_unfreeze_pipe(pipe_cdc_data_bulkin);
}


//! @brief This function sends a format-specific packet
//!
void host_cdc_send_encapsulated_command(void)
{
  usb_request.bmRequestType = 0x21;
  usb_request.bRequest      = SEND_ENCAPSULATED_COMMAND;
  usb_request.wValue        = 0;
  usb_request.wIndex        = cdc_interface_comm;
  usb_request.wLength       = ENCAPSULATED_PACKET_LENGTH;
  usb_request.incomplete_read = false;
  host_transfer_control(data_stage);
}


//! @brief This function requests a format-specific packet.
//!
//! @note The response is stored into data_stage.
//!
void host_cdc_get_encapsulated_response(void)
{
  usb_request.bmRequestType = 0xA1;
  usb_request.bRequest      = GET_ENCAPSULATED_COMMAND;
  usb_request.wValue        = 0;
  usb_request.wIndex        = cdc_interface_comm;
  usb_request.wLength       = ENCAPSULATED_PACKET_LENGTH;
  usb_request.incomplete_read = false;
  host_transfer_control(data_stage);
}


//! @brief This function set communication settings on the device.
//!
void host_cdc_set_line_coding(void)
{
  usb_request.bmRequestType = 0x21;
  usb_request.bRequest      = SET_LINE_CODING;
  usb_request.wValue        = 0;
  usb_request.wIndex        = cdc_interface_comm;
  usb_request.wLength       = 7;
  usb_request.incomplete_read = false;
  host_transfer_control(data_stage);
}


//! @brief This function requests the communication settings from the device.
//!
//! @note The response is stored into data_stage.
//!
void host_cdc_get_line_coding(void)
{
  usb_request.bmRequestType = 0xA1;
  usb_request.bRequest      = GET_LINE_CODING;
  usb_request.wValue        = 0;
  usb_request.wIndex        = cdc_interface_comm;
  usb_request.wLength       = 7;
  usb_request.incomplete_read = false;
  host_transfer_control(data_stage);
}


#endif  // USB_HOST_FEATURE == true
