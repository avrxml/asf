/**************************************************************************
 *
 * \file
 *
 * \brief Processing of USB device enumeration requests.
 *
 * This file contains the USB control endpoint management
 * routines corresponding to the standard enumeration process (refer to
 * chapter 9 of the USB specification).
 * This file calls routines of the usb_specific_request.c file for
 * non-standard request management.
 * The enumeration parameters (descriptor tables) are contained in the
 * usb_descriptors.c file.
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
#include "usb_task.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________
#if(!defined USB_REMOTE_WAKEUP_FEATURE)
#define USB_REMOTE_WAKEUP_FEATURE false
#endif

//_____ P R I V A T E   D E C L A R A T I O N S ____________________________

static  void    usb_get_descriptor   (void);
static  void    usb_set_address      (void);
static  void    usb_set_configuration(void);
static  void    usb_clear_feature    (void);
static  void    usb_set_feature      (void);
static  void    usb_get_status       (void);
static  void    usb_get_configuration(void);
static  bool    usb_get_interface    (void);
static  void    usb_set_interface    (void);


//_____ D E C L A R A T I O N S ____________________________________________

        const     void                               *pbuffer;
                  U16                                 data_to_transfer;
static            U8                                  bmRequestType;
        volatile  U8                                  usb_configuration_nb;
extern  volatile  bool                                usb_connected;
extern  const     S_usb_device_descriptor             usb_user_device_descriptor;
extern  const     S_usb_user_configuration_descriptor usb_user_configuration_descriptor;
static            U8                                  usb_interface_status[NB_INTERFACE];  // All interface with default setting
static            U8                                  device_status = DEVICE_STATUS;
                  U8                                  remote_wakeup_feature;

//! This function reads the SETUP request sent to the default control endpoint
//! and calls the appropriate function. When exiting of the usb_read_request
//! function, the device is ready to manage the next request.
//!
//! If the received request is not supported or a non-standard USB request, the function
//! will call the custom decoding function in usb_specific_request module.
//!
//! @note List of supported requests:
//! GET_DESCRIPTOR
//! GET_CONFIGURATION
//! SET_ADDRESS
//! SET_CONFIGURATION
//! CLEAR_FEATURE
//! SET_FEATURE
//! GET_STATUS
//!
void usb_process_request(void)
{
  U8 bRequest;

  Usb_reset_endpoint_fifo_access(EP_CONTROL);
  bmRequestType = Usb_read_endpoint_data(EP_CONTROL, 8);
  bRequest      = Usb_read_endpoint_data(EP_CONTROL, 8);

  switch (bRequest)
  {
  case GET_DESCRIPTOR:
    if (bmRequestType == 0x80) usb_get_descriptor();
    else goto unsupported_request;
    break;

  case GET_CONFIGURATION:
    if (bmRequestType == 0x80) usb_get_configuration();
    else goto unsupported_request;
    break;

  case SET_ADDRESS:
    if (bmRequestType == 0x00) usb_set_address();
    else goto unsupported_request;
    break;

  case SET_CONFIGURATION:
    if (bmRequestType == 0x00) usb_set_configuration();
    else goto unsupported_request;
    break;

  case CLEAR_FEATURE:
    if (bmRequestType <= 0x02) usb_clear_feature();
    else goto unsupported_request;
    break;

  case SET_FEATURE:
    if (bmRequestType <= 0x02) usb_set_feature();
    else goto unsupported_request;
    break;

  case GET_STATUS:
    if (0x7F < bmRequestType && bmRequestType <= 0x82) usb_get_status();
    else goto unsupported_request;
    break;

  case GET_INTERFACE:
    if (bmRequestType == 0x81)
    {
      if(!usb_get_interface())
      {
        Usb_enable_stall_handshake(EP_CONTROL);
        Usb_ack_setup_received_free();
      }
    }
    else goto unsupported_request;
    break;

  case SET_INTERFACE:
    if (bmRequestType == 0x01) usb_set_interface();
    else goto unsupported_request;
    break;

  case SET_DESCRIPTOR:
  case SYNCH_FRAME:
  default:  //!< unsupported request => call to user read request
unsupported_request:
    if (!usb_user_read_request(bmRequestType, bRequest))
    {
      Usb_enable_stall_handshake(EP_CONTROL);
      Usb_ack_setup_received_free();
    }
    break;
  }
}


//! This function manages the SET ADDRESS request. When complete, the device
//! will filter the requests using the new address.
//!
void usb_set_address(void)
{
  U8 addr = Usb_read_endpoint_data(EP_CONTROL, 8);
  Usb_configure_address(addr);

  Usb_ack_setup_received_free();

  Usb_ack_control_in_ready_send();    //!< send a ZLP for STATUS phase
  while (!Is_usb_control_in_ready()); //!< waits for status phase done
                                      //!< before using the new address
  Usb_enable_address();
}


//! This function manages the SET CONFIGURATION request. If the selected
//! configuration is valid, this function call the usb_user_endpoint_init()
//! function that will configure the endpoints following the configuration
//! number.
//!
void usb_set_configuration(void)
{
  U8 configuration_number = Usb_read_endpoint_data(EP_CONTROL, 8);
  U8 u8_i;

  if (configuration_number <= NB_CONFIGURATION)
  {
    Usb_ack_setup_received_free();
    usb_configuration_nb = configuration_number;
    for( u8_i=0; u8_i<NB_INTERFACE; u8_i++) usb_interface_status[u8_i]=0;

    usb_user_endpoint_init(usb_configuration_nb); //!< endpoint configuration
    Usb_set_configuration_action();

    Usb_ack_control_in_ready_send();              //!< send a ZLP for STATUS phase
  }
  else
  {
    //!< keep that order (set StallRq/clear RxSetup) or a
    //!< OUT request following the SETUP may be acknowledged
    Usb_enable_stall_handshake(EP_CONTROL);
    Usb_ack_setup_received_free();
  }
}


//! This function manages the GET DESCRIPTOR request. The device descriptor,
//! the configuration descriptor and the device qualifier are supported. All
//! other descriptors must be supported by the usb_user_get_descriptor
//! function.
//! Only 1 configuration is supported.
//!
void usb_get_descriptor(void)
{
  bool    zlp;
  U16     wLength;
  U8      descriptor_type;
  U8      string_type;
  Union32 temp;
#if (USB_HIGH_SPEED_SUPPORT==true)
  bool    b_first_data = true;
#endif

  zlp             = false;                                  /* no zero length packet */
  string_type     = Usb_read_endpoint_data(EP_CONTROL, 8);  /* read LSB of wValue    */
  descriptor_type = Usb_read_endpoint_data(EP_CONTROL, 8);  /* read MSB of wValue    */

  switch (descriptor_type)
  {
  case DEVICE_DESCRIPTOR:
    data_to_transfer = Usb_get_dev_desc_length();   //!< sizeof(usb_dev_desc);
    pbuffer          = Usb_get_dev_desc_pointer();
    break;

#if (USB_HIGH_SPEED_SUPPORT==false)
  case CONFIGURATION_DESCRIPTOR:
    data_to_transfer = Usb_get_conf_desc_length();  //!< sizeof(usb_conf_desc);
    pbuffer          = Usb_get_conf_desc_pointer();
    break;

#else
  case CONFIGURATION_DESCRIPTOR:
    if( Is_usb_full_speed_mode() )
    {
       data_to_transfer = Usb_get_conf_desc_fs_length();  //!< sizeof(usb_conf_desc_fs);
       pbuffer          = Usb_get_conf_desc_fs_pointer();
    }else{
       data_to_transfer = Usb_get_conf_desc_hs_length();  //!< sizeof(usb_conf_desc_hs);
       pbuffer          = Usb_get_conf_desc_hs_pointer();
    }
    break;

  case OTHER_SPEED_CONFIGURATION_DESCRIPTOR:
    if( !Is_usb_full_speed_mode() )
    {
       data_to_transfer = Usb_get_conf_desc_fs_length();  //!< sizeof(usb_conf_desc_fs);
       pbuffer          = Usb_get_conf_desc_fs_pointer();
    }else{
       data_to_transfer = Usb_get_conf_desc_hs_length();  //!< sizeof(usb_conf_desc_hs);
       pbuffer          = Usb_get_conf_desc_hs_pointer();
    }
    break;

  case DEVICE_QUALIFIER_DESCRIPTOR:
    data_to_transfer = Usb_get_qualifier_desc_length();  //!< sizeof(usb_qualifier_desc);
    pbuffer          = Usb_get_qualifier_desc_pointer();
    break;

#endif

  default:
    if (!usb_user_get_descriptor(descriptor_type, string_type))
    {
      Usb_enable_stall_handshake(EP_CONTROL);
      Usb_ack_setup_received_free();
      return;
    }
    break;
  }

  temp.u32 = Usb_read_endpoint_data(EP_CONTROL, 32);      //!< read wIndex and wLength with a 32-bit access
                                                          //!< since this access is aligned with a 32-bit
                                                          //!< boundary from the beginning of the endpoint
  wLength = usb_format_usb_to_mcu_data(16, temp.u16[1]);  //!< ignore wIndex, keep and format wLength
  Usb_ack_setup_received_free();                          //!< clear the setup received flag

  if (wLength > data_to_transfer)
  {
    zlp = !(data_to_transfer % EP_CONTROL_LENGTH);  //!< zero length packet condition
  }
  else
  {
    // No need to test ZLP sending since we send the exact number of bytes as
    // expected by the host.
    data_to_transfer = wLength; //!< send only requested number of data bytes
  }

  Usb_ack_nak_out(EP_CONTROL);

  while (data_to_transfer && !Is_usb_nak_out(EP_CONTROL))
  {
    while (!Is_usb_control_in_ready() && !Is_usb_nak_out(EP_CONTROL));

    if (Is_usb_nak_out(EP_CONTROL))
      break;  // don't clear the flag now, it will be cleared after

    Usb_reset_endpoint_fifo_access(EP_CONTROL);

#if (USB_HIGH_SPEED_SUPPORT==true) // To support other descriptors like OTHER_SPEED_CONFIGURATION_DESCRIPTOR
    if( b_first_data ) {
      b_first_data = false;
      if( 0!= data_to_transfer ) {
        usb_write_ep_txpacket(EP_CONTROL, pbuffer, 1, &pbuffer);
        data_to_transfer--;
      }
      if( 0!= data_to_transfer ) {
        usb_write_ep_txpacket(EP_CONTROL, &descriptor_type, 1, NULL);
        pbuffer = ((const U8*)pbuffer)+1;
        data_to_transfer--;
      }
    }
#endif
    if( 0!= data_to_transfer ) {
       data_to_transfer = usb_write_ep_txpacket(EP_CONTROL, pbuffer,
                                                data_to_transfer, &pbuffer);
    }
    if (Is_usb_nak_out(EP_CONTROL))
      break;

    Usb_ack_control_in_ready_send();  //!< Send data until necessary
  }

  if (zlp && !Is_usb_nak_out(EP_CONTROL))
  {
    while (!Is_usb_control_in_ready());
    Usb_ack_control_in_ready_send();
  }

  while (!Is_usb_nak_out(EP_CONTROL));
  Usb_ack_nak_out(EP_CONTROL);
  while (!Is_usb_control_out_received());
  Usb_ack_control_out_received_free();
}


