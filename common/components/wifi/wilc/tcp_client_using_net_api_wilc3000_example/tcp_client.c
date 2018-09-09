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

// Include Header Files here
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
#include <tcp_client.h>

/////////////////////////////////
// no need to initialise scoket 

////////////////////////////////

/** Security parameters for 64 bit WEP Encryption @ref m2m_wifi_connect */
tstrM2mWifiWepParams wep64_parameters = { MAIN_WLAN_WEP_KEY_INDEX, sizeof(MAIN_WLAN_WEP_KEY_40), MAIN_WLAN_WEP_KEY_40};

/** Security parameters for 128 bit WEP Encryption @ref m2m_wifi_connect */
tstrM2mWifiWepParams wep128_parameters = { MAIN_WLAN_WEP_KEY_INDEX, sizeof(MAIN_WLAN_WEP_KEY_104), MAIN_WLAN_WEP_KEY_104};


////////////////////////////////


/** IP address of host. */
uint32_t gu32HostIp = 0;

/** Wi-Fi status variable. */
static volatile bool gbConnectedWifi = false;

/** Get host IP status variable. */
static volatile bool gbHostIpByName = false;

/** server response. */
static char server_response[2048];


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
		case M2M_WIFI_RESP_CON_STATE_CHANGED : {
			tstrM2mWifiStateChanged *ctx = (tstrM2mWifiStateChanged*)msg;
			if (ctx->u8IfcId == STATION_INTERFACE) {
				if (ctx->u8CurrState == M2M_WIFI_CONNECTED) {
					osprintf("wifi_cb: M2M_WIFI_CONNECTED\n\r");
					net_interface_up(NET_IF_STA);				
					m2m_wifi_request_dhcp_client_ex();
				}
				if(ctx->u8CurrState == M2M_WIFI_DISCONNECTED) {
					osprintf("wifi_cb: M2M_WIFI_DISCONNECTED\n\r");
					osprintf("wifi_cb: reconnecting...\n\r");
					net_interface_down(NET_IF_STA);				
					//m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID),
					//		MAIN_WLAN_AUTH, (void *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);
				}
			}
		}
		break;

		case NET_IF_REQ_DHCP_CONF: {
			tstrM2MIPConfig2 *strIpConfig = msg;
			uint16_t *a = (void *)strIpConfig->u8StaticIPv6;
			osprintf("wifi_cb: STA M2M_WIFI_REQ_DHCP_CONF\n\r");
			osprintf("wifi_cb: STA IPv4 addr: %d.%d.%d.%d\n\r", strIpConfig->u8StaticIP[0], strIpConfig->u8StaticIP[1],
			strIpConfig->u8StaticIP[2], strIpConfig->u8StaticIP[3]);
			osprintf("wifi_cb: STA IPv6 addr: %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n\r",
			htons(a[0]), htons(a[1]), htons(a[2]), htons(a[3]),
			htons(a[4]), htons(a[5]), htons(a[6]), htons(a[7]));
			gbConnectedWifi = true;
		}
		break;

		default:
		break;
	}
}


/**
 * \brief TCP task function.
 */
void tcp_client_task(void *v)
{
	uint8_t request[256];
	struct netconn *conn = NULL;
	//ip_addr_t local_ip;
	ip_addr_t remote_ip;
	
	struct netbuf *rx_buf;
	char *str;
	char *tmp;
	uint16_t tot_len, len;
	//uint8 chkConn;
	
	/* Initialize the network stack. */
	net_init();
	
	/* Initialize the WILC driver. */
	tstrWifiInitParam param;
	memset(&param, 0, sizeof(param));
	param.pfAppWifiCb = wifi_cb;
	os_m2m_wifi_init(&param);

			/* Connect to defined AP. */
			/* Case 1. Connect to AP with security type WPA. */
			os_m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, (void *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);

			/* Case 2. Connect to AP with security type WEP. */
			//os_m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, &wep64_parameters, M2M_WIFI_CH_ALL); // use &wep64_parameters or &wep128_parameters



	while(!gbConnectedWifi){
	vTaskDelay(200);	
	}
	
	osprintf("tcp_client_task: connected to WiFi!\n\r"); 
	
	/* Create TCP socket. */
			conn = netconn_new(NETCONN_TCP);
			if (conn == NULL) {
				osprint("sta_task: failed to create connection!\n\r");
				while(1);
			}
									
			/* Connect socket. */
				remote_ip.addr= PP_HTONL(MAIN_WIFI_M2M_SERVER_IP);
				osprintf("display server addr:%x\n\r",(remote_ip.addr));
				
			if (netconn_connect(conn, &remote_ip, (uint32_t)MAIN_WIFI_M2M_SERVER_PORT) != ERR_OK) {
				osprint("sta_task: failed to connect server !\n\r");
				netconn_delete(conn);
				while(1);
			}
			
			/* Send request. */
			memset(request, 0, sizeof(request));
						
			strcpy((char *)request,(char *)MAIN_WIFI_M2M_PRODUCT_NAME);
			netconn_write(conn, (char *)request, strlen((char *)request), NETCONN_COPY);

			/* Wait for server response. */
			tmp = server_response;
			tot_len = 0;
			while (netconn_recv(conn, &rx_buf) != ERR_OK) {
				vTaskDelay(200);
			}
			do {
				netbuf_data(rx_buf, (void *)&str, &len);
				memcpy(tmp, str, len);
				tmp += len;
				tot_len += len;
			
			}while(netbuf_next(rx_buf) >= 0 && tot_len < 2048);
			
			if(tot_len >0)
			osprintf("TCP CLIENT TEST SUCCESSFUL\n\r");
			else
			osprintf("TCP CLIENT TEST FAIL\n\r");
			
			netbuf_delete(rx_buf);
			netconn_close(conn);
			netconn_delete(conn);
				

	}

	
