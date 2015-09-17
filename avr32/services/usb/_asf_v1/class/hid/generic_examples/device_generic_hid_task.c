/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB device generic HID task.
 *
 * This file manages the USB device generic HID task.
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
#include "usb_drv.h"
#include "gpio.h"
#include "joystick.h"
#include "usb_descriptors.h"
#include "usb_standard_request.h"
#include "device_generic_hid_task.h"

#include <string.h>


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________



//_____ D E C L A R A T I O N S ____________________________________________

static uint16_t  sof_cnt;

static void hid_report_out  (void);
static void hid_report_in   (void);

#define RXBUF_SIZE  8
#define TXBUF_SIZE  8
char rxbuf[RXBUF_SIZE];
char txbuf[TXBUF_SIZE];

extern int jump_bootloader;
int        last_joy = 0;

//!
//! @brief This function initializes the hardware/software resources
//! required for device generic HID task.
//!
void device_generic_hid_task_init(void)
{
  sof_cnt = 0;

#ifndef FREERTOS_USED
  #if USB_HOST_FEATURE == true
  // If both device and host features are enabled, check if device mode is engaged
  // (accessing the USB registers of a non-engaged mode, even with load operations,
  // may corrupt USB FIFO data).
  if (Is_usb_device())
  #endif  // USB_HOST_FEATURE == true
    Usb_enable_sof_interrupt();
#endif  // FREERTOS_USED

}

//! @brief Entry point of the HID generic communication task
//! This function manages IN/OUT report management.
//!
void device_generic_hid_task(void)
{
   if(!Is_device_enumerated())          // Check USB HID is enumerated
      return;

   hid_report_out();
   hid_report_in();
}


//! @brief Get data report from Host
//!
void hid_report_out(void)
{
  int led_number;
  int led_state;

   if(Is_usb_out_received(EP_HID_GENERIC_OUT))
   {
      Usb_reset_endpoint_fifo_access(EP_HID_GENERIC_OUT);
      memset(rxbuf, 0, RXBUF_SIZE);
      usb_read_ep_rxpacket(EP_HID_GENERIC_OUT, &rxbuf, RXBUF_SIZE, NULL);
      Usb_ack_out_received_free(EP_HID_GENERIC_OUT);
      // The Data has been read and stored in rxbuf

      led_state      = rxbuf[0]&0x0F; // RepportOUT[0] is LEDS value
      led_number     = rxbuf[1]&0x0F;
      switch (led_number)
      {
         case 1:
            if(led_state)
            {   gpio_clr_gpio_pin(LED0_GPIO);   }
            else { gpio_set_gpio_pin(LED0_GPIO);}
            break;
         case 2:
            if(led_state)
            {   gpio_clr_gpio_pin(LED1_GPIO);   }
            else { gpio_set_gpio_pin(LED1_GPIO);}
            break;
         case 3:
            if(led_state)
            {   gpio_clr_gpio_pin(LED2_GPIO);   }
            else { gpio_set_gpio_pin(LED2_GPIO);}
            break;
         case 4:
            if(led_state)
            {   gpio_clr_gpio_pin(LED3_GPIO);   }
            else { gpio_set_gpio_pin(LED3_GPIO);}
            break;
      }

   }

   //** Check if we received DFU mode command from host
   if(jump_bootloader)
   {
      uint32_t volatile tempo;
      gpio_set_gpio_pin(LED0_GPIO);
      gpio_set_gpio_pin(LED1_GPIO);
      gpio_set_gpio_pin(LED2_GPIO);
      gpio_set_gpio_pin(LED3_GPIO);
      Usb_detach();                          // Detach actual generic HID application
      for(tempo=0;tempo<70000;tempo++);      // Wait some time before
      //start_boot();                          // Jumping to bootloader
      while(1);
   }
}


//! @brief Send data report to Host
//!
void hid_report_in(void)
{

  int joy   =0;
  char* ptr_cram=txbuf;
  int data_to_transfer=0;

  if(!Is_usb_write_enabled(EP_HID_GENERIC_IN))
      return;                                // Not ready to send report

  // Build the Joystick report
  if(is_joystick_up()|| is_joystick_down() || is_joystick_right() || is_joystick_left() )                     //! Check for UP event
  {
    joy=0x01;
  }
  if(joy==last_joy) {
     return;                                // Same report then no send report
  }

  last_joy=joy;
  memset(txbuf, 0, RXBUF_SIZE);
  txbuf[0] = joy;

  // Send report
  data_to_transfer = sizeof(txbuf);
  while (data_to_transfer)
  {
       while (!Is_usb_in_ready(EP_HID_GENERIC_IN));

       Usb_reset_endpoint_fifo_access(EP_HID_GENERIC_IN);
       data_to_transfer = usb_write_ep_txpacket(EP_HID_GENERIC_IN, ptr_cram, data_to_transfer, (const void**)&ptr_cram);
       Usb_ack_in_ready_send(EP_HID_GENERIC_IN);
   }
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
