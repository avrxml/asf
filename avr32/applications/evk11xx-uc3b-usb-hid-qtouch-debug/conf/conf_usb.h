/**************************************************************************
 *
 * \file
 *
 * \brief USB configuration file.
 *
 * This file contains the possible external configuration of the USB.
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


#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include "compiler.h"
#include "board.h"
#include "usb_ids.h"



//! @defgroup usb_general_conf USB application configuration
//!
//! @{

  // _________________ USB MODE CONFIGURATION ____________________________
  //
  //! @defgroup USB_op_mode USB operating modes configuration
  //! Defines to enable device or host USB operating modes
  //! supported by the application
  //! @{

    //! @brief ENABLE to activate the host software framework support
    //!
    //! Possible values true or false
#define USB_HOST_FEATURE            DIASBLE

    //! @brief ENABLE to activate the device software framework support
    //!
    //! Possible values true or false
#define USB_DEVICE_FEATURE          true

  //! @}

  // _________________ DEVICE MODE CONFIGURATION __________________________
  //
  //! @defgroup USB_device_mode_cfg USB device operating mode configuration
  //!
  //! @{

#if USB_DEVICE_FEATURE == true

#define NB_ENDPOINTS          3 // Number of endpoints in the application including control endpoint
#define EP_HID_GENERIC_IN     1
#define EP_HID_GENERIC_OUT    2

    //! @defgroup device_cst_actions USB device custom actions
    //!
    //! @{
      // Write here the action to associate with each USB event.
      // Be careful not to waste time in order not to disturb the functions.
#define Usb_sof_action()                usb_sof_action()
#define Usb_wake_up_action()
#define Usb_resume_action()
#define Usb_suspend_action()
#define Usb_reset_action()
#define Usb_vbus_on_action()
#define Usb_vbus_off_action()
#define Usb_set_configuration_action()
    //! @}

extern void usb_sof_action(void);
extern void usb_suspend_action(void);

#endif  // USB_DEVICE_FEATURE == true

  //! @}


  //! USB interrupt priority level
#define USB_INT_LEVEL                   AVR32_INTC_INT0

  //! Debug trace macro
#define LOG_STR(str)                    //print_dbg(str)


//! @}


#endif  // _CONF_USB_H_
