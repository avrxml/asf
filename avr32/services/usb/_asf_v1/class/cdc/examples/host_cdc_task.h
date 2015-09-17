/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB host CDC task.
 *
 * This file manages the USB host CDC task.
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


#ifndef _HOST_CDC_TASK_H_
#define _HOST_CDC_TASK_H_


//_____ I N C L U D E S ____________________________________________________

#include "conf_usb.h"

#if USB_HOST_FEATURE == false
  #error host_cdc_task.h is #included although USB_HOST_FEATURE is disabled
#endif


#include "usb_host_task.h"


//_____ M A C R O S ________________________________________________________

#define Is_host_cdc_configured()  (cdc_connected && !Is_host_suspended())


//_____ D E F I N I T I O N S ______________________________________________

// CDC Custom configuration

//! @brief Enable or not the access to Management Element (Interrupt Pipe)
//!
//! Possible values true or false
#define     CDC_USE_MANAGEMENT_INTERFACE        true

//! @brief Enable or not the USART to USB gateway features & functions
//!
//! Possible values true or false
#define     CDC_USE_UART                        true

//! @brief Size of transmit buffer (must be less or equal than pipe size !)
//!
//! @note: Irrelevant when CDC_USE_UART is false.
#define     CDC_STREAM_OUT_SIZE                 0x10

//! @brief Size of receive buffer (must be less or equal than pipe size !)
//!
//! @note: Irrelevant when CDC_USE_UART is false.
#define     CDC_STREAM_IN_SIZE                  0x10

//! @brief Data Pipe flush timeout in milli-second.
#define     CDC_NB_MS_BEFORE_FLUSH              5

//! @brief Set here the length of your encapsulated command (user defined)
#define     ENCAPSULATED_PACKET_LENGTH          0x0A


#define LED_HOST_CDC_B0     LED_MONO0_GREEN
#define LED_HOST_CDC_B1     LED_MONO1_GREEN
#define LED_HOST_CDC_B2     LED_MONO2_GREEN
#define LED_HOST_CDC_B3     LED_MONO3_GREEN


//_____ D E C L A R A T I O N S ____________________________________________

extern volatile bool ms_connected;

extern void host_cdc_send_encapsulated_command(void);
extern void host_cdc_get_encapsulated_response(void);
extern void host_cdc_set_line_coding(void);
extern void host_cdc_get_line_coding(void);
extern void host_cdc_task_init(void);
#ifdef FREERTOS_USED
extern void host_cdc_task(void *pvParameters);
#else
extern void host_cdc_task(void);
#endif
extern void cdc_pipe_out_usb_flush(void);
extern void host_sof_action(void);

extern uint8_t  cdc_stream_out_array[CDC_STREAM_OUT_SIZE];    // stream array for OUT pipe (outgoing data)
extern uint8_t  cdc_stream_in_array[CDC_STREAM_IN_SIZE];      // stream array for IN pipe (incoming data)


#endif  // _HOST_CDC_TASK_H_