//! This function manages the GET CONFIGURATION request. The current
//! configuration number is returned.
//!
void usb_get_configuration(void)
{
  Usb_ack_setup_received_free();

  Usb_reset_endpoint_fifo_access(EP_CONTROL);
  Usb_write_endpoint_data(EP_CONTROL, 8, usb_configuration_nb);
  Usb_ack_control_in_ready_send();

  while (!Is_usb_control_out_received());
  Usb_ack_control_out_received_free();
}


//! This function manages the GET STATUS request. The device, interface or
//! endpoint status is returned.
//!
void usb_get_status(void)
{
  U8 wIndex;

  switch (bmRequestType)
  {
  case REQUEST_DEVICE_STATUS:
    Usb_ack_setup_received_free();
    Usb_reset_endpoint_fifo_access(EP_CONTROL);
    Usb_write_endpoint_data(EP_CONTROL, 8, device_status);
    break;

  case REQUEST_INTERFACE_STATUS:
    Usb_ack_setup_received_free();
    Usb_reset_endpoint_fifo_access(EP_CONTROL);
    Usb_write_endpoint_data(EP_CONTROL, 8, INTERFACE_STATUS);
    break;

  case REQUEST_ENDPOINT_STATUS:
    Usb_read_endpoint_data(EP_CONTROL, 16); //!< dummy read (wValue)
    wIndex = Usb_read_endpoint_data(EP_CONTROL, 8);
    wIndex = Get_desc_ep_nbr(wIndex);
    Usb_ack_setup_received_free();
    Usb_reset_endpoint_fifo_access(EP_CONTROL);
    Usb_write_endpoint_data(EP_CONTROL, 8, Is_usb_endpoint_stall_requested(wIndex) );
    break;

  default:
    Usb_enable_stall_handshake(EP_CONTROL);
    Usb_ack_setup_received_free();
    return;
  }

  Usb_write_endpoint_data(EP_CONTROL, 8, 0x00);
  Usb_ack_control_in_ready_send();

  while (!Is_usb_control_out_received());
  Usb_ack_control_out_received_free();
}


