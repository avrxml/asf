/**
 * \file
 *
 * \brief Android Open Accessory
 *
 * Copyright (C) 2012 Atmel Corporation. All rights reserved.
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
#include "conf_usb_host.h"
#include "usb_protocol_aoa.h"
#include "uhd.h"
#include "uhc.h"
#include "conf_aoa.h"
#include "uhi_aoa.h"
#include <string.h>

#ifdef USB_HOST_HUB_SUPPORT
#   error USB HUB support is not implemented on UHI Android Open Accessory
#endif

/**
 * \addtogroup uhi_aoa_group
 * @{
 */

/** \name Stages of enabling the Android Device
 * @{
 */
#define AOA_ENABLE_STAGE_PROCESSING 0
#define AOA_ENABLE_STAGE_SUCCESSFUL 1
#define AOA_ENABLE_STAGE_FAILED     2
/** @} */

/**
 * \name Internal Defines and Variables to store information about the Accessory
 * @{
 */

/*! \brief Function to enable the Accessory Mode on Android Devices
 *  - Implements step one of starting the Device in Accessory Mode
 *  - Checks the Protocol Version on the Android Device
 *  - Calls step two of starting the Device in Accessory Mode
 */
void uhi_aoa_mode_enable_step1(uhc_device_t *);

/*! \brief Function to enable the Accessory Mode on Android Devices
 *  - Implements steps two and three of starting the Device in Accessory Mode
 *  - Sends the Identifying Information as Strings
 *  - Calls step three of starting the Device in Accessory Mode
 */
void uhi_aoa_mode_enable_step2(usb_add_t, uhd_trans_status_t, uint16_t);

/*! \brief Function to enable the Accessory Mode on Android Devices
 *  - Implements step three of starting the Device in Accessory Mode
 *  - Sends the Request to Startup in Accessory Mode
 *  - Calls last step of starting the Device in Accessory Mode
 */
void uhi_aoa_mode_enable_step3(usb_add_t, uhd_trans_status_t, uint16_t);

/*! \brief Function to enable the Accessory Mode on Android Devices
 *  - Called when the process of trying to start the device in Accessory Mode
 *    is finished.
 *  - Determines whether the process of starting in Accessory Mode was successful
 *  - Does NOT check if the device is in Accessory Mode. That happens later
 */
void uhi_aoa_mode_enable_complete(usb_add_t, uhd_trans_status_t, uint16_t);

/** Stores relevant information about the USB connection */
struct uhi_aoa_dev_t {
	uhc_device_t *dev;
	usb_ep_t ep_in;
	usb_ep_t ep_out;
};

/** Current USB device AOA selected by the UHI AOA */
#ifdef USB_HOST_HUB_SUPPORT
static struct uhi_aoa_dev_t *uhi_aoa_dev_sel;
#else
#   define uhi_aoa_dev_sel (&uhi_aoa_dev)
#endif

static struct uhi_aoa_dev_t uhi_aoa_dev = {
	.dev = NULL,
};

#ifndef AOA_STRING_MANUFACTURER
#   define AOA_STRING_MANUFACTURER "Atmel"
#   warning Using the default Manufacturer value meant for the EVK1105
#endif
#ifndef AOA_STRING_MODEL
#   define AOA_STRING_MODEL        "EVK1105"
#   warning Using the default Model value meant for the EVK1105
#endif
#ifndef AOA_STRING_DESCRIPTION
#   define AOA_STRING_DESCRIPTION  "EVK1105 Application Board"
#   warning Using the default Application Board value meant for the EVK1105
#endif
#ifndef AOA_STRING_VERSION
#   define AOA_STRING_VERSION      "1.0"
#   warning Using the default Version value meant for the EVK1105
#endif
#ifndef AOA_STRING_URL
#   define AOA_STRING_URL          "http://www.atmel.com/applications/homeentertainment/mobile_accessories/default.aspx"
#   warning Using the default URL value meant for the EVK1105
#endif
#ifndef AOA_STRING_SERIAL
#   define AOA_STRING_SERIAL       "0000000012345678"
#   warning Using the default Serial value meant for the EVK1105
#endif

/** @} */

/** Stores the "Android Open Accessory Protocol" version */
volatile uint16_t uhi_aoa_protocol;

/** Stores the status of the process enabling the Accessory device */
volatile uint8_t uhi_aoa_enable_stage;

/**
 * \name Functions required by UHC
 * @{
 */

