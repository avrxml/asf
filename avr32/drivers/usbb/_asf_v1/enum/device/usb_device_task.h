/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB device controller.
 *
 * The USB task checks the income of new requests from the USB host.
 * When a setup request occurs, this task launches the processing of this
 * setup contained in the usb_standard_request.c file.
 * Other class-specific requests are also processed in this file.
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


#ifndef _USB_DEVICE_TASK_H_
#define _USB_DEVICE_TASK_H_


#include "conf_usb.h"

#if USB_DEVICE_FEATURE == false
  #error usb_device_task.h is #included although USB_DEVICE_FEATURE is disabled
#endif


//! @defgroup usb_device_task USB device task module
//! @{

//_____ I N C L U D E S ____________________________________________________


//_____ M A C R O S ________________________________________________________


//_____ D E C L A R A T I O N S ____________________________________________

//!
//! @brief This function initializes the USB device controller.
//!
//! This function enables the USB controller and inits the USB interrupts.
//! The aim is to allow the USB connection detection in order to send
//! the appropriate USB event to the operating mode manager.
//!
extern void usb_device_task_init(void);

//!
//! @brief This function starts the USB device controller.
//!
//! This function enables the USB controller and inits the USB interrupts.
//! The aim is to allow the USB connection detection in order to send
//! the appropriate USB event to the operating mode manager.
//! Start device function is executed once VBus connection has been detected
//! either by the VBus change interrupt or by the VBus high level.
//!
extern void usb_start_device(void);

//!
//! @brief Entry point of the USB device management
//!
//! This function is the entry point of the USB management. Each USB
//! event is checked here in order to launch the appropriate action.
//! If a Setup request occurs on the Default Control Endpoint,
//! the usb_process_request() function is call in the usb_standard_request.c file
//!
#ifdef FREERTOS_USED
extern void usb_device_task(void *pvParameters);
#else
extern void usb_device_task(void);
#endif

//! @}


#endif  // _USB_DEVICE_TASK_H_
