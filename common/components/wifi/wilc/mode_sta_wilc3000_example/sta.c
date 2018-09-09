/**
 *
 * \file
 *
 * \brief start STA mode.
 *
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

#include "asf.h"
#include "sta.h"
#include "FreeRTOS.h"
#include "osprintf.h"
#include "os/include/net_init.h"
#include "os/include/m2m_wifi_ex.h"
#include "lwip/def.h"
#include <string.h>
#include <stdio.h>
#include "driver/include/CertOut.h"

/* preprocessor define error check */
#if defined( STA_MODE_OPEN_SECURITY )
	#if defined(STA_MODE_WEP_SECURITY) || defined(STA_MODE_WPA_WPA2_PERSONAL_SECURITY) || defined(STA_MODE_WPA_WPA2_ENTERPRISE_SECURITY)
	#error "Error : Multiple Security types defined along with STA_MODE_OPEN_SECURITY"
	#endif
#elif defined(STA_MODE_WEP_SECURITY)
	#if defined(STA_MODE_WPA_WPA2_PERSONAL_SECURITY) || defined(STA_MODE_WPA_WPA2_ENTERPRISE_SECURITY)
	#error "Error : Multiple Security types defined along with STA_MODE_WEP_SECURITY"
	#endif
	#if (defined( WEP_SEC_TYPE_KEY_40 ) && defined ( WEP_SEC_TYPE_KEY_104 )) || !( defined( WEP_SEC_TYPE_KEY_40 ) || defined ( WEP_SEC_TYPE_KEY_104 ) )
	#error "Error : WEP Security - Define either WEP_SEC_TYPE_KEY_40 or WEP_SEC_TYPE_KEY_104"
	#endif
#elif defined(STA_MODE_WPA_WPA2_PERSONAL_SECURITY) 
	#if defined(STA_MODE_WPA_WPA2_ENTERPRISE_SECURITY)
	#error "Error : Multiple Security types defined(STA_MODE_WPA_WPA2_PERSONAL_SECURITY and STA_MODE_WPA_WPA2_ENTERPRISE_SECURITY) "
	#endif
#elif !defined(STA_MODE_WPA_WPA2_ENTERPRISE_SECURITY)
	#error "Error : Define Security type in sta.h (STA_MODE_OPEN_SECURITY or STA_MODE_WPA_WPA2_PERSONAL_SECURITY or STA_MODE_WEP_SECURITY or STA_MODE_WPA_WPA2_ENTERPRISE_SECURITY)"
#endif

#if defined(P2P_MODE_CONCURRENCY)
/**
 * \brief P2P mode
 *
 * Initializes the P2P mode.
 */
static int8_t enable_disable_p2p_mode(void)
{
	int8_t ret;

	osprintf("P2P mode, start\r\n");

	/* Set device name. */
	ret = os_m2m_wifi_set_device_name((uint8_t *)MAIN_WLAN_DEVICE_NAME, strlen(MAIN_WLAN_DEVICE_NAME));
	if (M2M_SUCCESS != ret) {
		osprintf("P2P mode, set device name failed\r\n");
		return ret;
	}
	
	os_m2m_wifi_set_p2p_control_ifc(P2P_STA_CONCURRENCY_INTERFACE); // Required if STA-P2P concurrent mode
	
	/* Start P2P with channel number. */
	ret = os_m2m_wifi_p2p(MAIN_WLAN_P2P_CHANNEL);
	if (M2M_SUCCESS != ret) {
		osprintf("start P2P failed\r\n");
		return ret;
	}
	return ret;
}
#endif

/* Connect to defined AP */
void wifi_connect(){
	tuniM2MWifiAuth sta_auth_param;
	
	#if defined( STA_MODE_OPEN_SECURITY )
	osprintf("Security : OPEN\r\n");
	os_m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), M2M_WIFI_SEC_OPEN, NULL, M2M_WIFI_CH_ALL);

	#elif defined( STA_MODE_WPA_WPA2_PERSONAL_SECURITY )
	osprintf("Security : WPA/WPA2 Personal\r\n");
	os_m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), M2M_WIFI_SEC_WPA_PSK, (void *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);

	#elif defined( STA_MODE_WEP_SECURITY )
	#if defined ( WEP_SEC_TYPE_KEY_40 )
	osprintf("Security : WEP with 10 digit WEP key\r\n");
	#elif defined ( WEP_SEC_TYPE_KEY_104 )
	osprintf("Security : WEP with 26 digit WEP key\r\n");
	#endif
	/** Security parameters for 64 bit/128 bit WEP Encryption @ref m2m_wifi_connect */
	tstrM2mWifiWepParams wep_parameters = { MAIN_WLAN_WEP_KEY_INDEX, sizeof(MAIN_WLAN_WEP_KEY), MAIN_WLAN_WEP_KEY, MAIN_WLAN_WEP_AUTH_TYPE};
	m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), M2M_WIFI_SEC_WEP,&wep_parameters, M2M_WIFI_CH_ALL);

	#elif defined( STA_MODE_WPA_WPA2_ENTERPRISE_SECURITY )
	osprintf("Security : WPA/WPA2 Enterprise Security\r\n");
	
			/* Request firmware to download Root Certificate */
			m2m_wifi_download_cert(cert_image,sizeof(cert_image));
			
			strncpy((char*)sta_auth_param.strCred1x.au8UserName, MAIN_WLAN_802_1X_USR_NAME,M2M_1X_USR_NAME_MAX-1);
			sta_auth_param.strCred1x.au8UserName[M2M_1X_USR_NAME_MAX-1] = '\0';
			
			strncpy((char*)sta_auth_param.strCred1x.au8Passwd, MAIN_WLAN_802_1X_PWD, M2M_1X_PWD_MAX-1);
			sta_auth_param.strCred1x.au8Passwd[M2M_1X_PWD_MAX-1]='\0';
			
			os_m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID),
			M2M_WIFI_SEC_802_1X, &sta_auth_param, M2M_WIFI_CH_ALL);
	#endif
}