uhc_enum_status_t uhi_aoa_install(uhc_device_t *dev)
{
	if (uhi_aoa_dev.dev != NULL) {
		return UHC_ENUM_SOFTWARE_LIMIT; /* Device already allocated */
	}

	if ((dev->dev_desc.idVendor != le16_to_cpu(GOOGLE_VID)) ||
			((dev->dev_desc.idProduct != le16_to_cpu(AOA_PID)) &&
			(dev->dev_desc.idProduct !=
			le16_to_cpu(AOA_ADB_PID)))) {
		/* No Device supporting AOA or not in Accessory Mode */
		uhi_aoa_enable_stage = AOA_ENABLE_STAGE_PROCESSING;
		uhi_aoa_mode_enable_step1(dev);
		if (uhi_aoa_enable_stage == AOA_ENABLE_STAGE_SUCCESSFUL) {
			/* Supports Android Open Accessory Protocol */
			return UHC_ENUM_SUCCESS;
		} else {
			/* Does not support Android Open Accessory Protocol */
			return UHC_ENUM_UNSUPPORTED;
		}
	} else {
		/* In AOA MODE */
		uhi_aoa_dev.dev = dev;
		return UHC_ENUM_SUCCESS;
	}

	return UHC_ENUM_UNSUPPORTED;
}

void uhi_aoa_enable(uhc_device_t *dev)
{
	if (uhi_aoa_dev.dev != dev) {
		return; /* No interface to enable */
	}

	/* Init value */
	UHI_AOA_CHANGE(dev, true);
}

void uhi_aoa_uninstall(uhc_device_t *dev)
{
	if (uhi_aoa_dev.dev != dev) {
		return; /* Device not enabled in this interface */
	}

	uhi_aoa_dev.dev = NULL;
	uhi_aoa_dev.ep_in = 0x00;
	uhi_aoa_dev.ep_out = 0x00;
	UHI_AOA_CHANGE(dev, false);
}

/** @} */

void uhi_aoa_mode_enable_step1(uhc_device_t *dev)
{
	uhi_aoa_dev.dev = dev;

	usb_setup_req_t req;
	req.bmRequestType = USB_REQ_RECIP_DEVICE | USB_REQ_TYPE_VENDOR |
			USB_REQ_DIR_IN;
	req.bRequest = (uint8_t)USB_REQ_AOA_GET_PROTOCOL;
	req.wValue = 0;
	req.wIndex = 0;
	req.wLength = 0x02;
	uhd_setup_request(dev->address, &req, (uint8_t *)&uhi_aoa_protocol, 2,
			NULL, uhi_aoa_mode_enable_step2);
}

void uhi_aoa_mode_enable_step2(
		usb_add_t add,
		uhd_trans_status_t status,
		uint16_t payload_trans)
{
	(void)add;
	(void)payload_trans;
	if (status == UHD_TRANS_NOERROR) {
		uhi_aoa_protocol = le16_to_cpu(uhi_aoa_protocol);
		if (uhi_aoa_protocol != 0) {
			uhi_aoa_send_info_string(
					(uint8_t)AOA_STRING_INDEX_MANUFACTURER,
					(char *)AOA_STRING_MANUFACTURER, NULL);
			uhi_aoa_send_info_string(
					(uint8_t)AOA_STRING_INDEX_MODEL,
					(char *)AOA_STRING_MODEL, NULL);
			uhi_aoa_send_info_string(
					(uint8_t)AOA_STRING_INDEX_DESCRIPTION,
					(char *)AOA_STRING_DESCRIPTION, NULL);
			uhi_aoa_send_info_string(
					(uint8_t)AOA_STRING_INDEX_VERSION,
					(char *)AOA_STRING_VERSION, NULL);
			uhi_aoa_send_info_string(
					(uint8_t)AOA_STRING_INDEX_URL,
					(char *)AOA_STRING_URL, NULL);
			uhi_aoa_send_info_string(
					(uint8_t)AOA_STRING_INDEX_SERIAL,
					(char *)AOA_STRING_SERIAL,
					uhi_aoa_mode_enable_step3);
		} else { /* Device does not support Accessory Mode */
			uhi_aoa_enable_stage = AOA_ENABLE_STAGE_FAILED;
		}
	} else {
		uhi_aoa_enable_stage = AOA_ENABLE_STAGE_FAILED;
	}
}

