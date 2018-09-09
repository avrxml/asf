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

// Include Files here
#include "asf.h"
#include "FreeRTOS.h"
#include "osprintf.h"
#include "lwip/ip.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/udp.h"
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
#include <udp_server.h>

#if LWIP_SOCKET /* don't build if not configured for use in lwipopts.h */

//////////////////////////////////

/** Message format definitions. */
typedef struct s_msg_wifi_product {
	uint8_t name[9];
} t_msg_wifi_product;

typedef struct s_msg_wifi_product_main {
	uint8_t name[9];
} t_msg_wifi_product_main;

/** Test buffer */
static uint8_t gau8SocketTestBuffer[MAIN_WIFI_M2M_BUFFER_SIZE] = {0,};

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

/** Socket for UDP */
static SOCKET udp_socket = -1;

#define BUFSIZE                         10

/** UDP MAX packet count */
#define MAIN_WIFI_M2M_PACKET_COUNT         10

static size_t	name_len;

/** Number of STA connected. */
uint32_t sta_connected = 0;

/** UDP packet count */
static uint8_t packetCnt = 0;


//////////////////////////////////////////////////////////////////

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
					net_interface_up(NET_IF_STA);				// need to check
					m2m_wifi_request_dhcp_client_ex();
				}
				if(ctx->u8CurrState == M2M_WIFI_DISCONNECTED) {
					osprintf("wifi_cb: M2M_WIFI_DISCONNECTED\n\r");
					osprintf("wifi_cb: reconnecting...\n\r");
					net_interface_down(NET_IF_STA);				// need to check
					//m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID),
					//		MAIN_WLAN_AUTH, (void *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);
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
			//osprintf("display local addr:%x\n",());
		}
		break;

		default:
		break;
	}
}


/**
 * \brief UDP Server task function.
 */
void udp_server_task(void *v)
{
//struct netconn *udp_socket;
name_len = 10;
int8_t ret, bindchk, recv_chk, fromlen;

struct sockaddr_in addr;
struct sockaddr_in remote_addr;

//int fromlen;

	/* Initialize the network stack. */
	net_init();


	/* Initialize the WILC driver. */ /* Initialize Wi-Fi parameters structure. */
	tstrWifiInitParam param;
	memset(&param, 0, sizeof(param));

	/* Initialize Wi-Fi driver with data and status callbacks. */
	param.pfAppWifiCb = wifi_cb;
	ret = os_m2m_wifi_init(&param);
	
	if(ret != M2M_SUCCESS){
		osprintf("os_m2m_wifi_init : Not initialized: %d!\r\n",ret);	
	}

	/* Connect to defined AP. */
	/* Case 1. Connect to AP with security type WPA. */
	os_m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, (void *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);

	/* Case 2. Connect to AP with security type WEP. */
	//os_m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, &wep64_parameters, M2M_WIFI_CH_ALL); // use &wep64_parameters or &wep128_parameters
	
	while(!gbConnectedWifi){
		vTaskDelay(200);
	}

	/* Initialize socket address structure. */
	//addr.sin_len= 128;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(MAIN_WIFI_M2M_SERVER_PORT);
	addr.sin_addr.s_addr = htonl(MAIN_WIFI_M2M_SERVER_IP);

	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = htons(MAIN_WIFI_M2M_CLIENT_PORT);
	remote_addr.sin_addr.s_addr = htonl(MAIN_WIFI_M2M_CLIENT_IP);
	
	fromlen = sizeof(struct sockaddr_in);

		/* Initialize,  create and connect socket in WILC module */
		udp_socket= socket(AF_INET, SOCK_DGRAM /*for UDP*/, 0/*flag*/);  //UDP socket creation

		/* Socket bind in WILC */
		// bind(int s, const struct sockaddr *name, socklen_t namelen)
		bindchk = bind(udp_socket, (struct sockaddr*)&addr, sizeof(addr));

		if(bindchk>=0){
			osprintf("main : UDP socket bind done !\r\n");
		}
		else
		{
			osprintf("main : UDP socket bind undone !\r\n");
		}


	packetCnt = 0;
	while (1) {
		if (packetCnt >= MAIN_WIFI_M2M_PACKET_COUNT) {
			printf("UDP Server test Complete!\r\n");
			close(udp_socket);
			udp_socket = -1;
			while(1);
		
		}

		
		/* Extra checking for Wi-Fi connected and Socket connection */
		if (gbConnectedWifi) {
			/* Create socket for Server / Receiving UDP */

				if ((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
					osprintf("main : failed to create UDP Client socket error!\r\n");
					continue;
				}
			
			//recvfrom(int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
			recv_chk = recvfrom(udp_socket, gau8SocketTestBuffer, MAIN_WIFI_M2M_BUFFER_SIZE, 0x00, (struct sockaddr*)&remote_addr, &fromlen);
			
			if(recv_chk>=0){
			osprintf("main : number of received packets from UDP Client : %d !\r\n",recv_chk);
		}
		else
		{
			osprintf("main : no packets received from UDP Client  !\r\n");
		}
		
		/* Prepare next buffer reception. */
		osprintf("main : UDP socket received packets are : %s !\r\n",gau8SocketTestBuffer);

			}
	}

}

#endif /* LWIP_SOCKET  don't build if not configured for use in lwipopts.h */