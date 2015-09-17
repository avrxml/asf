/**************************************************************************
 *
 * \file
 *
 * \brief Processing of USB device specific enumeration requests.
 *
 * This file contains the specific request decoding for enumeration process.
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


//_____ I N C L U D E S ____________________________________________________

#include "conf_usb.h"


#if USB_DEVICE_FEATURE == true

#include "usb_drv.h"
#include "usb_descriptors.h"
#include "usb_standard_request.h"
#include "usb_specific_request.h"
#include "ctrl_access.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

volatile bool ms_multiple_drive;


//_____ P R I V A T E   D E C L A R A T I O N S ____________________________

extern const  void *pbuffer;
extern        uint16_t   data_to_transfer;


//_____ D E C L A R A T I O N S ____________________________________________

//! @brief This function configures the endpoints of the device application.
//! This function is called when the set configuration request has been received.
//!
void usb_user_endpoint_init(uint8_t conf_nb)
{
  ms_multiple_drive = false;

#if (USB_HIGH_SPEED_SUPPORT==true)
   if( !Is_usb_full_speed_mode() )
   {
  (void)Usb_configure_endpoint(EP_MS_IN,
                               EP_ATTRIBUTES_1,
                               DIRECTION_IN,
                               EP_SIZE_1_HS,
                               DOUBLE_BANK);

  (void)Usb_configure_endpoint(EP_MS_OUT,
                               EP_ATTRIBUTES_2,
                               DIRECTION_OUT,
                               EP_SIZE_2_HS,
                               DOUBLE_BANK);
      return;
   }
#endif
  (void)Usb_configure_endpoint(EP_MS_IN,
                               EP_ATTRIBUTES_1,
                               DIRECTION_IN,
                               EP_SIZE_1_FS,
                               DOUBLE_BANK);

  (void)Usb_configure_endpoint(EP_MS_OUT,
                               EP_ATTRIBUTES_2,
                               DIRECTION_OUT,
                               EP_SIZE_2_FS,
                               DOUBLE_BANK);
}


//! This function is called by the standard USB read request function when
//! the USB request is not supported. This function returns true when the
//! request is processed. This function returns false if the request is not
//! supported. In this case, a STALL handshake will be automatically
//! sent by the standard USB read request function.
//!
bool usb_user_read_request(uint8_t type, uint8_t request)
{
   uint16_t wInterface;
   uint8_t wValue_msb;
   uint8_t wValue_lsb;

   wValue_lsb = Usb_read_endpoint_data(EP_CONTROL, 8);
   wValue_msb = Usb_read_endpoint_data(EP_CONTROL, 8);

   //** Specific request from Class MassStorage
   if( USB_SETUP_SET_CLASS_INTER == type )
   {
      switch( request )
      {
         case MASS_STORAGE_RESET:
         // wValue must be 0
         // wIndex = Interface
         if( (0!=wValue_lsb) || (0!=wValue_msb) )
            break;
         wInterface=Usb_read_endpoint_data(EP_CONTROL, 16);
         if( INTERFACE_NB != wInterface )
            break;
         Usb_ack_setup_received_free();
         Usb_ack_control_in_ready_send();
         return true;
      }
   }
   if( USB_SETUP_GET_CLASS_INTER == type )
   {
      switch( request )
      {
         case GET_MAX_LUN:
         // wValue must be 0
         // wIndex = Interface
         if( (0!=wValue_lsb) || (0!=wValue_msb) )
            break;
         wInterface=Usb_read_endpoint_data(EP_CONTROL, 16);
         if( INTERFACE_NB != wInterface )
            break;
         Usb_ack_setup_received_free();
         Usb_reset_endpoint_fifo_access(EP_CONTROL);
         Usb_write_endpoint_data(EP_CONTROL, 8, get_nb_lun() - 1);
         Usb_ack_control_in_ready_send();
         while (!Is_usb_control_in_ready());

         while(!Is_usb_control_out_received());
         Usb_ack_control_out_received_free();

         ms_multiple_drive = true;
         return true;
      }
   }

   return false;
}


//! This function returns the size and the pointer on a user information
//! structure
//!
bool usb_user_get_descriptor(uint8_t type, uint8_t string)
{
  pbuffer = NULL;

  switch (type)
  {
  case STRING_DESCRIPTOR:
    switch (string)
    {
    case LANG_ID:
      data_to_transfer = sizeof(usb_user_language_id);
      pbuffer = &usb_user_language_id;
      break;

    case MAN_INDEX:
      data_to_transfer = sizeof(usb_user_manufacturer_string_descriptor);
      pbuffer = &usb_user_manufacturer_string_descriptor;
      break;

    case PROD_INDEX:
      data_to_transfer = sizeof(usb_user_product_string_descriptor);
      pbuffer = &usb_user_product_string_descriptor;
      break;

    case SN_INDEX:
      data_to_transfer = sizeof(usb_user_serial_number);
      pbuffer = &usb_user_serial_number;
      break;

    default:
      break;
    }
    break;

  default:
    break;
  }

  return pbuffer != NULL;
}


#endif  // USB_DEVICE_FEATURE == true
