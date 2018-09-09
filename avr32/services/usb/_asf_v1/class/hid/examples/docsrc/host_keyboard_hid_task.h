/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB host keyboard HID task.
 *
 * This file manages the USB host keyboard HID task.
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


#ifndef _HOST_KEYBOARD_HID_TASK_H_
#define _HOST_KEYBOARD_HID_TASK_H_


//_____ I N C L U D E S ____________________________________________________

#include "conf_usb.h"

#if USB_HOST_FEATURE == false
  #error host_keyboard_hid_task.h is #included although USB_HOST_FEATURE is disabled
#endif


#include "usb_host_task.h"
#include "fs_com.h"


//_____ M A C R O S ________________________________________________________

#define Is_host_ms_configured()     (ms_connected && !Is_host_suspended())


//_____ D E F I N I T I O N S ______________________________________________

#define DEVICE_TO_HOST        0x01
#define HOST_TO_DEVICE        0x02
#define FULL_SYNC             (DEVICE_TO_HOST | HOST_TO_DEVICE)

#define DIR_LOCAL_OUT_NAME    "OUT"
#define DIR_LOCAL_IN_NAME     "IN"

#define DIR_USB_OUT_NAME      "OUT"
#define DIR_USB_IN_NAME       "IN"


//_____ D E C L A R A T I O N S ____________________________________________

extern volatile bool ms_connected;

extern void host_keyboard_hid_task_init(void);
#ifdef FREERTOS_USED
extern void host_keyboard_hid_task(void *pvParameters);
#else
extern void host_keyboard_hid_task(void);
#endif
extern void host_sof_action(void);

extern bool host_mass_storage_task_sync_dir(Fs_index *dst_fs_idx, const char *dst_dir, Fs_index *src_fs_idx, const char *src_dir);
extern bool host_mass_storage_task_sync_drives(uint8_t sync_direction);


#endif  // _HOST_KEYBOARD_HID_TASK_H_
