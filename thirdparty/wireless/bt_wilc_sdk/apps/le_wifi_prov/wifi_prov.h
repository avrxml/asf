/**
 *
 * \file
 *
 * \brief Header file for WIFI related activity for provision AP.
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
#include "lwip/ip.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/dns.h"
#include "os/include/net_init.h"
#include "os/include/m2m_wifi_ex.h"
#include "tinyservices.h"
#include "conf_wilc.h"

#define MAIN_WLAN_WEP_KEY_INDEX         1 /**< WEP key index */

#define MAIN_WLAN_WEP_AUTH_TYPE   	 	WEP_ANY

typedef enum{
	WIFI_BT_INIT = 1,
	BT_WIFI_SCAN,
	BT_WIFI_CONNECT	
	}wifi_bt_intf_msg_t;
	
typedef enum{
	WIFI_CONNECT_FAIL = 0,
	WIFI_CONNECT_INPROGRESS,
	WIFI_CONNECT_SUCCESS
	}wifi_connect_status_t;	
	
typedef enum{
	SCANLIST_STOP = 0,
	SCANLIST_SCANNING,
	SCANLIST_UPDATED
	}wifi_scan_status_t;	

typedef struct {
	uint32_t id;
} wifi_bt_if_msg_t ;

typedef struct{
	uint8_t	 security_type;
	sint8 	 rssi_value;
	uint16_t ssid_len;
	uint8_t  ssid_val[M2M_MAX_SSID_LEN]; 	
}wifi_ap_detail;

typedef struct{
	uint8_t scan_mode;
	uint8_t 	ap_count;
	wifi_ap_detail ap_val[15];
}wifi_scan_ap_info;

typedef struct{
	uint8_t   conn_state_descriptor;
	uint8_t   conn_state;
	uint8_t   conn_param_offset;
	uint8_t   conn_param[98];
}wifi_conn_ap_info;

xQueueHandle wifi_bt_if_queue;

xSemaphoreHandle wifi_bt_if_sem;

xSemaphoreHandle wifi_bt_conn_if_sem;

tstrWifiInitParam param;

TaskHandle_t wifi_task_handle;

uint8_t scan_request_index;
uint8_t scan_ap_index;
uint8_t blank_ssid_num;

wifi_scan_ap_info scan_ap;
wifi_conn_ap_info conn_ap;
wifi_bt_if_msg_t msg;

void wifi_task(void *argument);









