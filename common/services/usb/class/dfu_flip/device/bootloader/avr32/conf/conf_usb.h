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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include "compiler.h"

//! To reduce the code size, the interrupt register management from IAR is not used
#if defined(__ICCAVR32__)
#  include "intc.h"
#  undef  irq_register_handler
#  define irq_register_handler(func, int_num, int_lvl)                    \
	INTC_register_interrupt(&func, int_num,                                \
			TPASTE2(AVR32_INTC_INT, int_lvl))
#  undef  ISR
#  define ISR(func, int_grp, int_lvl)    \
	   __interrupt static void func (void)
#endif

//! To link service clock routines used by usb driver to specific clock management
#include "sysclk_auto.h"
#define  sysclk_enable_usb  sysclk_auto_enable_usb
#define  sysclk_disable_usb  sysclk_auto_disable_usb

/**
 * USB Device Configuration
 * @{
 */

//! Device definition (mandatory)
#define  USB_DEVICE_VENDOR_ID             USB_VID_ATMEL
#if UC3A0 || UC3A1
#define  USB_DEVICE_PRODUCT_ID            USB_PID_ATMEL_DFU_AT32UC3A
#define  USB_DEVICE_PRODUCT_NAME          "DFU AT32UC3A"
#elif UC3B
#define  USB_DEVICE_PRODUCT_ID            USB_PID_ATMEL_DFU_AT32UC3B
#define  USB_DEVICE_PRODUCT_NAME          "DFU AT32UC3B"
#elif UC3A3
#define  USB_DEVICE_PRODUCT_ID            USB_PID_ATMEL_DFU_AT32UC3A3
#define  USB_DEVICE_PRODUCT_NAME          "DFU AT32UC3A3"
#elif UC3C
#define  USB_DEVICE_PRODUCT_ID            USB_PID_ATMEL_DFU_AT32UC3C
#define  USB_DEVICE_PRODUCT_NAME          "DFU AT32UC3C"
#elif UC3D
#define  USB_DEVICE_PRODUCT_ID            USB_PID_ATMEL_DFU_ATUC3D
#define  USB_DEVICE_PRODUCT_NAME          "DFU AT32UC3D"
#elif UC3L3_L4
#define  USB_DEVICE_PRODUCT_ID            USB_PID_ATMEL_DFU_ATUCL3_L4
#define  USB_DEVICE_PRODUCT_NAME          "DFU UC3 L3U/L4U"
#else
#error Unknow DFU PID for this AVR part
#endif

#define  USB_DEVICE_MAJOR_VERSION         (BOOTLOADER_VERSION>>4)
#define  USB_DEVICE_MINOR_VERSION         (BOOTLOADER_VERSION&0x0F)
#define  USB_DEVICE_POWER                 100 // Consumption on Vbus line (mA)
#define  USB_DEVICE_MANUFACTURE_NAME      "ATMEL"
#define  USB_DEVICE_ATTR                  USB_CONFIG_ATTR_SELF_POWERED

/**
 * Device speeds support
 * Only Full speed is authorized on DFU ATMEL
 * Because FLIP supports only full speed
 * Also:
 * - low speed is too slow (endpoint 8 Bytes)
 * - high speed does not increase flash program speed
 * - high speed is not supported on custom board with bad USB layout
 * @{
 */
// No speed change possible
//@}

/**
 * USB Device Callbacks definitions (Optional)
 * @{
 */
#define  UDC_VBUS_EVENT(b_vbus_high)
//@}

//@}


/**
 * USB Interface Configuration
 * @{
 */
/**
 * Configuration of DFU interface
 * @{
 */
//! Interface callback definition
#define  UDI_DFU_ENABLE_EXT()             true
#define  UDI_DFU_DISABLE_EXT()

//! FLIP protocol version to use
#define  FLIP_PROTOCOL_VERSION   FLIP_PROTOCOL_VERSION_2
// Split erase is available since batchisp 1.2.5 to avoid USB protocol 2 error
#define  UDI_DFU_ATMEL_PROTOCOL_2_SPLIT_ERASE_CHIP
//@}
//@}


/**
 * USB Device Driver Configuration
 * @{
 */
#define UDD_NO_SLEEP_MGR
//@}

//! The includes of classes and other headers must be done at the end of this file to avoid compile error
#include "udi_dfu_atmel_conf.h"
#include "main.h"
#include "conf_isp.h"   // Need to put bootloader version at USB device version

#endif // _CONF_USB_H_
