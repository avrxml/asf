/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB device mouse Audio task.
 *
 * This file manages the USB device mouse Audio task.
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


#ifndef _DEVICE_AUDIO_TASK_H_
#define _DEVICE_AUDIO_TASK_H_


//_____ I N C L U D E S ____________________________________________________

#include "conf_usb.h"

#if USB_DEVICE_FEATURE == false
  #error device_audio_task.h is #included although USB_DEVICE_FEATURE is disabled
#endif


//_____ D E F I N I T I O N S ______________________________________________

//! Define for each 'feature unit' the max numbers of bmaControls
#define MAX_BMA_CONTROLS 4

//typedef struct
//{
//   uint8_t  unit;
//   uint8_t  n_bmaControls;
//   uint8_t  bmaControls[MAX_BMA_CONTROLS];
//} cs_feature_unit_t;

//! Define for each 'feature unit' the max numbers of bmaControls
//! One of them is used by the microphone!
//#define MAX_FEATURE_UNITS 3

typedef struct
{
  volatile uint32_t g_sample_freq;
  volatile uint8_t  g_n_channels;
  volatile uint8_t  g_n_bits_per_sample;
           uint16_t byte_count;

  uint8_t  interf_audio_stream;

  bool sample_rate_set;
} device_audio_task_data_t;

//_____ M A C R O S ________________________________________________________


//_____ D E C L A R A T I O N S ____________________________________________

extern void device_audio_task_init(void);
#ifdef FREERTOS_USED
extern void device_audio_task(void *pvParameters);
#else
extern void device_audio_task(void);
#endif
extern void usb_sof_action(void);
extern void device_audio_enable_microphone(void);
extern void device_audio_disable_microphone(void);

#endif  // _DEVICE_AUDIO_TASK_H_
