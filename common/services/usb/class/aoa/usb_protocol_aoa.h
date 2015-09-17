/**
 * \file
 *
 * \brief USB Android Open Accessory protocol definitions.
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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

#ifndef _USB_PROTOCOL_AOA_H_
#define _USB_PROTOCOL_AOA_H_

/**
 * \ingroup usb_protocol_group
 * \defgroup usb_aoa_protocol USB Android Open Accessory
 *
 * @{
 */

/**
 * \brief Android Open Accessory USB requests (bRequest)
 */
/** Get AOA Protocol version */
#define USB_REQ_AOA_GET_PROTOCOL      0x33
/** Sending identifying str info */
#define USB_REQ_AOA_SEND_IDENT_STRING 0x34
/** Start the Android Device in acc mode */
#define USB_REQ_AOA_STARTUP           0x35

/** Google Vendor ID */
#define GOOGLE_VID              0x18D1
/** Google Product ID which indicates it supports Android Accessory Device */
#define AOA_PID                 0x2D00
/** Google Product ID which indicates it supports
 * Android Accessory Device which supports ADB */
#define AOA_ADB_PID             0x2D01

/** Index of manufacturer */
#define AOA_STRING_INDEX_MANUFACTURER  0
/** Index of model */
#define AOA_STRING_INDEX_MODEL         1
/** Index of description */
#define AOA_STRING_INDEX_DESCRIPTION   2
/** Index of version */
#define AOA_STRING_INDEX_VERSION       3
/** Index of URL */
#define AOA_STRING_INDEX_URL           4
/** Index of serialnumber */
#define AOA_STRING_INDEX_SERIAL        5

/** @} */

#endif /* _USB_PROTOCOL_AOA_H_ */