//! This function manages the SET FEATURE request. The USB test modes are
//! supported by this function.
//!
void usb_set_feature(void)
{
  U16 wValue  = usb_format_usb_to_mcu_data(16, Usb_read_endpoint_data(EP_CONTROL, 16));
  U16 wIndex  = usb_format_usb_to_mcu_data(16, Usb_read_endpoint_data(EP_CONTROL, 16));
  U16 wLength = usb_format_usb_to_mcu_data(16, Usb_read_endpoint_data(EP_CONTROL, 16));

  if (wLength)
    goto unsupported_request;

   if (bmRequestType==USB_SETUP_SET_STAND_DEVICE) {
#if (USB_REMOTE_WAKEUP_FEATURE == true)
      if (FEATURE_DEVICE_REMOTE_WAKEUP == wValue)
      {
         device_status |= USB_DEV_STATUS_REMOTEWAKEUP;
         remote_wakeup_feature = true;
         Usb_ack_setup_received_free();
         Usb_ack_control_in_ready_send();
         return;
      }
#endif
      goto unsupported_request;
   }

  switch (wValue)
  {
  case FEATURE_ENDPOINT_HALT:
    wIndex = Get_desc_ep_nbr(wIndex);  // clear direction flag
    if (bmRequestType != ENDPOINT_TYPE ||
        wIndex == EP_CONTROL ||
        !Is_usb_endpoint_enabled(wIndex))
      goto unsupported_request;

    Usb_enable_stall_handshake(wIndex);
    Usb_ack_setup_received_free();
    Usb_ack_control_in_ready_send();
    break;

#if (USB_HIGH_SPEED_SUPPORT==true)
  case FEATURE_TEST_MODE:
    if (bmRequestType != DEVICE_TYPE ||
        wIndex & 0x00FF)
      goto unsupported_request;

    switch (wIndex >> 8)
    {
    case TEST_J:
      Usb_ack_setup_received_free();
      Usb_ack_control_in_ready_send();
      while (!Is_usb_control_in_ready());
      Wr_bitfield(AVR32_USBB_udcon, AVR32_USBB_UDCON_SPDCONF_MASK, 2);
      Set_bits(AVR32_USBB_udcon, AVR32_USBB_UDCON_TSTJ_MASK);
      break;

    case TEST_K:
      Usb_ack_setup_received_free();
      Usb_ack_control_in_ready_send();
      while (!Is_usb_control_in_ready());
      Wr_bitfield(AVR32_USBB_udcon, AVR32_USBB_UDCON_SPDCONF_MASK, 2);
      Set_bits(AVR32_USBB_udcon, AVR32_USBB_UDCON_TSTK_MASK);
      break;

    case TEST_SE0_NAK:
      Usb_ack_setup_received_free();
      Usb_ack_control_in_ready_send();
      while (!Is_usb_control_in_ready());
      Wr_bitfield(AVR32_USBB_udcon, AVR32_USBB_UDCON_SPDCONF_MASK, 2);
      break;

    case TEST_PACKET:
      {
        static const U8 test_packet[] =
        {
          // 00000000 * 9
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          // 01010101 * 8
          0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
          // 01110111 * 8
          0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
          // 0, {111111S * 15}, 111111
          0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          // S, 111111S, {0111111S * 7}
          0x7F, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB, 0xFD,
          // 00111111, {S0111111 * 9}, S0
          0xFC, 0x7E, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB, 0xFD, 0x7E
        };

        Usb_ack_setup_received_free();
        Usb_ack_control_in_ready_send();
        while (!Is_usb_control_in_ready());
        Wr_bitfield(AVR32_USBB_udcon, AVR32_USBB_UDCON_SPDCONF_MASK, 2);
        Usb_disable_endpoint(EP_CONTROL);
        Usb_unallocate_memory(EP_CONTROL);
        (void)Usb_configure_endpoint(EP_CONTROL,
                                     TYPE_BULK,
                                     DIRECTION_IN,
                                     64,
                                     SINGLE_BANK);
        Usb_reset_endpoint(EP_CONTROL);
        Set_bits(AVR32_USBB_udcon, AVR32_USBB_UDCON_TSTPCKT_MASK);
        usb_write_ep_txpacket(EP_CONTROL, &test_packet, sizeof(test_packet), NULL);
        Usb_send_in(EP_CONTROL);
      }
      break;

    case TEST_FORCE_ENABLE: // Only for downstream facing hub ports
    default:
      goto unsupported_request;
    }
    break;
#endif

  case FEATURE_DEVICE_REMOTE_WAKEUP:
  default:
    goto unsupported_request;
  }

  return;

unsupported_request:
  Usb_enable_stall_handshake(EP_CONTROL);
  Usb_ack_setup_received_free();
}


