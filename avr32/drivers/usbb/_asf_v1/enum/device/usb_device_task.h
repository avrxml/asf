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
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
