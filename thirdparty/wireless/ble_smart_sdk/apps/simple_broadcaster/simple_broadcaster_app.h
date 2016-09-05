/**
 * \file
 *
 * \brief Simple Broadcaster Application declarations
 *
 * Copyright (c) 2014-2016 Atmel Corporation. All rights reserved.
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel
 * Support</a>
 */

#ifndef __SIMPLE_BROADCASTER_APP_H__
#define __SIMPLE_BROADCASTER_APP_H__

#include <asf.h>
#include "platform.h"
#include "at_ble_api.h"
#include "console_serial.h"
#include "led.h"
#include "timer_hw.h"
#include "ble_manager.h"
#include "ble_utils.h"

/* 1000 ms */
#define APP_BROADCAST_FAST_ADV                  (1600)

/* 10 min */
#define APP_BROADCAST_ADV_TIMEOUT               (655)

/* maximum length of advertisement data */
#define MAX_ADV_LEN								(28)

/* maximum length of scan response data */
#define MAX_SCAN_LEN MAX_ADV_LEN

/* /Advertisement types */
#define ADV_TYPE_SCANNABLE_UNDIRECTED AT_BLE_ADV_TYPE_SCANNABLE_UNDIRECTED
#define ADV_TYPE_NONCONN_UNDIRECTED AT_BLE_ADV_TYPE_NONCONN_UNDIRECTED

/* /Advertisement data type length */
#define ADV_TYPE_LEN							(0x01)

/** @brief Return status to app from the APIs provided
 */
typedef enum {
	/* /Success */
	STATUS_SUCCEEDED = 0,
	/* /adv_data or scan_rsp_data have reached maximum length */
	STATUS_MAX_LENGTH_REACHED,
	/* /Input parameter invalid */
	STATUS_INVALID_PARAM,
	/* /Trying to send incomplete data on scan response data */
	STATUS_OPERATION_NOT_SUPPORTED,
	/* /Failure */
	STATUS_FAILED
} status_t;

/** @brief data type, configurable by app to specify which part of the data(scan
 * response or advertisement)
 *                 should contain the data given by app.
 */
typedef enum {
	ADVERTISEMENT_DATA = 0,
	SCAN_RESP_DATA
} data_type_t;

/** Advertisement data types
 */
typedef enum {
	ADV_DATA_TYPE_INCOMPLETE_SERVICE_UUID16 = 2,
	ADV_DATA_TYPE_COMPLETE_SERVICE_UUID16,
	ADV_DATA_TYPE_INCOMPLETE_SERVICE_UUID32,
	ADV_DATA_TYPE_COMPLETE_SERVICE_UUID32,
	ADV_DATA_TYPE_INCOMPLETE_SERVICE_UUID128,
	ADV_DATA_TYPE_COMPLETE_SERVICE_UUID128,
	ADV_DATA_TYPE_SHORTENED_LOCAL_NAME,
	ADV_DATA_TYPE_COMPLETE_LOCAL_NAME,
	ADV_DATA_TYPE_APPEARANCE = 0x019,
	ADV_DATA_TYPE_MANUFACTURER_DATA = 0xff
} adv_data_type_t;

#define BRD_ADV_DATA_UUID_LEN				(2)
#define BRD_ADV_DATA_UUID_DATA              "\x00\x18"

#define BRD_ADV_DATA_APPEARANCE_LEN			(2)
#define BRD_ADV_DATA_APPEARANCE_DATA		"\x80\x00"

#define BRD_ADV_DATA_NAME_LEN               (0x0b)
#define BRD_ADV_DATA_NAME_DATA              "ATMEL-BROADCAST"

#define BRD_ADV_DATA_MANUFACTURER_LEN		(0x08)
#define BRD_ADV_DATA_MANUFACTURER_DATA		"\x00\x06\xd6\xb2\xf0\x05\xf0\xf8"

#endif /* __SIMPLE_BROADCASTER_APP_H__ */