//! This function manages the CLEAR FEATURE request.
//!
void usb_clear_feature(void)
{
   U8 wValue;
   U8 wIndex;

   switch (bmRequestType)
   {
#if (USB_REMOTE_WAKEUP_FEATURE == true)
      case  USB_SETUP_SET_STAND_DEVICE:
      wValue = Usb_read_endpoint_data(EP_CONTROL, 8);
      if (wValue != FEATURE_DEVICE_REMOTE_WAKEUP)
         break;              // Invalid request
      device_status &= ~USB_DEV_STATUS_REMOTEWAKEUP;
      remote_wakeup_feature = false;
      Usb_ack_setup_received_free();
      Usb_ack_control_in_ready_send();
      return;
#endif

      case USB_SETUP_SET_STAND_INTERFACE:
      break;

      case USB_SETUP_SET_STAND_ENDPOINT:
      wValue = Usb_read_endpoint_data(EP_CONTROL, 8);
      if (wValue != FEATURE_ENDPOINT_HALT)
         break;
      Usb_read_endpoint_data(EP_CONTROL, 8);  //!< dummy read (MSB of wValue)
      wIndex = Usb_read_endpoint_data(EP_CONTROL, 8);
      wIndex = Get_desc_ep_nbr(wIndex);

      if (!Is_usb_endpoint_enabled(wIndex))
         break;
      if (wIndex != EP_CONTROL)
      {
        Usb_disable_stall_handshake(wIndex);
        Usb_reset_endpoint(wIndex);
        Usb_reset_data_toggle(wIndex);
      }
      Usb_ack_setup_received_free();
      Usb_ack_control_in_ready_send();
      return;

      default:
      break;
   }
   Usb_enable_stall_handshake(EP_CONTROL);
   Usb_ack_setup_received_free();
}

