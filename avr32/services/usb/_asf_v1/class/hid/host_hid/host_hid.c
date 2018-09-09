/*****************************************************************************
 *
 * \file
 *
 * \brief Management of the generic host HID features.
 *
 * This file manages the generic host HID features.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


//_____  I N C L U D E S _______________________________________________________

#include <stddef.h>
#include "compiler.h"
#include "conf_usb.h"
#include "usb_task.h"
#include "usb_host_task.h"
#include "usb_host_enum.h"
#include "hid.h"
#include "host_hid.h"


//_____ M A C R O S ____________________________________________________________


//_____ D E F I N I T I O N S __________________________________________________

//! HID report descriptor parser control structure.
static struct
{
  size_t length;
  const hid_item_t *item;
} host_hid_report_descriptor_parser =
{
  0,
  NULL
};


//_____ D E C L A R A T I O N S ________________________________________________

/*! \name Standard Requests Applied to HID
 */
//! @{


Status_t host_hid_get_descriptor(uint8_t descriptor_type, uint8_t descriptor_index, uint8_t s_interface)
{
  Status_t status;
  const hid_descriptor_t *hid_descriptor = (hid_descriptor_t *)&data_stage;
  uint8_t i;

  usb_request.bmRequestType   = 0x81;
  usb_request.bRequest        = GET_DESCRIPTOR;
  usb_request.wValue          = descriptor_type << 8 | descriptor_index;
  usb_request.wIndex          = Get_interface_number(s_interface);
  usb_request.wLength         = SIZEOF_DATA_STAGE;
  usb_request.incomplete_read = false;

  status = host_transfer_control(data_stage);

  switch (descriptor_type)
  {
  case HID_DESCRIPTOR:
    for (i = 0; i < hid_descriptor->bNumDescriptors; i++)
    {
      if (hid_descriptor->Descriptor[i].bType == HID_REPORT_DESCRIPTOR)
      {
        host_hid_report_descriptor_parser.length =
          usb_format_usb_to_mcu_data(16, hid_descriptor->Descriptor[i].wLength);
        break;
      }
    }
    break;

  case HID_REPORT_DESCRIPTOR:
    host_hid_report_descriptor_parser.item = (hid_item_t *)&data_stage;
    break;
  }

  return status;
}


Status_t host_hid_set_descriptor(uint8_t descriptor_type, uint8_t descriptor_index, uint8_t s_interface, uint16_t length)
{
  usb_request.bmRequestType   = 0x01;
  usb_request.bRequest        = SET_DESCRIPTOR;
  usb_request.wValue          = descriptor_type << 8 | descriptor_index;
  usb_request.wIndex          = Get_interface_number(s_interface);
  usb_request.wLength         = length;
  usb_request.incomplete_read = false;

  return host_transfer_control(data_stage);
}


//! @}


/*! \name HID-Specific Requests
 */
//! @{


Status_t host_hid_get_report(uint8_t report_type, uint8_t report_id, uint8_t s_interface)
{
  usb_request.bmRequestType   = 0xA1;
  usb_request.bRequest        = HID_GET_REPORT;
  usb_request.wValue          = report_type << 8 | report_id;
  usb_request.wIndex          = Get_interface_number(s_interface);
  usb_request.wLength         = SIZEOF_DATA_STAGE;
  usb_request.incomplete_read = false;

  return host_transfer_control(data_stage);
}


Status_t host_hid_set_report(uint8_t report_type, uint8_t report_id, uint8_t s_interface, uint16_t length)
{
  usb_request.bmRequestType   = 0x21;
  usb_request.bRequest        = HID_SET_REPORT;
  usb_request.wValue          = report_type << 8 | report_id;
  usb_request.wIndex          = Get_interface_number(s_interface);
  usb_request.wLength         = length;
  usb_request.incomplete_read = false;

  return host_transfer_control(data_stage);
}


uint8_t host_hid_get_idle(uint8_t report_id, uint8_t s_interface)
{
  usb_request.bmRequestType   = 0xA1;
  usb_request.bRequest        = HID_GET_IDLE;
  usb_request.wValue          = 0x00 << 8 | report_id;
  usb_request.wIndex          = Get_interface_number(s_interface);
  usb_request.wLength         = 0x0001;
  usb_request.incomplete_read = false;

  host_transfer_control(data_stage);

  return data_stage[0x00];
}


