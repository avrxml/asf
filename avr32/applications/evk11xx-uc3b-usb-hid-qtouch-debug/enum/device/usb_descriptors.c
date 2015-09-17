/**************************************************************************
 *
 * \file
 *
 * \brief USB identifiers.
 *
 * This file contains the USB parameters that uniquely identify the USB
 * application through descriptor tables.
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
// usb_hid_report_descriptor
const U8 usb_hid_report_descriptor [USB_HID_REPORT_DESC] = {

      0x06, 0xFF, 0xFF,          // 04|2   , Usage Page (vendordefined?)
      0x09, 0x01,                // 08|1   , Usage      (vendordefined
      0xA1, 0x01,                // A0|1   , Collection (Application)

      // IN report
      0x09, 0x02,                // 08|1   , Usage      (vendordefined)
      0x09, 0x03,                // 08|1   , Usage      (vendordefined)
      0x15, 0x00,                // 14|1   , Logical Minimum(0 for signed byte?)
      0x26 ,0xFF,0x00,           // 24|1   , Logical Maximum(255 for signed byte?)
      0x75, 0x08,                // 74|1   , Report Size(8) = field size in bits = 1 byte
      0x95, LENGTH_OF_REPORT_IN, // 94|1:ReportCount(size) = repeat count of previous item
      0x81, 0x02,                // 80|1: IN report (Data,Variable, Absolute)

      // OUT report
      0x09, 0x04,                // 08|1   , Usage      (vendordefined)
      0x09, 0x05,                // 08|1   , Usage      (vendordefined)
      0x15, 0x00,                // 14|1   , Logical Minimum(0 for signed byte?)
      0x26, 0xFF,0x00,           // 24|1   , Logical Maximum(255 for signed byte?)
      0x75, 0x08,                // 74|1   , Report Size(8) = field size in bits = 1 byte
      0x95, LENGTH_OF_REPORT_OUT,// 94|1:ReportCount(size) = repeat count of previous item
      0x91, 0x02,                // 90|1: OUT report (Data,Variable, Absolute)

      // Feature report
      0x09, 0x06,                // 08|1   , Usage      (vendordefined)
      0x09, 0x07,                // 08|1   , Usage      (vendordefined)
      0x15, 0x00,                // 14|1   , LogicalMinimum(0 for signed byte)
      0x26, 0xFF,0x00,           // 24|1   , Logical Maximum(255 for signed byte)
      0x75, 0x08,                // 74|1   , Report Size(8) =field size in bits = 1 byte
      0x95, 0x04,                // 94|1:ReportCount
      0xB1, 0x02,                // B0|1:   Feature report
      0xC0                       // C0|0    , End Collection

 };




// usb_user_device_descriptor
const S_usb_device_descriptor usb_dev_desc =
{
  sizeof(S_usb_device_descriptor),
  DEVICE_DESCRIPTOR,
  Usb_format_mcu_to_usb_data(16, USB_SPECIFICATION),
  DEVICE_CLASS,
  DEVICE_SUB_CLASS,
  DEVICE_PROTOCOL,
  EP_CONTROL_LENGTH,
  Usb_format_mcu_to_usb_data(16, VENDOR_ID),
  Usb_format_mcu_to_usb_data(16, PRODUCT_ID),
  Usb_format_mcu_to_usb_data(16, RELEASE_NUMBER),
  MAN_INDEX,
  PROD_INDEX,
  SN_INDEX,
  NB_CONFIGURATION
};


// usb_user_configuration_descriptor FS
const S_usb_user_configuration_descriptor usb_conf_desc_fs =
{
  {
    sizeof(S_usb_configuration_descriptor),
    CONFIGURATION_DESCRIPTOR,
    Usb_format_mcu_to_usb_data(16, sizeof(S_usb_user_configuration_descriptor)),
    NB_INTERFACE,
    CONF_NB,
    CONF_INDEX,
    CONF_ATTRIBUTES,
    MAX_POWER
  },

  {
    sizeof(S_usb_interface_descriptor),
    INTERFACE_DESCRIPTOR,
    INTERFACE_NB,
    ALTERNATE,
    NB_ENDPOINT,
    INTERFACE_CLASS,
    INTERFACE_SUB_CLASS,
    INTERFACE_PROTOCOL,
    INTERFACE_INDEX
  },

  {
    sizeof(S_usb_hid_descriptor),
    HID_DESCRIPTOR,
    Usb_format_mcu_to_usb_data(16, HID_BDC),
    HID_COUNTRY_CODE_NOT_SUPPORTED,
    HID_CLASS_DESC_NB_DEFAULT,
    HID_REPORT_DESCRIPTOR,
    Usb_format_mcu_to_usb_data(16, sizeof(usb_hid_report_descriptor))
  },
  {
    sizeof(S_usb_endpoint_descriptor),
    ENDPOINT_DESCRIPTOR,
    ENDPOINT_NB_1,
    EP_ATTRIBUTES_1,
    Usb_format_mcu_to_usb_data(16, EP_SIZE_1),
    EP_INTERVAL_1
  }
  ,
  { sizeof(S_usb_endpoint_descriptor)
  , DESCRIPTOR_ENDPOINT
  , ENDPOINT_NB_2
  , EP_ATTRIBUTES_2
  , Usb_format_mcu_to_usb_data(16,EP_SIZE_2)
  , EP_INTERVAL_2
  }

};

// usb_user_language_id
const S_usb_language_id usb_user_language_id =
{
  sizeof(S_usb_language_id),
  STRING_DESCRIPTOR,
  Usb_format_mcu_to_usb_data(16, LANGUAGE_ID)
};


// usb_user_manufacturer_string_descriptor
const S_usb_manufacturer_string_descriptor usb_user_manufacturer_string_descriptor =
{
  sizeof(S_usb_manufacturer_string_descriptor),
  STRING_DESCRIPTOR,
  USB_MANUFACTURER_NAME
};


// usb_user_product_string_descriptor
const S_usb_product_string_descriptor usb_user_product_string_descriptor =
{
  sizeof(S_usb_product_string_descriptor),
  STRING_DESCRIPTOR,
  USB_PRODUCT_NAME
};


// usb_user_serial_number
const S_usb_serial_number usb_user_serial_number =
{
  sizeof(S_usb_serial_number),
  STRING_DESCRIPTOR,
  USB_SERIAL_NUMBER
};


#endif  // USB_DEVICE_FEATURE == true
