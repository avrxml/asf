/**************************************************************************
 *
 * \file
 *
 * \brief Processing of USB device enumeration requests.
 *
 * This file contains the USB control endpoint management
 * routines corresponding to the standard enumeration process (refer to
 * chapter 9 of the USB specification).
 * This file calls routines of the usb_specific_request.c file for
 * non-standard request management.
 * The enumeration parameters (descriptor tables) are contained in the
 * usb_descriptors.c file.
 *
 * Copyright (c) 2009 Atmel Corporation. All rights reserved.
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


#ifndef _USB_STANDARD_REQUEST_H_
#define _USB_STANDARD_REQUEST_H_


//_____ I N C L U D E S ____________________________________________________

#include "conf_usb.h"

#if USB_DEVICE_FEATURE == false
  #error usb_standard_request.h is #included although USB_DEVICE_FEATURE is disabled
#endif


#include "usb_task.h"
#include "usb_descriptors.h"


//! @defgroup std_request USB device standard requests decoding module
//! @{


//_____ M A C R O S ________________________________________________________


//_____ S T A N D A R D    D E F I N I T I O N S ___________________________

        // Device State
#define ATTACHED                          0
#define POWERED                           1
#define DEFAULT                           2
#define ADDRESSED                         3
#define CONFIGURED                        4
#define SUSPENDED                         5

#define USB_CONFIG_ATTRIBUTES_RESERVED    0x80
#define USB_CONFIG_BUSPOWERED            (USB_CONFIG_ATTRIBUTES_RESERVED | 0x00)
#define USB_CONFIG_SELFPOWERED           (USB_CONFIG_ATTRIBUTES_RESERVED | 0x40)
#define USB_CONFIG_REMOTEWAKEUP          (USB_CONFIG_ATTRIBUTES_RESERVED | 0x20)


//_____ D E C L A R A T I O N S ____________________________________________

  //! @brief Returns true when device connected and correctly enumerated with a host.
  //! The device high-level application should test this before performing any applicative request.
#define Is_device_enumerated()            (usb_configuration_nb != 0)

  //! This function reads the SETUP request sent to the default control endpoint
  //! and calls the appropriate function. When exiting of the usb_read_request
  //! function, the device is ready to manage the next request.
  //!
  //! If the received request is not supported or a non-standard USB request, the function
  //! will call the custom decoding function in usb_specific_request module.
  //!
  //! @note List of supported requests:
  //! GET_DESCRIPTOR
  //! GET_CONFIGURATION
  //! SET_ADDRESS
  //! SET_CONFIGURATION
  //! CLEAR_FEATURE
  //! SET_FEATURE
  //! GET_STATUS
  //!
extern void usb_process_request(void);

extern volatile U8 usb_configuration_nb;


//! @}


#endif  // _USB_STANDARD_REQUEST_H_
