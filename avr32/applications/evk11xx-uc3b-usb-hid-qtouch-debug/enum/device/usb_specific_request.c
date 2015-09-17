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


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________


//_____ P R I V A T E   D E C L A R A T I O N S ____________________________

U8 g_u8_report_rate=0;
int jump_bootloader=0;

extern const    void *pbuffer;
extern          U16   data_to_transfer;


//_____ D E C L A R A T I O N S ____________________________________________



//! @brief This function manages hid set idle request.
//!
//! @param u8_report_id    0 the idle rate applies to all input reports, else only applies to the Report ID
//! @param u8_duration     When the upper byte of wValue is 0 (zero), the duration is indefinite else from 0.004 to 1.020 seconds

//!
void usb_hid_set_idle (U8 u8_report_id, U8 u8_duration )
{
   U16 wInterface;

   // Get interface number to put in idle mode
   wInterface=Usb_read_endpoint_data(EP_CONTROL, 16);
   Usb_ack_setup_received_free();

   if( wInterface == INTERFACE_NB )
     g_u8_report_rate = u8_duration;

   Usb_ack_control_in_ready_send();
   while (!Is_usb_control_in_ready());
}


//! @brief This function manages hid get idle request.
//!
//! @param u8_report_id    0 the idle rate applies to all input reports, else only applies to the Report ID
//!
void usb_hid_get_idle (U8 u8_report_id)
{
   U16 wLength;
   U16 wInterface;

   // Get interface number to put in idle mode
   wInterface=Usb_read_endpoint_data(EP_CONTROL, 16);
   wLength   =Usb_read_endpoint_data(EP_CONTROL, 16);
   Usb_ack_setup_received_free();

   if( (wLength != 0) && (wInterface == INTERFACE_NB) )
   {
      Usb_write_endpoint_data(EP_CONTROL, 8, g_u8_report_rate);
      Usb_ack_control_in_ready_send();
   }

   while (!Is_usb_control_out_received());
   Usb_ack_control_out_received_free();
}


//! This function manages the HID Get_Descriptor request.
//!
static void hid_get_descriptor(U8 size_of_report, const U8* p_usb_hid_report)
{
  bool  zlp;
  U16   wIndex;
  U16   wLength;

  zlp = false;                                              /* no zero length packet */

  data_to_transfer = size_of_report;
  pbuffer          = p_usb_hid_report;

  wIndex = Usb_read_endpoint_data(EP_CONTROL, 16);
  wIndex = usb_format_usb_to_mcu_data(16, wIndex);
  wLength = Usb_read_endpoint_data(EP_CONTROL, 16);
  wLength = usb_format_usb_to_mcu_data(16, wLength);
  Usb_ack_setup_received_free();                          //!< clear the setup received flag

  if (wLength > data_to_transfer)
  {
    zlp = !(data_to_transfer % EP_CONTROL_LENGTH);  //!< zero length packet condition
  }
  else
  {
    data_to_transfer = wLength; //!< send only requested number of data bytes
  }

  Usb_ack_nak_out(EP_CONTROL);

  while (data_to_transfer && (!Is_usb_nak_out(EP_CONTROL)))
  {
    while( !Is_usb_control_in_ready() && !Is_usb_nak_out(EP_CONTROL) );

    if( Is_usb_nak_out(EP_CONTROL) )
       break;    // don't clear the flag now, it will be cleared after

    Usb_reset_endpoint_fifo_access(EP_CONTROL);
    data_to_transfer = usb_write_ep_txpacket(EP_CONTROL, pbuffer,
                                             data_to_transfer, &pbuffer);
    if( Is_usb_nak_out(EP_CONTROL) )
       break;
    else
       Usb_ack_control_in_ready_send();  //!< Send data until necessary
  }

  if ( zlp && (!Is_usb_nak_out(EP_CONTROL)) )
  {
    while (!Is_usb_control_in_ready());
    Usb_ack_control_in_ready_send();
  }

  while (!(Is_usb_nak_out(EP_CONTROL)));
  Usb_ack_nak_out(EP_CONTROL);
  while (!Is_usb_control_out_received());
  Usb_ack_control_out_received_free();
}


//! @brief This function configures the endpoints of the device application.
//! This function is called when the set configuration request has been received.
//!
void usb_user_endpoint_init(U8 conf_nb)
{

  (void)Usb_configure_endpoint(EP_HID_GENERIC_IN,
                               EP_ATTRIBUTES_1,
                               DIRECTION_IN,
                               EP_SIZE_1,
                               SINGLE_BANK);

   (void)Usb_configure_endpoint(EP_HID_GENERIC_OUT,
                               EP_ATTRIBUTES_2,
                               DIRECTION_OUT,
                               EP_SIZE_2,
                               SINGLE_BANK);


}


