/**
 * \file
 *
 * \brief USB configuration
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include "compiler.h"


/**
 * USB Device Configuration
 * @{
 */

//! Device definition (mandatory)
#define  USB_DEVICE_VENDOR_ID             USB_VID_ATMEL
#define  USB_DEVICE_PRODUCT_ID            USB_PID_ATMEL_ASF_XMEGA_B1_XPLAINED
#define  USB_DEVICE_MAJOR_VERSION         1
#define  USB_DEVICE_MINOR_VERSION         0
#define  USB_DEVICE_POWER                 100 // Consumption on Vbus line (mA)
#define  USB_DEVICE_ATTR                  (USB_CONFIG_ATTR_BUS_POWERED)

//! USB Device string definitions (Optional)
#define  USB_DEVICE_MANUFACTURE_NAME      "ATMEL ASF"
#define  USB_DEVICE_PRODUCT_NAME          "XMEGA-B1 Xplained Composite demo"
#define  USB_DEVICE_SERIAL_NAME
#define  USB_DEVICE_GET_SERIAL_NAME_POINTER xplain_b1_serial_number
#define  USB_DEVICE_GET_SERIAL_NAME_LENGTH  12

/**
 * Device speeds support
 * @{
 */
//! To define a Low speed device
//#define  USB_DEVICE_LOW_SPEED

//! To authorize the High speed
#if (UC3A3||UC3A4)
//#define  USB_DEVICE_HS_SUPPORT
#endif
//@}

/**
 * USB Device Callbacks definitions (Optional)
 * @{
 */
#define  UDC_VBUS_EVENT(b_vbus_high)
#define  UDC_SOF_EVENT()                  main_sof_action()
#define  UDC_SUSPEND_EVENT()              ui_powerdown()
#define  UDC_RESUME_EVENT()               ui_wakeup()
#define  UDC_REMOTEWAKEUP_ENABLE()
#define  UDC_REMOTEWAKEUP_DISABLE())
//@}

/**
 * USB Device low level configuration
 * When only one interface is used, these configurations are defined by the class module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Control endpoint size
#define  USB_DEVICE_EP_CTRL_SIZE       64

//! Number of interfaces for this device
#define  USB_DEVICE_NB_INTERFACE       2

//! Total endpoint used by all interfaces
//! Note:
//! It is possible to define an IN and OUT endpoints with the same number on xmega product only
//! E.g. MSC class can be have IN endpoint 0x81 and OUT endpoint 0x01
#define  USB_DEVICE_MAX_EP             4  // 2 for generic HID
//@}

//@}


/**
 * USB Interface Configuration
 * @{
 */

/**
 * Configuration of MSC interface (if used)
 * @{
 */
//! Vendor name and Product version of MSC interface
#define UDI_MSC_GLOBAL_VENDOR_ID            \
   'A', 'T', 'M', 'E', 'L', ' ', ' ', ' '
#define UDI_MSC_GLOBAL_PRODUCT_VERSION            \
   '1', '.', '0', '0'

//! Interface callback definition
#define  UDI_MSC_ENABLE_EXT()          main_msc_enable()
#define  UDI_MSC_DISABLE_EXT()         main_msc_disable()

/**
 * USB MSC low level configuration
 * In standalone these configurations are defined by the MSC module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoint numbers definition
#define  UDI_MSC_EP_IN                 (1 | USB_EP_DIR_IN)
#define  UDI_MSC_EP_OUT                (2 | USB_EP_DIR_OUT)

//! Interface number
#define  UDI_MSC_IFACE_NUMBER          0
//@}
//@}

/**
 * Configuration of HID Generic interface (if used)
 * @{
 */
//! Interface callback definition

#define  UDI_HID_GENERIC_ENABLE_EXT()       main_hid_generic_enable()
#define  UDI_HID_GENERIC_DISABLE_EXT()      main_hid_generic_disable()
#define  UDI_HID_GENERIC_REPORT_OUT(ptr)    usb_hid_com_report_out_received(ptr)
#define  UDI_HID_GENERIC_SET_FEATURE(f)     usb_hid_com_set_feature(f)

#define  UDI_HID_REPORT_IN_SIZE             64
#define  UDI_HID_REPORT_OUT_SIZE            64
#define  UDI_HID_REPORT_FEATURE_SIZE        4
#define  UDI_HID_GENERIC_EP_SIZE            64

/**
 * USB HID Generic low level configuration
 * In standalone these configurations are defined by the HID generic module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoint numbers definition
#define  UDI_HID_GENERIC_EP_OUT   (3 | USB_EP_DIR_OUT)
#define  UDI_HID_GENERIC_EP_IN    (4 | USB_EP_DIR_IN)

//! Interface number
#define  UDI_HID_GENERIC_IFACE_NUMBER    1
//@}
//@}

//@}


/**
 * Description of Composite Device
 * @{
 */
//! USB Interfaces descriptor structure
#define UDI_COMPOSITE_DESC_T                  \
	udi_msc_desc_t udi_msc;               \
	udi_hid_generic_desc_t udi_hid_generic

//! USB Interfaces descriptor value for Full Speed
#define UDI_COMPOSITE_DESC_FS \
	.udi_msc                   = UDI_MSC_DESC_FS, \
	.udi_hid_generic           = UDI_HID_GENERIC_DESC

//! USB Interface APIs
#define UDI_COMPOSITE_API  \
	&udi_api_msc,            \
	&udi_api_hid_generic

//@}


/**
 * USB Device Driver Configuration
 * @{
 */
//@}

//! The includes of classes and other headers must be done at the end of this file to avoid compile error
#include "udi_msc.h"
#include "udi_hid_generic.h"
#include "usb_hid_com.h"
#include "main.h"


#endif // _CONF_USB_H_
