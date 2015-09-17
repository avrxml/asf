/**************************************************************************
 *
 * \file
 *
 * \brief Processing of USB host user process.
 *
 * This file contains the specific request decoding for enumeration process.
 *
 * - Supported hosts:    All AVR32 hosts with a USB module can be used.
 *
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


#ifndef _USB_HOST_USER_H_
#define _USB_HOST_USER_H_


//_____ I N C L U D E S ____________________________________________________

#include "conf_usb.h"

#if USB_HOST_FEATURE == false
#  error usb_host_user.h is #included although USB_HOST_FEATURE is disabled
#endif


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

#define OFFSET_FIELD_DESCRIPTOR_SUBTYPE    2

//! @defgroup host_user USB host specific requests
//! @{

//! Initialization
//!
extern void host_user_check_class_init(void);

//! This function is called by the standard USB host_check_class() function when
//! the Host is analyzing the configuration descriptor previously sent by a USB Device.
//! The function will be called for each descriptor found in the configuration descriptor.
//!
extern bool host_user_check_class(const uint8_t *);
//! @}


// ____ T Y P E  D E F I N I T I O N _______________________________________


#endif  // _USB_HOST_USER_H_
