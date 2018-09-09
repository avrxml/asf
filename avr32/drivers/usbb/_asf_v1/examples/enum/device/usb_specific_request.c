/**************************************************************************
 *
 * \file
 *
 * \brief Processing of USB device specific enumeration requests.
 *
 * This file contains the specific request decoding for enumeration process.
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

extern const  void *pbuffer;
extern        U16   data_to_transfer;


//_____ D E C L A R A T I O N S ____________________________________________

//! @brief This function configures the endpoints of the device application.
//! This function is called when the set configuration request has been received.
//!
void usb_user_endpoint_init(U8 conf_nb)
{
  (void)Usb_configure_endpoint(EP_TEMP_IN,
                               EP_ATTRIBUTES_TEMP1,
                               DIRECTION_IN,
                               EP_SIZE_TEMP1,
                               SINGLE_BANK);

  (void)Usb_configure_endpoint(EP_TEMP_OUT,
                               EP_ATTRIBUTES_TEMP2,
                               DIRECTION_OUT,
                               EP_SIZE_TEMP2,
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
  U8 descriptor_type;
  U8 string_type;

  string_type     = Usb_read_endpoint_data(EP_CONTROL, 8);
  descriptor_type = Usb_read_endpoint_data(EP_CONTROL, 8);

  switch (request)
  {
  case GET_DESCRIPTOR:
    switch (descriptor_type)
    {
    default:
      break;
    }
    break;

  case SET_CONFIGURATION:
    switch (descriptor_type)
    {
    default:
      break;
    }
    break;

  default:
    break;
  }

  return false;
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


#endif  // USB_DEVICE_FEATURE == true