void uhi_aoa_mode_enable_step3(
		usb_add_t add,
		uhd_trans_status_t status,
		uint16_t payload_trans)
{
	(void)add;
	(void)payload_trans;
	if (status == UHD_TRANS_NOERROR) {
		usb_setup_req_t req;

		req.bmRequestType = USB_REQ_RECIP_DEVICE | USB_REQ_TYPE_VENDOR |
				USB_REQ_DIR_OUT;
		req.bRequest = (uint8_t)USB_REQ_AOA_STARTUP;
		req.wValue = 0;
		req.wIndex = 0;
		req.wLength = 0;

		uhd_setup_request(uhi_aoa_dev_sel->dev->address, &req, NULL, 0,
				NULL, uhi_aoa_mode_enable_complete);
	} else {
		uhi_aoa_enable_stage = AOA_ENABLE_STAGE_FAILED;
	}
}

void uhi_aoa_mode_enable_complete(
		usb_add_t add,
		uhd_trans_status_t status,
		uint16_t payload_trans)
{
	(void)add;
	(void)payload_trans;
	if (status == UHD_TRANS_NOERROR) {
		uhi_aoa_enable_stage = AOA_ENABLE_STAGE_SUCCESSFUL;
	} else {
		uhi_aoa_enable_stage = AOA_ENABLE_STAGE_FAILED;
	}
}

void uhi_aoa_send_info_string(uint8_t pindex, char *pinfo,
		uhd_callback_setup_end_t callback_end)
{
	usb_setup_req_t req;

	req.bmRequestType = USB_REQ_RECIP_DEVICE | USB_REQ_TYPE_VENDOR |
			USB_REQ_DIR_OUT;
	req.bRequest = (uint8_t)USB_REQ_AOA_SEND_IDENT_STRING;
	req.wValue = 0;
	req.wIndex = ((uint16_t)pindex);
	req.wLength = ((uint16_t)(strlen(pinfo) + 1));
	uhd_setup_request(uhi_aoa_dev_sel->dev->address, &req, (uint8_t *)pinfo,
			req.wLength, NULL, callback_end);
}

bool uhi_aoa_configure(uhc_device_t *dev)
{
	uint16_t conf_desc_length;
	usb_iface_desc_t *ptr_iface;

	conf_desc_length = le16_to_cpu(dev->conf_desc->wTotalLength);
	ptr_iface = (usb_iface_desc_t *)dev->conf_desc;

	usb_ep_desc_t *ptr_ep;
	while (conf_desc_length) {
		switch (ptr_iface->bDescriptorType) {
		case USB_DT_INTERFACE:
			/* No used Information */
			break;

		case USB_DT_ENDPOINT:
			/* Allocation of the endpoint */
			ptr_ep = (usb_ep_desc_t *)ptr_iface;
			Assert(ptr_ep->bmAttributes != USB_EP_TYPE_BULK);
			if (!uhd_ep_alloc(dev->address, ptr_ep)) {
				return false; /* Endpoint allocation fail */
			}

			if (ptr_ep->bEndpointAddress & USB_EP_DIR_IN) {
				uhi_aoa_dev.ep_in = ptr_ep->bEndpointAddress;
			} else {
				uhi_aoa_dev.ep_out = ptr_ep->bEndpointAddress;
			}

			if (uhi_aoa_dev.ep_out && uhi_aoa_dev.ep_in) {
				/* All Endpoints allocated */
				uhi_aoa_dev.dev = dev;
				return true;
			}

			break;

		default:
			break;
		}
		Assert(conf_desc_length >= ptr_iface->bLength);
		conf_desc_length -= ptr_iface->bLength;
		ptr_iface = (usb_iface_desc_t *)((uintptr_t)ptr_iface +
				ptr_iface->bLength);
	}
	return false;
}

void main_event_aoa(uhc_device_t *dev, bool b_present)
{
	if (b_present == false) {
		/* Stop Android Device */
		uhi_aoa_dev.dev = NULL;
	} else {
		/* Start to communicate with Android device */
		uhi_aoa_configure(dev);
	}
}

bool uhi_aoa_read(uint8_t *payload, uint16_t payload_size,
		uhd_callback_trans_t callback_end)
{
	return uhd_ep_run(uhi_aoa_dev_sel->dev->address, uhi_aoa_dev.ep_in,
			false, payload, payload_size, 100, callback_end);
}

bool uhi_aoa_write(uint8_t *payload, uint16_t payload_size,
		uhd_callback_trans_t callback_end)
{
	return uhd_ep_run(uhi_aoa_dev_sel->dev->address, uhi_aoa_dev.ep_out,
			false, payload, payload_size, 100, callback_end);
}

/** @} */
/** @} */
