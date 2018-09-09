/**
 * Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
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

//  Include Header Files here 
#include "asf.h"
#include "FreeRTOS.h"
#include "osprintf.h"
#include "lwip/ip.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/dns.h"
#include "os/include/net_init.h"
#include "os/include/m2m_wifi_ex.h"
#include "tinyservices.h"
#include "conf_wilc.h"
#include "driver/include/m2m_wifi.h"
#include "common/include/nm_common.h"
#include "lwip/sockets.h"
#include <string.h>
#include <stdio.h>
#include <ap_scan_with_rssi.h>

/////////////////////////////////
// no need to initialise socket 

////////////////////////////////

/** Index of scan list to request scan result. */
static uint8_t scan_request_index = 0;

/** Number of APs found. */
static uint8_t num_founded_ap = 0;

/** IP address of host. */
uint32_t gu32HostIp = 0;

/** Wi-Fi status variable. */
static volatile bool gbConnectedWifi = false;

/** Get host IP status variable. */
static volatile bool gbHostIpByName = false;

/**
 * \brief Callback to get the Wi-Fi status update.
 *
 * \param[in] u8MsgType Type of Wi-Fi notification.
 * \param[in] pvMsg A pointer to a buffer containing the notification parameters.
 *
 * \return None.
 */
static void wifi_cb(uint8 msg_type, void *msg)
{
	switch (msg_type) {
		
		case M2M_WIFI_RESP_SCAN_DONE:
	{
		tstrM2mScanDone *pstrInfo = (tstrM2mScanDone *)msg;
		scan_request_index = 0;
		if (pstrInfo->u8NumofCh >= 1) {
			os_m2m_wifi_req_scan_result(scan_request_index);
			scan_request_index++;
		} else {
			os_m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
		}

	}
	break;
	
	case M2M_WIFI_RESP_SCAN_RESULT:
	{
		tstrM2mWifiscanResult *pstrScanResult = (tstrM2mWifiscanResult *)msg;
		uint16_t demo_ssid_len;
		uint16_t scan_ssid_len = strlen((const char *)pstrScanResult->au8SSID);

		/* display founded AP. */
		osprintf("[%d] SSID:%s\r\n", scan_request_index, pstrScanResult->au8SSID);

		num_founded_ap = os_m2m_wifi_get_num_ap_found();
		if (scan_ssid_len) {
			/* check same SSID. */
			demo_ssid_len = strlen((const char *)MAIN_WLAN_SSID);
			if
			(
				(demo_ssid_len == scan_ssid_len) &&
				(!memcmp(pstrScanResult->au8SSID, (uint8_t *)MAIN_WLAN_SSID, demo_ssid_len))
			) {
				/* A scan result matches an entry in the preferred AP List.
				 * Initiate a connection request.
				 */
				osprintf("Found %s \r\n", MAIN_WLAN_SSID);
				os_m2m_wifi_connect((char *)MAIN_WLAN_SSID,
						sizeof(MAIN_WLAN_SSID),
						MAIN_WLAN_AUTH,
						(void *)MAIN_WLAN_PSK,
						M2M_WIFI_CH_ALL);
				break;
			}
		}

		if (scan_request_index < num_founded_ap) {
			os_m2m_wifi_req_scan_result(scan_request_index);
			scan_request_index++;
		} else {
			osprintf("can not find AP %s\r\n", MAIN_WLAN_SSID);
			os_m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
		}

	}
	break;
	
		
		case M2M_WIFI_RESP_CON_STATE_CHANGED : {
			tstrM2mWifiStateChanged *ctx = (tstrM2mWifiStateChanged*)msg;
			if (ctx->u8IfcId == STATION_INTERFACE) {
				if (ctx->u8CurrState == M2M_WIFI_CONNECTED) {
					osprintf("wifi_cb: M2M_WIFI_CONNECTED\n\r");
					net_interface_up(NET_IF_STA);				// need to check
					m2m_wifi_request_dhcp_client_ex();
				}
				if(ctx->u8CurrState == M2M_WIFI_DISCONNECTED) {
					osprintf("wifi_cb: M2M_WIFI_DISCONNECTED\n\r");
					osprintf("wifi_cb: reconnecting...\n\r");
					net_interface_down(NET_IF_STA);				// need to check
					os_m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID),
							MAIN_WLAN_AUTH, (void *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);
				}
			}
		}
		break;

		case NET_IF_REQ_DHCP_CONF : {
			tstrM2MIPConfig2 *strIpConfig = msg;
			uint16_t *a = (void *)strIpConfig->u8StaticIPv6;
			osprintf("wifi_cb: STA M2M_WIFI_REQ_DHCP_CONF\n\r");
			osprintf("wifi_cb: STA IPv4 addr: %d.%d.%d.%d\n\r", strIpConfig->u8StaticIP[0], strIpConfig->u8StaticIP[1],
			strIpConfig->u8StaticIP[2], strIpConfig->u8StaticIP[3]);
			osprintf("wifi_cb: STA IPv6 addr: %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n\r",
			htons(a[0]), htons(a[1]), htons(a[2]), htons(a[3]),
			htons(a[4]), htons(a[5]), htons(a[6]), htons(a[7]));
			gbConnectedWifi = true;
			
			/* Request RSSI for the connected AP. */
			os_m2m_wifi_req_curr_rssi();

			
		}
		break;


		case M2M_WIFI_RESP_CURRENT_RSSI:
		{
			/* This message type is triggered by "m2m_wifi_req_curr_rssi()" function. */
			int8_t *rssi = (int8_t *)msg;
			osprintf("RSSI for the current connected AP (%d)\r\n", (int8_t)(*rssi));

		}
		break;
		
		default:
		{
			break;
		}

	}
}


/**
 * \brief AP Scan task function.
 */
void ap_scan_with_rssi_task(void *v)
{
	int8_t ret;
	
	/* Initialize the network stack. */
	net_init();
	
	/* Initialize the WILC1000 driver. */
	tstrWifiInitParam param;
	memset(&param, 0, sizeof(param));
	param.pfAppWifiCb = wifi_cb;
	//os_m2m_wifi_init(&param);

	ret = os_m2m_wifi_init(&param);
	if (M2M_SUCCESS != ret) {
		printf("main: m2m_wifi_init call error!(%d)\r\n", ret);
		while (1) {
		}
	}

	/* Connect to defined AP. */
	//os_m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, (void *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);

	/* Request scan. */
	os_m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
	
	
	while (1) {
		}
	
	}

	
