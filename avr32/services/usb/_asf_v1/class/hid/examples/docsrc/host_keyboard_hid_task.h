/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB host keyboard HID task.
 *
 * This file manages the USB host keyboard HID task.
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
