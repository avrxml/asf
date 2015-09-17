/**************************************************************************
 *
 * \file
 *
 * \brief USB configuration file.
 *
 * This file contains the possible external configuration of the USB.
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

#include "conf_audio_player.h"

#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include "compiler.h"
#include "board.h"
#include "usb_ids.h"

//! @defgroup usb_general_conf USB application configuration
//!
//! @{

  // _________________ HOST MODE CONFIGURATION ____________________________
  //
  //! @defgroup USB_host_mode_cfg USB host operating mode configuration
  //!
  //! @{

#if USB_HOST_FEATURE == true

    //! The size of RAM buffer reserved for descriptor handling
#define SIZEOF_DATA_STAGE        250

    //! The address that will be assigned to the connected device
#define DEVICE_ADDRESS           0x05

    //! The maximal number of endpoints per interface supported
#define MAX_EP_PER_INTERFACE     3

    //! The host controller will be limited to the strict VID/PID list.
    //! When enabled, if the device VID/PID does not belong to the supported list,
    //! the host controller software will not go to deeper configuration, but to error state.
#define HOST_STRICT_VID_PID_TABLE      DISABLE

    //! Try to configure the host pipe according to the device descriptors received
#define HOST_AUTO_CFG_ENDPOINT         ENABLE

    //! Host Start-of-Frame interrupt always enabled
#define HOST_CONTINUOUS_SOF_INTERRUPT  DISABLE

    //! When host error state detected, go to detached state
#define HOST_ERROR_RESTART             DISABLE

    //! USB host pipes transfers use USB communication interrupt (allows to use non-blocking functions)
#define USB_HOST_PIPE_INTERRUPT_TRANSFER  DISABLE

    //! Force CPU reset upon ID pin change
#define ID_PIN_CHANGE_GENERATE_RESET   ENABLE

    //! Reset callback - called just before the CPU reset takes in place
#if (UC3L || UC3C || UC3A3)
#include "twim.h"
#else // use the standard TWI module on UC3A0x and UC3A1x
#include "twi.h"
#endif
#include "pdca.h"
#define CPU_RESET_CALLBACK() { \
  extern void controller_shutdown(void); \
  extern void gui_shutdown(void); \
  extern void twi_disable_interrupt(volatile avr32_twi_t *twi); \
  pdca_disable_interrupt_transfer_complete(0); \
  pdca_disable_interrupt_reload_counter_zero(0); \
  controller_shutdown(); \
  gui_shutdown(); \
  twi_disable_interrupt(&AVR32_TWI); \
}
    //! Enable time-out delay for host transfer
#define TIMEOUT_DELAY_ENABLE           ENABLE

    //! Delay (unit is in 1/4 s - 250 ms) before time-out value
#define TIMEOUT_DELAY                  10

    //! Enable cpt NAK time-out for host transfer
#define NAK_TIMEOUT_ENABLE          DISABLE

    //! Number of NAK handshakes before time-out for transmit functions (up to 0xFFFF)
#define NAK_SEND_TIMEOUT            0x0FFF

    //! Number of NAK handshakes before time-out for receive functions (up to 0xFFFF)
#define NAK_RECEIVE_TIMEOUT         0x0FFF

  #if HOST_AUTO_CFG_ENDPOINT == DISABLE
    //! If no auto configuration of EP, map here user function
#define User_configure_endpoint()
  #endif

    //! The time in cycle number that the VBus need to be dropped down after a "device unplugged" has been detected
    //! if no timeouts are needed, then this constant should not be defined
#define HOST_VBUS_LOW_TIMEOUT       ((2000 * (unsigned long long) 60000000) / 1000) // (time_ms * FCPU_HZ) / 1000

    //! @defgroup host_cst_actions USB host custom actions
    //!
    //! @{
      // Write here the action to associate with each USB host event.
      // Be careful not to waste time in order not to disturb the functions.
#define Usb_id_transition_action()

#define Host_device_connection_action()
#define Host_sof_action()
#define Host_suspend_action()
#define Host_hwup_action()
#define Host_device_supported_action()
#define Host_device_not_supported_action()
#define Host_device_class_not_supported_action()
#define Host_device_error_action()
#ifndef Host_int_action
  #define Host_int_action()
#endif

#endif  // USB_HOST_FEATURE == true

  //! @}


  // _________________ DEVICE MODE CONFIGURATION __________________________
  //
  //! @defgroup USB_device_mode_cfg USB device operating mode configuration
  //!
  //! @{

#if USB_DEVICE_FEATURE == true

#define NB_ENDPOINTS          3 // Number of endpoints in the application including control endpoint
#define EP_AUDIO_OUT          5
#define EP_AUDIO_IN           6
#define EP_KBD_IN             1

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
#define Usb_set_configuration_action()  Usb_enable_sof_interrupt()
    //! @}

extern void usb_sof_action(void);
extern void usb_suspend_action(void);

#define USB_DEVICE_SCSI_FEATURE false

#endif  // USB_DEVICE_FEATURE == true

  //! @}

//! @}

  //! USB interrupt priority level
#define USB_INT_LEVEL                   3

  //! Debug trace macro
#define LOG_STR(str)                    ((void)(str))

//! @defgroup usb_stream_control USB stream control parameters
//! Defines the way the USB stream control will operate. The USB Stream Control embeds a mechanism
//! that ensures a good audio playback by keeping synchronized both Host and Device, even if their
//! sampling frequency are not strictly equivalent.
//! @{

//! Size of a buffer (in bytes) used in the USB stream FIFO. It shall be equivalent to the pipe/endpoint
//! from which the stream comes to.
#define USB_STREAM_BUFFER_SIZE        256   // Size in bytes.

//! Number of buffers used in the USB stream FIFO.
#define USB_STREAM_BUFFER_NUMBER       16   // Unit is in number of buffers. Must be a 2-power number.

//! Steps in PPM for the resynchronization.
#define USB_STREAM_RESYNC_PPM_STEPS    10   // Unit is in PPM.

//! Period (in ms) of the polling of the buffers monitoring.
#define TIMER_USB_STREAM_MONITOR      700   // Unit is in ms.

//! @brief true to activate the USB audio resynchronization
//!
//! Possible values true or false
#define USB_RESYNC_AUDIO_STREAM      true

//! @}


//! @}

#endif  // _CONF_USB_H_
