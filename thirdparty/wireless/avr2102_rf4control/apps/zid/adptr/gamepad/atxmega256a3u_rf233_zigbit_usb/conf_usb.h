/**
 * \file
 *
 * \brief USB configuration file
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
 */

#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include "compiler.h"

/**
 * USB Device Configuration
 * @{
 */

/* ! Device definition (mandatory) */

#define  USB_DEVICE_VENDOR_ID             USB_VID_ATMEL
#define  USB_DEVICE_PRODUCT_ID            0x2437
#define  USB_DEVICE_MAJOR_VERSION         1
#define  USB_DEVICE_MINOR_VERSION         0
#define  USB_DEVICE_POWER                 100 /* Consumption on Vbus line (mA)
	                                       **/
#define  USB_DEVICE_ATTR \
	(USB_CONFIG_ATTR_REMOTE_WAKEUP | USB_CONFIG_ATTR_SELF_POWERED)
/* (USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED) */
/* (USB_CONFIG_ATTR_SELF_POWERED) */
/* (USB_CONFIG_ATTR_BUS_POWERED) */

/* ! USB Device string definitions (Optional) */
#define  USB_DEVICE_MANUFACTURE_NAME      "ATMEL ASF"
#define  USB_DEVICE_PRODUCT_NAME          "HID Gamepad"
#define  USB_DEVICE_SERIAL_NAME           "123123123123" /* Disk SN for MSC */

/**
 * Device speeds support
 * @{
 */

/* ! To define a Low speed device */
#define  USB_DEVICE_LOW_SPEED

/* ! To authorize the High speed */
#if (UC3A3 || UC3A4)
#  define  USB_DEVICE_HS_SUPPORT
#elif (SAM3XA || SAM3U)
#  define  USB_DEVICE_HS_SUPPORT
#endif
/* @} */

/**
 * USB Device Callbacks definitions (Optional)
 * @{
 */
#define  UDC_VBUS_EVENT(b_vbus_high)
#define  UDC_SOF_EVENT()
#define  UDC_SUSPEND_EVENT()
#define  UDC_RESUME_EVENT()
/* ! Mandatory when USB_DEVICE_ATTR authorizes remote wakeup feature */
#define  UDC_REMOTEWAKEUP_ENABLE()
#define  UDC_REMOTEWAKEUP_DISABLE()
/* ! When a extra string descriptor must be supported */
/* ! other than manufacturer, product and serial string */
/* #define  UDC_GET_EXTRA_STRING() */
/* @} */

/**
 * USB Device low level configuration
 * When only one interface is used, these configurations are defined by the
 * class module.
 * For composite device, these configuration must be defined here
 * @{
 */
/* ! Control endpoint size */
#define  USB_DEVICE_EP_CTRL_SIZE       8

/* ! Two interfaces for this device (CDC + MSC + HID mouse + HID keyboard) */
#define  USB_DEVICE_NB_INTERFACE       1

/* ! 7 endpoints used by HID mouse, HID keyboard, CDC and MSC interfaces */

/* ! but an IN and OUT endpoints can be defined with the same number on XMEGA,
 * thus 5 */
/* (7 | USB_EP_DIR_IN)  // CDC Notify endpoint */
/* (6 | USB_EP_DIR_IN)  // CDC TX */
/* (5 | USB_EP_DIR_OUT) // CDC RX */
/* (1 | USB_EP_DIR_IN)  // MSC IN */
/* (2 | USB_EP_DIR_OUT) // MSC OUT */
/* (3 | USB_EP_DIR_IN)  // HID mouse report */
/* (4 | USB_EP_DIR_IN)  // HID keyboard report */
#define  USB_DEVICE_MAX_EP             1
#  if SAM3XA && defined(USB_DEVICE_HS_SUPPORT)
/* In HS mode, size of bulk endpoints are 512 */
/* If CDC and MSC endpoints all uses 2 banks, DPRAM is not enough: 4 bulk */
/* endpoints requires 4K bytes. So reduce the number of banks of CDC bulk */
/* endpoints to use less DPRAM. Keep MSC setting to keep MSC performance. */
#     define  UDD_BULK_NB_BANK(ep) ((ep == 5 || ep == 6) ? 1 : 2)
#  endif
/* @} */

/* @} */

/**
 * USB Interface Configuration
 * @{
 */

/**
 * Configuration of HID Keyboard interface
 * @{
 */
/* ! Interface callback definition */
#define  UDI_HID_GPD_ENABLE_EXT()      main_kbd_enable()
#define  UDI_HID_GPD_DISABLE_EXT()     main_kbd_disable()
#define  UDI_HID_GPD_CHANGE_LED(value)

/* ! Enable id string of interface to add an extra USB string */
/* #define  UDI_HID_KBD_STRING_ID            0 */

/**
 * USB HID Keyboard low level configuration
 * In standalone these configurations are defined by the HID Keyboard module.
 * For composite device, these configuration must be defined here
 * @{
 */
/* ! Endpoint numbers definition */
#define  UDI_HID_GPD_EP_IN           (1 | USB_EP_DIR_IN)

/* ! Interface number */
#define  UDI_HID_GPD_IFACE_NUMBER    0
/* @} */
/* @} */

/* @} */

/**
 * Description of Composite Device
 * @{
 */
/* ! USB Interfaces descriptor structure */
#define UDI_COMPOSITE_DESC_T \
	udi_hid_gpd_desc_t udi_hid_gpd

/* ! USB Interfaces descriptor value for Full Speed */
#define UDI_COMPOSITE_DESC_FS \
	.udi_hid_gpd   = UDI_HID_GPD_DESC

/* ! USB Interfaces descriptor value for High Speed */
#define UDI_COMPOSITE_DESC_HS \
	.udi_hid_gpd   = UDI_HID_GPD_DESC

/* ! USB Interface APIs */
#define UDI_COMPOSITE_API \
	& udi_api_hid_gpd
/* @} */

/**
 * USB Device Driver Configuration
 * @{
 */
/* @} */

/* ! The includes of classes and other headers must be done at the end of this
 * file to avoid compile error */
/* #include "udi_cdc.h" */
/* #include "udi_msc.h" */
/* #include "udi_hid_mouse.h" */
#include "udi_hid_gpd.h"
#include "app_config.h"

#endif /* _CONF_USB_H_ */
