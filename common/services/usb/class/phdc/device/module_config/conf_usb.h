/**
 * \file
 *
 * \brief USB configuration file
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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

#warning You must refill the following definitions with a correct values

/**
 * USB Device Configuration
 * @{
 */

/* ! Device definition (mandatory) */
#define  USB_DEVICE_VENDOR_ID             USB_VID_ATMEL
#define  USB_DEVICE_PRODUCT_ID            USB_PID_ATMEL_ASF_PHDC
#define  USB_DEVICE_MAJOR_VERSION         1
#define  USB_DEVICE_MINOR_VERSION         0
#define  USB_DEVICE_POWER                 100 /* Consumption on Vbus line (mA) */
#define  USB_DEVICE_ATTR \
	(USB_CONFIG_ATTR_SELF_POWERED)
/* (USB_CONFIG_ATTR_BUS_POWERED) */
/* (USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_SELF_POWERED) */
/* (USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED) */

/* ! USB Device string definitions (Optional) */
#define  USB_DEVICE_MANUFACTURE_NAME      "ATMEL AVR"
#define  USB_DEVICE_PRODUCT_NAME          "PHDC"
/* #define  USB_DEVICE_SERIAL_NAME           "12...EF" */

/**
 * Device speeds support
 * Low speed is not authorized
 * @{
 */
/* ! To authorize the High speed */
#if (UC3A3 || UC3A4)
/* #define  USB_DEVICE_HS_SUPPORT */
#elif (SAM3XA || SAM3U)
/* #define  USB_DEVICE_HS_SUPPORT */
#endif
/* @} */

/**
 * USB Device Callbacks definitions (Optional)
 * @{
 */
/* #define  UDC_VBUS_EVENT(b_vbus_high)
 *      user_callback_vbus_action(b_vbus_high) */
/* extern void user_callback_vbus_action(bool b_vbus_high); */
/* #define  UDC_SOF_EVENT()                  user_callback_sof_action() */
/* extern void user_callback_sof_action(void); */
/* #define  UDC_SUSPEND_EVENT()              user_callback_suspend_action() */
/* extern void user_callback_suspend_action(void); */
/* #define  UDC_RESUME_EVENT()               user_callback_resume_action() */
/* extern void user_callback_resume_action(void); */
/* ! Mandatory when USB_DEVICE_ATTR authorizes remote wakeup feature */
/* #define  UDC_REMOTEWAKEUP_ENABLE()        user_callback_remotewakeup_enable() */
/* extern void user_callback_remotewakeup_enable(void); */
/* #define  UDC_REMOTEWAKEUP_DISABLE()
 *       user_callback_remotewakeup_disable() */
/* extern void user_callback_remotewakeup_disable(void); */
/* ! When a extra string descriptor must be supported */
/* ! other than manufacturer, product and serial string */
/* #define  UDC_GET_EXTRA_STRING() */
/* @} */
/* @} */

/**
 * USB Interface Configuration
 * @{
 */

/**
 * Configuration of PHDC interface
 * @{
 */
/* ! Interface callback definition */
#define  UDI_PHDC_ENABLE_EXT()          true
#define  UDI_PHDC_DISABLE_EXT()

/* ! Select data format of application */
/* ! USB_PHDC_DATAMSG_FORMAT_VENDOR or USB_PHDC_DATAMSG_FORMAT_11073_20601 */
#define  UDI_PHDC_DATAMSG_FORMAT       USB_PHDC_DATAMSG_FORMAT_11073_20601
/* ! If USB_PHDC_DATAMSG_FORMAT_11073_20601 then define specialization(s) */
/* 4113(Dec) for BloodGlucose defined in 11073_20601 */
#define  UDI_PHDC_SPECIALIZATION       {0x1011}

/* ! Defines the QOS need for each transfer direction */

/*
#define  UDI_PHDC_QOS_OUT \
		(USB_PHDC_QOS_MEDIUM_BETTER|USB_PHDC_QOS_HIGH_BEST)
#define  UDI_PHDC_QOS_IN \
		(USB_PHDC_QOS_LOW_GOOD|USB_PHDC_QOS_MEDIUM_BETTER|USB_PHDC_QOS_MEDIUM_BEST)
*/
#define  UDI_PHDC_QOS_OUT \
	(USB_PHDC_QOS_MEDIUM_BEST)
#define  UDI_PHDC_QOS_IN \
	(USB_PHDC_QOS_MEDIUM_BEST)

/* ! The following METADATAs can be sent during USB enumeration
 * ! for each endpoints (optional)
 * ! It can define the "Opaque data" (Refer to IEEE11073) format for each endpoints
 * ! These must be up to 253 and recommended to be less than 30
 */

/*
#define  UDI_PHDC_METADATA_DESC_BULK_IN   {0x01,0x02,0x03}
#define  UDI_PHDC_METADATA_DESC_BULK_OUT  {0x01,0x02,0x03}
#define  UDI_PHDC_METADATA_DESC_INT_IN    {0x01,0x02,0x03}
*/

/* @} */
/* @} */

/**
 * USB Device Driver Configuration
 * @{
 * @}
 */

/* ! The includes of classes and other headers must be done at the end of this
 * file to avoid compile error */
#include "udi_phdc_conf.h"

#endif /* _CONF_USB_H_ */
