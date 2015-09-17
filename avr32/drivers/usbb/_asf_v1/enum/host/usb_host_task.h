/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB host controller.
 *
 * This file manages the host controller, the host enumeration process and
 * the suspend/resume host requests.
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


#ifndef _USB_HOST_TASK_H_
#define _USB_HOST_TASK_H_


#include "conf_usb.h"

#if USB_HOST_FEATURE == false
  #error usb_host_task.h is #included although USB_HOST_FEATURE is disabled
#endif


//! @defgroup usb_host_task USB host task module
//! @{

//_____ I N C L U D E S ____________________________________________________

#include "compiler.h"
#include "usb_host_enum.h"


//_____ T Y P E S  _________________________________________________________

typedef void Pipe_handler(Status_t status, U16 nb_byte);

typedef struct
{
  bool          enable;
  U16           nb_byte_to_process;
  U16           nb_byte_processed;
  U16           nb_byte_on_going;
  void         *ptr_buf;
  Pipe_handler *handler;
  Status_t      status;
  U8            timeout;
  U16           nak_timeout;
} S_pipe_int;


//_____ M A C R O S ________________________________________________________

#define PIPE_GOOD           0x00
#define PIPE_DATA_TOGGLE    0x01
#define PIPE_DATA_PID       0x02
#define PIPE_PID            0x04
#define PIPE_TIMEOUT        0x08
#define PIPE_CRC16          0x10
#define PIPE_STALL          0x20
#define PIPE_NAK_TIMEOUT    0x40
#define PIPE_DELAY_TIMEOUT  0x80

//! @brief Returns true when device connected and correctly enumerated.
//! The host high-level application should test this before performing any applicative request.
#define Is_host_ready()                   (device_state == DEVICE_READY)

//! Check if host controller is in suspend mode
#define Is_host_suspended()               (device_state == DEVICE_WAIT_RESUME ||\
                                           device_state == DEVICE_SUSPENDED)

//! Check if there is an error
#define Is_host_error()                   (device_state == DEVICE_ERROR)

//! Check if there is a device addressed by the host
#define Is_host_addressed()               (device_state >= DEVICE_ADDRESSED)

//! Check if there is a device powered by the host
#define Is_host_powered()                 (device_state >= DEVICE_POWERED)

//! Check if there is a device attached to the host
#define Is_host_attached()                (device_state >= DEVICE_ATTACHED)

//! Should be called to make the host controller enter USB suspend mode
#define Host_request_suspend()            (device_state = DEVICE_SUSPENDED)

//! Should be called to request the host controller to resume the USB bus
#define Host_request_resume()             (request_resume = true)

//! Private ack for resume software event
#define Host_ack_request_resume()         (request_resume = false)

//! Private check for resume sequence
#define Is_host_request_resume()          (request_resume == true)

//! @defgroup device_state_value Host controller states
//! Defines for device state coding
//! \image html usb_host_task.jpg "USB Host Task Overview"
//! @{
#define DEVICE_UNATTACHED         0
#define DEVICE_ATTACHED           1
#define DEVICE_POWERED            2
#define DEVICE_DEFAULT            3
#define DEVICE_ADDRESSED          4
#define DEVICE_CONFIGURED         5
#define DEVICE_READY              6

#define DEVICE_ERROR              7

#define DEVICE_SUSPENDED          8
#define DEVICE_WAIT_RESUME        9

#define Host_set_device_supported()       (Set_bits(device_status, 0x01))
#define Host_clear_device_supported()     (Clr_bits(device_status, 0x01))
#define Is_host_device_supported()        (Tst_bits(device_status, 0x01))

#define Host_set_device_ready()           (Set_bits(device_status, 0x02))
#define Host_clear_device_ready()         (Clr_bits(device_status, 0x02))
#define Is_host_device_ready()            (Tst_bits(device_status, 0x02))

#define Host_set_configured()             (Set_bits(device_status, 0x04))
#define Host_clear_configured()           (Clr_bits(device_status, 0x04))
#define Is_host_configured()              (Tst_bits(device_status, 0x04))

#define Host_clear_device_status()        (device_status = 0x00)
//! @}


//_____ D E C L A R A T I O N S ____________________________________________

//!
//! @brief This function initializes the USB host controller.
//!
//! This function enables the USB controller for host-mode operation.
//!
extern void usb_host_task_init(void);

//!
//! @brief Entry point of the USB host management
//!
//! The aim is to manage the target device connection and enumeration.
//! Depending on device_state, the function performs the required operations
//! to get the device enumerated and configured.
//! Once the device is operational, device_state is DEVICE_READY.
//! This state should be tested by the host task before sending any
//! applicative request to the device.
//! This function is called from the usb_task function depending on the USB
//! operating mode (device or host) currently engaged.
//!
#ifdef FREERTOS_USED
extern void usb_host_task(void *pvParameters);
#else
extern void usb_host_task(void);
#endif

//!
//! @brief This function sends nb_data bytes pointed to by ptr_buf on the specified pipe.
//!
//! @note This function activates the host SOF interrupt to detect time-outs.
//! The initial enable state of this interrupt will be restored.
//!
//! @param pipe
//! @param nb_data
//! @param ptr_buf
//!
//! @return Status_t: Pipe status
//!
extern Status_t host_send_data(U8 pipe, U16 nb_data, const void *ptr_buf);

//!
//! @brief This function receives nb_data bytes pointed to by ptr_buf on the specified pipe.
//!
//! *nb_data is updated with the final number of data bytes received.
//!
//! @note This function activates the host SOF interrupt to detect time-outs.
//! The initial enable state of this interrupt will be restored.
//!
//! @param pipe
//! @param nb_data
//! @param ptr_buf
//!
//! @return Status_t: Pipe status
//!
extern Status_t host_get_data(U8 pipe, U16 *nb_data, void *ptr_buf);

#if USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE

extern void reset_it_pipe_str(void);

extern bool is_any_interrupt_pipe_active(void);

//!
//! @brief This function sends nb_data bytes pointed to by ptr_buf on the specified pipe.
//!
//! @param pipe
//! @param nb_data
//! @param ptr_buf
//! @param handler Call-back function pointer
//!
//! @return bool: Status
//!
extern bool host_send_data_interrupt(U8 pipe, U16 nb_data, const void *ptr_buf, Pipe_handler *handler);

//!
//! @brief This function receives nb_data bytes pointed to by ptr_buf on the specified pipe.
//!
//! *nb_data is updated with the final number of data bytes received.
//!
//! @param pipe
//! @param nb_data
//! @param ptr_buf
//! @param handler Call-back function pointer
//!
//! @return bool: Status
//!
extern bool host_get_data_interrupt(U8 pipe, U16 nb_data, void *ptr_buf, Pipe_handler *handler);

//!
//! @brief USB pipe interrupt subroutine
//!
//! @param pipe
//!
extern void usb_pipe_interrupt(U8 pipe);

#endif  // USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE

extern volatile U8 device_state;
extern volatile S_usb_setup_data usb_request;
extern U8 data_stage[SIZEOF_DATA_STAGE];
extern volatile U8 device_status;
extern volatile bool request_resume;

//! @}


#endif  // _USB_HOST_TASK_H_