Status_t host_hid_set_idle(uint8_t duration_4_ms, uint8_t report_id, uint8_t s_interface)
{
  usb_request.bmRequestType   = 0x21;
  usb_request.bRequest        = HID_SET_IDLE;
  usb_request.wValue          = duration_4_ms << 8 | report_id;
  usb_request.wIndex          = Get_interface_number(s_interface);
  usb_request.wLength         = 0x0000;
  usb_request.incomplete_read = false;

  return host_transfer_control(data_stage);
}


uint8_t host_hid_get_protocol(uint8_t s_interface)
{
  usb_request.bmRequestType   = 0xA1;
  usb_request.bRequest        = HID_GET_PROTOCOL;
  usb_request.wValue          = 0x0000;
  usb_request.wIndex          = Get_interface_number(s_interface);
  usb_request.wLength         = 0x0001;
  usb_request.incomplete_read = false;

  host_transfer_control(data_stage);

  return data_stage[0x00];
}


Status_t host_hid_set_protocol(uint8_t protocol, uint8_t s_interface)
{
  usb_request.bmRequestType   = 0x21;
  usb_request.bRequest        = HID_SET_PROTOCOL;
  usb_request.wValue          = protocol;
  usb_request.wIndex          = Get_interface_number(s_interface);
  usb_request.wLength         = 0x0000;
  usb_request.incomplete_read = false;

  return host_transfer_control(data_stage);
}


//! @}


/*! \name HID Report Descriptor Parsing Functions
 */
//! @{


Status_bool_t host_hid_get_item(host_hid_item_t *item)
{
  const hid_item_t *hid_item = host_hid_report_descriptor_parser.item;
  uint8_t size;

  if (host_hid_report_descriptor_parser.length <
      (uint8_t *)&hid_item->header - (uint8_t *)hid_item +
      sizeof(hid_item->header))
    return false;

  item->type = hid_item->header.bType;

  if (hid_item->header.bTag == HID_ITEM_TAG_LONG_ITEM)
  {
    if (host_hid_report_descriptor_parser.length <
        (uint8_t *)&hid_item->long_format.bDataSize - (uint8_t *)hid_item +
        sizeof(hid_item->long_format.bDataSize))
      return false;

    size = hid_item->long_format.bDataSize;

    if (host_hid_report_descriptor_parser.length <
        (uint8_t *)&hid_item->long_format.data - (uint8_t *)hid_item +
        size)
      return false;

    item->tag = hid_item->long_format.bLongItemTag;

    item->long_format = true;

    item->long_data.size = size;
    item->long_data.data = &hid_item->long_format.data;

    host_hid_report_descriptor_parser.length -=
      (uint8_t *)&hid_item->long_format.data - (uint8_t *)hid_item +
      size;

    host_hid_report_descriptor_parser.item =
      (hid_item_t *)&hid_item->long_format.data[size];
  }
  else
  {
    uint8_t i;

    size = (hid_item->short_format.bSize) ?
             1 << (hid_item->short_format.bSize - 1) :
             0;

    if (host_hid_report_descriptor_parser.length <
        (uint8_t *)&hid_item->short_format.data - (uint8_t *)hid_item +
        size)
      return false;

    item->tag = hid_item->short_format.bTag;

    item->long_format = false;

    item->short_data.value = 0x00000000;
    for (i = 0; i < size; i++)
    {
      item->short_data.value = item->short_data.value << 8 |
                               hid_item->short_format.data[i];
    }
    item->short_data.value =
      usb_format_usb_to_mcu_data(32, item->short_data.value <<
                                     ((sizeof(uint32_t) - size) << 3));

    host_hid_report_descriptor_parser.length -=
      (uint8_t *)&hid_item->short_format.data - (uint8_t *)hid_item +
      size;

    host_hid_report_descriptor_parser.item =
      (hid_item_t *)&hid_item->short_format.data[size];
  }

  return true;
}


//! @}
