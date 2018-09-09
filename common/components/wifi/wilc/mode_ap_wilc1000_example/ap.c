/**
 *
 * \file
 *
 * \brief start AP mode.
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
#include "ap.h"
#include "FreeRTOS.h"
#include "osprintf.h"
#include "os/include/net_init.h"
#include "os/include/m2m_wifi_ex.h"
#include <string.h>
#include <stdio.h>


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
	
	os_m2m_wifi_set_p2p_control_ifc(P2P_AP_CONCURRENCY_INTERFACE); // Required if AP-P2P concurrent mode
	
	/* Start P2P with channel number. */
	ret = os_m2m_wifi_p2p(MAIN_WLAN_P2P_CHANNEL);
	if (M2M_SUCCESS != ret) {
		osprintf("start P2P failed\r\n");
		return ret;
	}
	return ret;
}
#endif

/* security type preprocessor define verification */
#if defined ( USE_SEC_OPEN )
	#if defined ( USE_WEP ) || defined ( USE_WPA_PSK )
	#error "Multiple Security types defined along with USE_SEC_OPEN"
	#endif
#elif defined ( USE_WEP )
	#if defined ( USE_WPA_PSK )
	#error "Multiple Security types defined (USE_WEP and USE_WPA_PSK)"
	#endif
#elif !defined ( USE_WPA_PSK )
#error "Define a Security type : USE_SEC_OPEN, USE_WEP or USE_WPA_PSK"
#endif

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
#if defined(P2P_MODE_CONCURRENCY)
	int8_t ret;
#endif
	switch (u8MsgType) {
		case M2M_WIFI_RESP_CON_STATE_CHANGED:
		{
			uint8 isDeviceMac = 1;
			tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
			if (pstrWifiState->u8IfcId == AP_INTERFACE) {			
				if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) {
					uint8 pu8MacAddr0[6], pu8MacAddr1[6];
					if(m2m_wifi_get_mac_address(pu8MacAddr0, pu8MacAddr1) != M2M_SUCCESS){ //failed get Device MAC
						osprintf("Failed to get Device MAC\r\n");
						osprintf("wifi_cb : M2M_WIFI_CONNECTED for MAC %02x:%02x:%02x:%02x:%02x:%02x\r\n", pstrWifiState->u8MAcAddr[0], pstrWifiState->u8MAcAddr[1],\
						pstrWifiState->u8MAcAddr[2], pstrWifiState->u8MAcAddr[3], pstrWifiState->u8MAcAddr[4], pstrWifiState->u8MAcAddr[5]);		
						break;
					}
					//Check MAC with Device MAC
					for(int i=0; i<6; i++){ 
						if(pstrWifiState->u8MAcAddr[i] != pu8MacAddr0[i]){
							isDeviceMac = 0;
							break;
						}
					}
					
					if(!isDeviceMac) {
						osprintf("Station connected    %02x:%02x:%02x:%02x:%02x:%02x\r\n", pstrWifiState->u8MAcAddr[0], pstrWifiState->u8MAcAddr[1],\
							pstrWifiState->u8MAcAddr[2], pstrWifiState->u8MAcAddr[3], pstrWifiState->u8MAcAddr[4], pstrWifiState->u8MAcAddr[5]);
					}
					else  {
						osprintf("AP mode started. You can connect to %s\r\n", MAIN_WLAN_SSID);
					#if defined(P2P_MODE_CONCURRENCY)
						ret = enable_disable_p2p_mode();
						if (M2M_SUCCESS != ret) {
							osprintf("enable_disable_ap_mode call error!\r\n");
						}
					#endif
					}
				} else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) {
					osprintf("Station disconnected %02x:%02x:%02x:%02x:%02x:%02x\r\n", pstrWifiState->u8MAcAddr[0], pstrWifiState->u8MAcAddr[1],\
						pstrWifiState->u8MAcAddr[2], pstrWifiState->u8MAcAddr[3], pstrWifiState->u8MAcAddr[4], pstrWifiState->u8MAcAddr[5]);
				} 
				break;
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
		}


		default:
		{
			break;
		}
	}
}

/**
 * \brief AP mode main function.
 */
void mode_ap(void *argument)
{
	/* Initialize the network stack. */
	net_init();
	
	/* Initialize the WILC1000 driver. */
	tstrWifiInitParam param;
	memset(&param, 0, sizeof(param));
	param.pfAppWifiCb = wifi_cb;
	os_m2m_wifi_init(&param);

	/* Enable AP mode. */
	tstrM2MAPConfig cfg;
	memset(&cfg, 0, sizeof(cfg));
	strcpy((char *)cfg.au8SSID, MAIN_WLAN_SSID);
	cfg.u8ListenChannel = MAIN_WLAN_CHANNEL;
	cfg.u8MaxSupportedSta = MAIN_WLAN_MAX_STA_COUNT;

#if defined ( USE_WPA_PSK )
	cfg.u8SecType = M2M_WIFI_SEC_WPA_PSK;
	strcpy((char *)cfg.uniAuth.au8PSK, "12345678");

#elif defined ( USE_WEP )
	osprintf("WEP security\r\n");
	cfg.u8SecType = M2M_WIFI_SEC_WEP;	
	cfg.uniAuth.strWepInfo = (tstrM2mWifiWepParams){WEP_KEY_INDEX, sizeof(WEP_KEY), WEP_KEY, WEP_AUTH_TYPE};

#elif defined ( USE_SEC_OPEN )
	cfg.u8SecType = M2M_WIFI_SEC_OPEN;
#endif
	os_m2m_wifi_enable_ap(&cfg);
	while(1){}
}
