/**
 *
 * \file
 *
 * \brief Header file for BT related activity for provision AP.
 *
 * Copyright (c) 2018 Atmel Corporation. All rights reserved.
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

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "event_groups.h"

#define WIFI_CONN_STATE_HANDLE			0x0009
#define WIFI_CONN_STATE_CCD_HANDLE		0x000A
#define WIFI_AP_PARAM_HANDLE			0x000C
#define WIFI_SCAN_MODE_HANDLE			0x000F
#define WIFI_SCAN_MODE_CCD_HANDLE		0x0010
#define WIFI_AP_SCAN_HANDLE				0x0012
#define WIFI_AP_LIST_START_HANDLE		0x0013
#define WIFI_AP_LIST_END_HANDLE			0x0030

#define WIFI_MAX_SSID_LEN				32
#define WIFI_MAX_SEC_KEY_LEN			63


#define WIFI_AP_1_HANDLE				0x0014
#define WIFI_AP_2_HANDLE				0x0016
#define WIFI_AP_3_HANDLE				0x0018
#define WIFI_AP_4_HANDLE				0x001A
#define WIFI_AP_5_HANDLE				0x001C
#define WIFI_AP_6_HANDLE				0x001E
#define WIFI_AP_7_HANDLE				0x0020
#define WIFI_AP_8_HANDLE				0x0022
#define WIFI_AP_9_HANDLE				0x0024
#define WIFI_AP_10_HANDLE				0x0026
#define WIFI_AP_11_HANDLE				0x0028
#define WIFI_AP_12_HANDLE				0x002A
#define WIFI_AP_13_HANDLE				0x002C
#define WIFI_AP_14_HANDLE				0x002E
#define WIFI_AP_15_HANDLE				0x0030

#define WIFI_AP_1_INDEX				0
#define WIFI_AP_2_INDEX				1
#define WIFI_AP_3_INDEX				2
#define WIFI_AP_4_INDEX				3
#define WIFI_AP_5_INDEX				4
#define WIFI_AP_6_INDEX				5
#define WIFI_AP_7_INDEX				6
#define WIFI_AP_8_INDEX				7
#define WIFI_AP_9_INDEX				8
#define WIFI_AP_10_INDEX			9
#define WIFI_AP_11_INDEX			10
#define WIFI_AP_12_INDEX			11
#define WIFI_AP_13_INDEX			12
#define WIFI_AP_14_INDEX			13
#define WIFI_AP_15_INDEX			14

#define SECURITY_BYTE_SIZE		1
#define RSSI_VALUE_SIZE			1
#define SSID_LEN_SIZE			1
#define SSID_VAL_SIZE			WIFI_MAX_SSID_LEN
#define SECURITY_KEY_LEN		1
#define SECURITY_KEY_SIZE		WIFI_MAX_SEC_KEY_LEN
#define CONN_PARAM_SIZE		    (SECURITY_BYTE_SIZE + SSID_LEN_SIZE + SSID_VAL_SIZE + SECURITY_KEY_LEN + SECURITY_KEY_SIZE)

#define SECURITY_BYTE_INDEX		0
#define RSSI_VALUE_INDEX		1
#define SSID_LEN_INDEX			2
#define SSID_VAL_INDEX			3

#define CONN_SSID_LEN_INDEX				1
#define CONN_SSID_VAL_INDEX				2
#define CONN_SECURITY_KEY_LEN_INDEX		34
#define CONN_SECURITY_KEY_SIZE_INDEX	35

#define RES_SUCCESS						0x0000									
#define RES_FAIL						0x0001									

#define WIFICON_UUID(ID16)  0x1b, 0xc5, 0xd5, 0xa5, 0x02, 0x00, 0x89, 0x86, 0xe4, 0x11, 0x29, 0xd2, (ID16&0xff), (ID16>>8), 0x88, 0x77

typedef enum
{
	BT_IDLE,
	BT_AP_SCANNING,
	BT_AP_LIST_UPDATED,
	BT_AP_CONN_STATE,
	BT_AP_CONN_STATE_UPDATED
}bt_state_t;

TaskHandle_t bt_task_handle;
int btstack_main(void);
void bt_task(void *arg);
void le_prov_app_setup(void);
void le_prov_adv_start(void);







