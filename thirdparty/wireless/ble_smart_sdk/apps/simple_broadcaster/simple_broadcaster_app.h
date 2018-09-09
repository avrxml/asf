/**
 * \file
 *
 * \brief Simple Broadcaster Application declarations
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
 */

/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Atmel
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
