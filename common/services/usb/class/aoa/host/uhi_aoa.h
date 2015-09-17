/**
 * \file
 *
 * \brief Android Open Accessory Header File
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef _UHI_AOA_H_
#define _UHI_AOA_H_

#if !(UC3A || UC3B)
#error Unsupported Device
#endif

#include <string.h>
#include "conf_usb_host.h"
#include "usb_protocol_aoa.h"
#include "uhc.h"
#include "uhi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup uhi_group
 * \defgroup uhi_aoa_group UHI for Android Open Accessory
 * @{
 */

/** Global structure which contains standard UHI API for UHC */
#define UHI_AOA { \
		.install = uhi_aoa_install, \
		.enable = uhi_aoa_enable, \
		.uninstall = uhi_aoa_uninstall, \
		.sof_notify = NULL, \
}

/**
 * Functions required by UHC
 *
 */
extern uhc_enum_status_t uhi_aoa_install(uhc_device_t *dev);
extern void uhi_aoa_enable(uhc_device_t *dev);
extern void uhi_aoa_uninstall(uhc_device_t *dev);

/**
 * \name Routines to initialize the Android device
 * @{
 */

/*! \brief Function to send Identifying Information to the Android Device
 *
 */
void uhi_aoa_send_info_string(uint8_t, char *, uhd_callback_setup_end_t);

/**
 * \brief Tries to configure bulk Endpoints for the Android Accessory
 *
 * \return true, if Configuration was successful
 */
bool uhi_aoa_configure(uhc_device_t *);

/**
 * \brief Takes care of Devices being attached/detached
 *
 */
void main_event_aoa(uhc_device_t *, bool);

/** @} */

/**
 * \name Routines to communicate with the Android device
 * @{
 */

/**
 * \brief Main Function to receive a message from an Android device
 *
 * \param payload        Pointer on the data to transfer
 * \param payload_size   Size of the data to transfer
 * \param callback_end   Callback to call at the end of transfer
 */
bool uhi_aoa_read(uint8_t *payload, uint16_t payload_size,
		uhd_callback_trans_t callback_end);

/**
 * \brief Main Function to send a message to an Android device
 *
 * \param payload        Pointer on the data to transfer
 * \param payload_size   Size of the data to transfer
 * \param callback_end   Callback to call at the end of transfer
 */
bool uhi_aoa_write(uint8_t *payload, uint16_t payload_size,
		uhd_callback_trans_t callback_end);

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /** _UHI_AOA_H_ */