//! This function manages the SETUP_GET_INTERFACE request.
//!
bool usb_get_interface (void)
{
   U16   wInterface;
   U16   wValue;

   // Read wValue
   wValue = usb_format_usb_to_mcu_data(16, Usb_read_endpoint_data(EP_CONTROL, 16));
   // wValue = Alternate Setting
   // wIndex = Interface
   wInterface=usb_format_usb_to_mcu_data(16, Usb_read_endpoint_data(EP_CONTROL, 16));
   if(0!=wValue)
      return false;
   Usb_ack_setup_received_free();

   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   Usb_write_endpoint_data(EP_CONTROL, 8, usb_interface_status[wInterface] );
   Usb_ack_control_in_ready_send();

   while( !Is_usb_control_out_received() );
   Usb_ack_control_out_received_free();
   return true;
}


//! This function manages the SET INTERFACE request.
//!
void usb_set_interface(void)
{
   U8 u8_i;

   // wValue = Alternate Setting
   // wIndex = Interface
   U16 wValue  = usb_format_usb_to_mcu_data(16, Usb_read_endpoint_data(EP_CONTROL, 16));
   U16 wIndex  = usb_format_usb_to_mcu_data(16, Usb_read_endpoint_data(EP_CONTROL, 16));
   Usb_ack_setup_received_free();

   // Get descriptor
#if (USB_HIGH_SPEED_SUPPORT==true)
   if( Is_usb_full_speed_mode() )
   {
      data_to_transfer = Usb_get_conf_desc_fs_length();  //!< sizeof(usb_conf_desc_fs);
      pbuffer          = Usb_get_conf_desc_fs_pointer();
   }else{
      data_to_transfer = Usb_get_conf_desc_hs_length();  //!< sizeof(usb_conf_desc_hs);
      pbuffer          = Usb_get_conf_desc_hs_pointer();
   }
#else
   data_to_transfer = Usb_get_conf_desc_length();  //!< sizeof(usb_conf_desc);
   pbuffer          = Usb_get_conf_desc_pointer();
#endif

   //** Scan descriptor

   //* Find configuration selected
   if( usb_configuration_nb == 0 )
   {
      // No configuration selected then no interface enable
      Usb_enable_stall_handshake(EP_CONTROL);
      Usb_ack_setup_received_free();
      return;
   }
   u8_i = usb_configuration_nb;
   while( u8_i != 0 )
   {
      if( CONFIGURATION_DESCRIPTOR != ((S_usb_configuration_descriptor*)pbuffer)->bDescriptorType )
      {
         data_to_transfer -=  ((S_usb_configuration_descriptor*)pbuffer)->bLength;
         pbuffer =  (U8*)pbuffer + ((S_usb_configuration_descriptor*)pbuffer)->bLength;
         continue;
      }
      u8_i--;
      if( u8_i != 0 )
      {
         data_to_transfer -=  ((S_usb_configuration_descriptor*)pbuffer)->wTotalLength;
         pbuffer =  (U8*)pbuffer + ((S_usb_configuration_descriptor*)pbuffer)->wTotalLength;
      }
   }

   // Find interface selected
   if( wIndex >= ((S_usb_configuration_descriptor*)pbuffer)->bNumInterfaces )
   {
      // Interface number unknown
      Usb_enable_stall_handshake(EP_CONTROL);
      Usb_ack_setup_received_free();
      return;
   }
   while( 1 )
   {
      if( data_to_transfer <= ((S_usb_interface_descriptor*)pbuffer)->bLength )
      {
         // Interface unknown
         Usb_enable_stall_handshake(EP_CONTROL);
         Usb_ack_setup_received_free();
         return;
      }
      data_to_transfer -=  ((S_usb_interface_descriptor*)pbuffer)->bLength;
      pbuffer =  (U8*)pbuffer + ((S_usb_interface_descriptor*)pbuffer)->bLength;
      if( INTERFACE_DESCRIPTOR != ((S_usb_interface_descriptor*)pbuffer)->bDescriptorType )
         continue;
      if( wIndex != ((S_usb_interface_descriptor*)pbuffer)->bInterfaceNumber )
         continue;
      if( wValue != ((S_usb_interface_descriptor*)pbuffer)->bAlternateSetting )
         continue;
      usb_interface_status[wIndex] = wValue;
      break;
   }

   //* Find endpoints of interface and reset it
   while( 1 )
   {
      if( data_to_transfer <= ((S_usb_endpoint_descriptor*)pbuffer)->bLength )
         break;    // End of interface
      data_to_transfer -=  ((S_usb_endpoint_descriptor*)pbuffer)->bLength;
      pbuffer =  (U8*)pbuffer + ((S_usb_endpoint_descriptor*)pbuffer)->bLength;
      if( INTERFACE_DESCRIPTOR == ((S_usb_endpoint_descriptor*)pbuffer)->bDescriptorType )
         break;    // End of interface
      if( ENDPOINT_DESCRIPTOR == ((S_usb_endpoint_descriptor*)pbuffer)->bDescriptorType )
      {
         // Reset endpoint
         u8_i = ((S_usb_endpoint_descriptor*)pbuffer)->bEndpointAddress & (~MSK_EP_DIR);
         Usb_disable_stall_handshake(u8_i);
         Usb_reset_endpoint(u8_i);
         Usb_reset_data_toggle(u8_i);
      }
   }

   // send a ZLP for STATUS phase
   Usb_ack_control_in_ready_send();
   while (!Is_usb_control_in_ready());
}


#endif  // USB_DEVICE_FEATURE == true
