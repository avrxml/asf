/**
 * \file
 *
 * \brief USB configuration file for CDC application
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

#include <compiler.h>

/**
 * USB Device Configuration
 * @{
 */

/* ! Device definition */
#define  USB_DEVICE_VENDOR_ID             USB_VID_ATMEL
#define  USB_DEVICE_PRODUCT_ID            USB_PID_ATMEL_XMEGA_USB_ZIGBIT_SUBGHZ
#define  USB_DEVICE_MAJOR_VERSION         1
#define  USB_DEVICE_MINOR_VERSION         0
#define  USB_DEVICE_POWER                 100
#define  USB_DEVICE_MANUFACTURE_NAME      "ATMEL ASF"
#define  USB_DEVICE_PRODUCT_NAME          "CDC"

#define  USB_DEVICE_ATTR \
	(USB_CONFIG_ATTR_SELF_POWERED)
/* (USB_CONFIG_ATTR_BUS_POWERED) */
/*	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_SELF_POWERED) */
/*	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED) */

/**
 * Device speeds support
 * Low speed not supported by CDC
 * @{
 */
/* ! To authorize the High speed */
#if (UC3A3 || UC3A4)
/* #define  USB_DEVICE_HS_SUPPORT */
#endif
/* @} */

#define USB_ENABLE

/**
 * USB Device Callbacks definitions
 * @{
 */
#define  UDC_VBUS_EVENT(b_vbus_high)
#define  UDC_SOF_EVENT()
#define  UDC_SUSPEND_EVENT()
#define  UDC_RESUME_EVENT()
/* @} */

/* @} */

/**
 * USB Interface Configuration
 * @{
 */

/**
 * Configuration of CDC interface
 * @{
 */
/* ! Interface callback definition */
#define  UDI_CDC_ENABLE_EXT(port)          main_cdc_enable(port)
#define  UDI_CDC_DISABLE_EXT(port)         main_cdc_disable(port)
#define  UDI_CDC_RX_NOTIFY(port)           usb_rx_notify()
#define  UDI_CDC_SET_CODING_EXT(port, cfg)
#define  UDI_CDC_SET_DTR_EXT(port, set)
#define  UDI_CDC_SET_RTS_EXT(port, set)

bool main_cdc_enable(uint8_t port);

/*! \brief Called by CDC interface
 * Callback running when USB Host disable cdc interface
 */
void main_cdc_disable(uint8_t port);

/* ! Default configuration of communication port */
#define  UDI_CDC_DEFAULT_RATE             115200
#define  UDI_CDC_DEFAULT_STOPBITS         CDC_STOP_BITS_1
#define  UDI_CDC_DEFAULT_PARITY           CDC_PAR_NONE
#define  UDI_CDC_DEFAULT_DATABITS         8
/* @} */
/* @} */

/**
 * USB Device Driver Configuration
 * @{
 */
/* @} */

/* ! The includes of classes and other headers must be done at the end of this
 * file to avoid compile error */
#include <udi_cdc_conf.h>
#include "sio2host.h"

#endif /* _CONF_USB_H_ */
