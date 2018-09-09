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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "malloc.h"

#include "wifi_prov.h"

/**
 * \brief Callback to get the Wi-Fi status update.
 *
 * \param[in] msg_type Type of Wi-Fi notification.
 * \param[in] wifi_msg A pointer to a buffer containing the notification parameters.
 *
 * \return None.
 */
static void wifi_cb(uint8 msg_type, void *wifi_msg)
{
	switch (msg_type) 
	{
	case M2M_WIFI_RESP_SCAN_DONE:
	{
		tstrM2mScanDone *pstrInfo = (tstrM2mScanDone *)wifi_msg;
		scan_request_index = 0;
		scan_ap_index = 0;
		blank_ssid_num = 0;
		
		/*Store number of scan AP, maximum limit is 15 as per BLE database*/
		if(pstrInfo->u8NumofCh > 15)
		{
			scan_ap.ap_count = 15;
		}
		else
		{
			scan_ap.ap_count = pstrInfo->u8NumofCh;	
		}				
		if (pstrInfo->u8NumofCh >= 1) {
			os_m2m_wifi_req_scan_result(scan_request_index);
			scan_request_index++;
		} else {
			os_m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
		}
		break;
	}

	case M2M_WIFI_RESP_SCAN_RESULT:
	{
		tstrM2mWifiscanResult *pstrScanResult = (tstrM2mWifiscanResult *)wifi_msg;
		
		if (strlen((char *)pstrScanResult->au8SSID) > 0)
		{
			/* display founded AP. */
			printf("\r\n[%d] SSID:%s\n", scan_ap_index, pstrScanResult->au8SSID);

			/*Store security type of scan AP*/
			scan_ap.ap_val[scan_ap_index].security_type = pstrScanResult->u8AuthType;
			
			/*Store RSSI value of scan AP*/
			scan_ap.ap_val[scan_ap_index].rssi_value = pstrScanResult->s8rssi;
			
			/*Store the SSID len of scan AP*/
			scan_ap.ap_val[scan_ap_index].ssid_len = strlen((char *)pstrScanResult->au8SSID);
			
			/*Store the SSID value of scan AP*/
			memcpy(scan_ap.ap_val[scan_ap_index].ssid_val, pstrScanResult->au8SSID, scan_ap.ap_val[scan_ap_index].ssid_len);
			scan_ap_index++;
		}
		else
		{
			/*Number of SSID found with null value*/
			blank_ssid_num++;
		}	
			
		if (scan_request_index < scan_ap.ap_count) 
		{
			os_m2m_wifi_req_scan_result(scan_request_index);
			scan_request_index++;
		}
		else
		{
			scan_ap.ap_count -= blank_ssid_num;
			scan_request_index = 0;
			scan_ap_index = 0;
			xSemaphoreGive(wifi_bt_if_sem);
			vTaskDelay(1);
			xSemaphoreTake(wifi_bt_if_sem, portMAX_DELAY); 
		}
		break;
	}
	
	case M2M_WIFI_RESP_CON_STATE_CHANGED :
	{
		tstrM2mWifiStateChanged *ctx = (tstrM2mWifiStateChanged*)wifi_msg;
		if (ctx->u8CurrState == M2M_WIFI_CONNECTED)
		{
			printf("\r\nWiFi Connected\n");
			conn_ap.conn_state = WIFI_CONNECT_SUCCESS;
			xSemaphoreGive(wifi_bt_conn_if_sem);
		}
		
		if (ctx->u8CurrState == M2M_WIFI_DISCONNECTED)
		{
			printf("\r\nWiFi Disconnected %d\n", ctx->u8ErrCode);
			conn_ap.conn_state = WIFI_CONNECT_FAIL;
			xSemaphoreGive(wifi_bt_conn_if_sem);
		}
		break;
	}
	
	default:
	{
		break;
	}

	}
}

/**
 * \brief STA task main function.
 */
void wifi_task(void *argument)
{
	
	int8_t ret;
	tuniM2MWifiAuth   sta_auth_param;
	
	/* Create WIFI and BT interface queue for communication */
	wifi_bt_if_queue = xQueueCreate(2, sizeof(wifi_bt_if_msg_t));
	
	/* Initialize the network stack. */
	net_init();
	
	/* Initialize the WILC3000 driver. */
	memset(&param, 0, sizeof(param));
	param.pfAppWifiCb = wifi_cb;
	
	ret = os_m2m_wifi_init(&param);
	if (M2M_SUCCESS != ret) {
		printf("main: m2m_wifi_init call error!(%d)\r\n", ret);
		while (1) {
		}
	}
	else
	{
		printf("main: m2m_wifi_init call successfully\r\n");
	}
	
	xSemaphoreTake(wifi_bt_conn_if_sem, portMAX_DELAY);
	msg.id = WIFI_BT_INIT;
	xQueueSend(wifi_bt_if_queue, (void *)&msg, portMAX_DELAY); 
	
	xSemaphoreTake(wifi_bt_if_sem, portMAX_DELAY);
	
	while (1) 
	{
		if(xQueueReceive(wifi_bt_if_queue, (void *)&msg, portMAX_DELAY) != pdFALSE)
		{
			/*Message received from BT task for scanning*/
			if(msg.id == BT_WIFI_SCAN)
			{
				printf("\r\nInitiate WIFI Scanning.\n");
				ret = os_m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
				if (M2M_SUCCESS != ret) {
					printf("\r\nWIFI Scan Fail.\n");
					while (1) {
					}
				}
			}
			
			/*Message received from BT task for connection*/
			if (msg.id == BT_WIFI_CONNECT)
			{
				printf("\r\nInitiate WIFI Connection\n");
				if (conn_ap.conn_param[0] == M2M_WIFI_SEC_OPEN)
				{
					sta_auth_param.au8PSK[0] = '\0';
				}
				else if (conn_ap.conn_param[0] == M2M_WIFI_SEC_WPA_PSK)
				{
					memcpy(&sta_auth_param.au8PSK[0], &conn_ap.conn_param[35], conn_ap.conn_param[34]);
					sta_auth_param.au8PSK[conn_ap.conn_param[34]]	= '\0';
				}
				else if (conn_ap.conn_param[0] == M2M_WIFI_SEC_WEP)
				{
					sta_auth_param.strWepInfo.u8KeyIndx = MAIN_WLAN_WEP_KEY_INDEX;
					sta_auth_param.strWepInfo.u8KeySz = (conn_ap.conn_param[34] + 1);
					memcpy(&sta_auth_param.strWepInfo.au8WepKey[0], &conn_ap.conn_param[35], conn_ap.conn_param[34]);
					sta_auth_param.strWepInfo.u8WepAuthType = MAIN_WLAN_WEP_AUTH_TYPE;
				}
						
				ret = os_m2m_wifi_connect((char *)&conn_ap.conn_param[2], conn_ap.conn_param[1], conn_ap.conn_param[0], &sta_auth_param, M2M_WIFI_CH_ALL);
				if (M2M_SUCCESS != ret) 
				{
					printf("\r\nmain: os_m2m_wifi_connect call error!(%d)\n", ret);
					while (1) {
					}
				}
			}
		}

	}
}