/**
 * \brief Callback to get the Wi-Fi status update.
 *
 * \param[in] u8MsgType type of Wi-Fi notification. Possible types are:
 *  - [M2M_WIFI_RESP_CON_STATE_CHANGED](@ref M2M_WIFI_RESP_CON_STATE_CHANGED)
 *  - [M2M_WIFI_REQ_DHCP_CONF](@ref M2M_WIFI_REQ_DHCP_CONF)
 * \param[in] pvMsg A pointer to a buffer containing the notification parameters
 * (if any). It should be casted to the correct data type corresponding to the
 * notification type.
 */
static void wifi_cb(uint8_t u8MsgType, void *pvMsg)
{

	switch (u8MsgType) {
		case M2M_WIFI_RESP_CON_STATE_CHANGED:
		{
			tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
			if (pstrWifiState->u8IfcId == STATION_INTERFACE) {
				if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) {
					osprintf("Wi-Fi connected\r\n");
					if(use_static_IP == 1) {
					ip_addr_t ipaddr, netmask, gw;
					IP4_ADDR(&gw, 192,168,0,1);
					IP4_ADDR(&ipaddr, 192,168,0,109);
					IP4_ADDR(&netmask, 255,255,255,0);
					net_set_interface_address(NET_IF_STA,&ipaddr,&netmask,&gw);
						m2m_wifi_request_static_client_ex();
						// net_interface_up should not be made common to DHCP & Static IP as use_static_IP = 1 is required inside.
						net_interface_up(NET_IF_STA); 
						//use_static_IP = 0; //Static IP configuration should be retained in case of Disconnection
					}
					else {
						m2m_wifi_request_dhcp_client_ex();
						net_interface_up(NET_IF_STA);
					}
				} else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) {
					osprintf("Wi-Fi disconnected\r\n");
					osprintf("wifi_cb: reconnecting...\r\n");
					net_interface_down(NET_IF_STA);
					/* Connect to defined AP. */
					wifi_connect();
				}
			}
#if defined(P2P_MODE_CONCURRENCY)			
			else if (pstrWifiState->u8IfcId == P2P_INTERFACE) {
				if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) {
					osprintf("wifi_cb: P2P: M2M_WIFI_CONNECTED\r\n");
				}
				if(pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) {
					osprintf("wifi_cb: P2P: M2M_WIFI_DISCONNECTED\r\n");
				}
			}
#endif			
			break;
		}

		case NET_IF_REQ_DHCP_CONF :
		{
			tstrM2MIPConfig2 *strIpConfig = pvMsg;
			uint16_t *a = (void *)strIpConfig->u8StaticIPv6;
			osprintf("wifi_cb: STA IPv4 addr: %d.%d.%d.%d\r\n", strIpConfig->u8StaticIP[0], strIpConfig->u8StaticIP[1],
			strIpConfig->u8StaticIP[2], strIpConfig->u8StaticIP[3]);
			osprintf("wifi_cb: STA IPv6 addr: %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\r\n",
			htons(a[0]), htons(a[1]), htons(a[2]), htons(a[3]),
			htons(a[4]), htons(a[5]), htons(a[6]), htons(a[7]));
			
		#if defined(P2P_MODE_CONCURRENCY)
			int ret;
			ret = enable_disable_p2p_mode();
			if (M2M_SUCCESS != ret) {
				osprintf("enable_disable_ap_mode call error!\r\n");
			}
		#endif
		}
		break;

		default:
		{
			break;
		}
	}
}

/**
 * \brief STA mode main function.
 */
void mode_sta(void *argument)
{
	/* Initialize the network stack. */
	net_init();
	
	#if defined(STA_MODE_STATIC_IP)
		use_static_IP = 1;
	#else
		use_static_IP = 0;
	#endif
	
	/* Initialize the WILC driver. */
	tstrWifiInitParam param;
	memset(&param, 0, sizeof(param));
	param.pfAppWifiCb = wifi_cb;
	os_m2m_wifi_init(&param);

	osprintf("Connecting to %s\r\n", (char *)MAIN_WLAN_SSID);
	/* Connect to defined AP. */
	wifi_connect();
	while(1){
	}
}
