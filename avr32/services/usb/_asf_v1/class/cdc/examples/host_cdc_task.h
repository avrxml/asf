/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB host CDC task.
 *
 * This file manages the USB host CDC task.
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