//! This function is called by the standard USB read request function when
//! the USB request is not supported. This function returns true when the
//! request is processed. This function returns false if the request is not
//! supported. In this case, a STALL handshake will be automatically
//! sent by the standard USB read request function.
//!
bool usb_user_read_request(U8 type, U8 request)
{
   U8    wValue_msb;
   U8    wValue_lsb;

   // Read wValue
   wValue_lsb = Usb_read_endpoint_data(EP_CONTROL, 8);
   wValue_msb = Usb_read_endpoint_data(EP_CONTROL, 8);
/*
   U8 descriptor_type;

  Usb_read_endpoint_data(EP_CONTROL, 8); // string_type
  descriptor_type = Usb_read_endpoint_data(EP_CONTROL, 8);
*/

   //** Specific request from Class HID
   if( 0x81 == type )   // USB_SETUP_GET_STAND_INTERFACE
   {
      switch( request )
      {
         case GET_DESCRIPTOR:
         switch( wValue_msb ) // Descriptor ID
         {
#if (USB_HIGH_SPEED_SUPPORT==false)
            case HID_DESCRIPTOR:
            hid_get_descriptor(
               sizeof(usb_conf_desc_fs.hid)
            ,  (const U8*)&usb_conf_desc_fs.hid);
            return true;
#else
            case HID_DESCRIPTOR:
            if( Is_usb_full_speed_mode() )
            {
               hid_get_descriptor(
                  sizeof(usb_conf_desc_fs.hid)
               ,  (const U8*)&usb_conf_desc_fs.hid);
            }else{
               hid_get_descriptor(
                  sizeof(usb_conf_desc_hs.hid_mouse)
               ,  (const U8*)&usb_conf_desc_hs.hid);
            }
            return true;
#endif

            case HID_REPORT_DESCRIPTOR:
            hid_get_descriptor(
               sizeof(usb_hid_report_descriptor)
            ,  usb_hid_report_descriptor);
            return true;

            case HID_PHYSICAL_DESCRIPTOR:
            // TODO
            break;
         }
         break;
      }
   }
   if( 0x21 == type ) // USB_SETUP_SET_CLASS_INTER
   {
      switch( request )
      {
         case HID_SET_REPORT:
         // The MSB wValue field specifies the Report Type
         // The LSB wValue field specifies the Report ID
         switch (wValue_msb)
         {
            case HID_REPORT_INPUT:
            // TODO
            break;

            case HID_REPORT_OUTPUT:
            Usb_ack_setup_received_free();
            while (!Is_usb_control_out_received());
            Usb_ack_control_out_received_free();
            Usb_ack_control_in_ready_send();
            while (!Is_usb_control_in_ready());
            return true;

            case HID_REPORT_FEATURE:
            usb_hid_set_report_feature();
            return true;
            break;
         }
         break;

         case HID_SET_IDLE:
         usb_hid_set_idle(wValue_lsb, wValue_msb);
         return true;

         case HID_SET_PROTOCOL:
         // TODO
         break;
      }
   }
   if( 0xA1 == type ) // USB_SETUP_GET_CLASS_INTER
   {
      switch( request )
      {
         case HID_GET_REPORT:
         // TODO
         break;
         case HID_GET_IDLE:
         usb_hid_get_idle(wValue_lsb);
         return true;
         case HID_GET_PROTOCOL:
         // TODO
         break;
      }
   }
   return false;  // No supported request
}


//! This function returns the size and the pointer on a user information
//! structure
//!
bool usb_user_get_descriptor(U8 type, U8 string)
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

void usb_hid_set_report_feature(void)
{

   Usb_ack_setup_received_free();
   Usb_ack_control_in_ready_send();// send a ZLP

   while(!Is_usb_control_out_received());

   if(Usb_read_endpoint_data(EP_CONTROL, 8)==0x55)
      if(Usb_read_endpoint_data(EP_CONTROL, 8)==0xAA)
         if(Usb_read_endpoint_data(EP_CONTROL, 8)==0x55)
            if(Usb_read_endpoint_data(EP_CONTROL, 8)==0xAA)
            {
               jump_bootloader=1; // Specific Request with 0x55AA55AA code
            }
   Usb_ack_control_out_received_free();
   Usb_ack_control_in_ready_send();    //!< send a ZLP for STATUS phase
   while(!Is_usb_control_in_ready());
}

#endif  // USB_DEVICE_FEATURE == true
