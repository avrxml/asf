/**
 * \file
 *
 * \brief USB Device Human Interface Device (HID) multi-touch interface.
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _UDI_HID_MULTI_TOUCH_H_
#define _UDI_HID_MULTI_TOUCH_H_

#include "conf_usb.h"
#include "usb_protocol.h"
#include "usb_protocol_hid.h"
#include "udc_desc.h"
#include "udi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup udi_hid_mtouch_group_udc
 * @{
 */
//! Global structure which contains standard UDI API for UDC
extern UDC_DESC_STORAGE udi_api_t udi_api_hid_multi_touch;
//@}

/**
 * \ingroup udi_hid_mtouch_group
 * \defgroup udi_hid_mtouch_group_desc USB interface descriptors
 *
 * The following structures provide predefined USB interface descriptors.
 * It must be used to define the final USB descriptors.
 */
//@{

//! Interface descriptor structure for HID multi-touch
typedef struct {
	usb_iface_desc_t iface;
	usb_hid_descriptor_t hid;
	usb_ep_desc_t ep_in;
	usb_ep_desc_t ep_out;
} udi_hid_multi_touch_desc_t;

//! Report descriptor for HID multi-touch
typedef struct {
	uint8_t array[151];
} udi_hid_multi_touch_report_desc_t;


//! By default no string associated to this interface
#ifndef UDI_HID_MULTI_TOUCH_STRING_ID
#define UDI_HID_MULTI_TOUCH_STRING_ID 0
#endif

#define UDI_HID_REPORT_ID_MTOUCH    1
#define UDI_HID_REPORT_ID_MAX_COUNT 2
#define UDI_HID_REPORT_ID_FEATURE   3



//! Content of HID multi-touch interface descriptor for all speed
#define UDI_HID_MULTI_TOUCH_DESC    {\
   .iface.bLength             = sizeof(usb_iface_desc_t),\
   .iface.bDescriptorType     = USB_DT_INTERFACE,\
   .iface.bInterfaceNumber    = UDI_HID_MULTI_TOUCH_IFACE_NUMBER,\
   .iface.bAlternateSetting   = 0,\
   .iface.bNumEndpoints       = 2,\
   .iface.bInterfaceClass     = HID_CLASS,\
   .iface.bInterfaceSubClass  = HID_SUB_CLASS_NOBOOT,\
   .iface.bInterfaceProtocol  = HID_PROTOCOL_GENERIC,\
   .iface.iInterface          = UDI_HID_MULTI_TOUCH_STRING_ID,\
   .hid.bLength               = sizeof(usb_hid_descriptor_t),\
   .hid.bDescriptorType       = USB_DT_HID,\
   .hid.bcdHID                = LE16(USB_HID_BDC_V1_11),\
   .hid.bCountryCode          = USB_HID_NO_COUNTRY_CODE,\
   .hid.bNumDescriptors       = USB_HID_NUM_DESC,\
   .hid.bRDescriptorType      = USB_DT_HID_REPORT,\
   .hid.wDescriptorLength     = LE16(sizeof(udi_hid_multi_touch_report_desc_t)),\
   .ep_in.bLength             = sizeof(usb_ep_desc_t),\
   .ep_in.bDescriptorType     = USB_DT_ENDPOINT,\
   .ep_in.bEndpointAddress    = UDI_HID_MULTI_TOUCH_EP_IN,\
   .ep_in.bmAttributes        = USB_EP_TYPE_INTERRUPT,\
   .ep_in.wMaxPacketSize      = LE16(UDI_HID_MTOUCH_EP_SIZE),\
   .ep_in.bInterval           = 10,\
   .ep_out.bLength            = sizeof(usb_ep_desc_t),\
   .ep_out.bDescriptorType    = USB_DT_ENDPOINT,\
   .ep_out.bEndpointAddress   = UDI_HID_MULTI_TOUCH_EP_OUT,\
   .ep_out.bmAttributes       = USB_EP_TYPE_INTERRUPT,\
   .ep_out.wMaxPacketSize     = LE16(UDI_HID_MTOUCH_EP_SIZE),\
   .ep_out.bInterval          = 10,\
   }
//@}


/**
 * \ingroup udi_hid_group
 * \defgroup udi_hid_mtouch_group USB Device Interface (UDI) for Human Interface Device (HID) Multi-touch Class
 *
 * Common APIs used by high level application to use this USB class.
 *
 * See \ref udi_hid_mtouch_quickstart.
 * @{
 */

/**
 * \brief Routine used to send a report to USB Host
 *
 * \param data     Pointer on the report to send (size = UDI_HID_REPORT_IN_SIZE)
 * 
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
bool udi_hid_multi_touch_send_report_in(uint8_t *data);

//@}


#ifdef __cplusplus
}
#endif

/**
 * \page udi_hid_mtouch_quickstart Quick start guide for USB device multi-touch module (UDI Multi-touch)
 *
 */

#endif // _UDI_HID_MULTI_TOUCH_H_
