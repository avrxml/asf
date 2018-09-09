/**
 *
 * \file
 *
 * \brief change STA, AP and P2P modes.
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
#include "modes.h"
#include "FreeRTOS.h"
#include "osprintf.h"
#include "os/include/net_init.h"
#include "os/include/m2m_wifi_ex.h"
#include <string.h>
#include <stdio.h>


/**
 * \brief AP mode
 *
 * Initializes the AP mode for a while and terminate.
 */
static int8_t enable_disable_ap_mode(void)
{
	int8_t ret;
	tstrM2MAPConfig strM2MAPConfig;

	osprintf("AP mode, start\r\n");

	/* Configure AP. */
	memset(&strM2MAPConfig, 0x00, sizeof(tstrM2MAPConfig));
	strcpy((char *)&strM2MAPConfig.au8SSID, MAIN_WLAN_SSID);
	strM2MAPConfig.u8ListenChannel = MAIN_WLAN_AP_CHANNEL;
	strM2MAPConfig.u8SecType = MAIN_WLAN_AUTH;

	/* Start AP mode. */
	ret = os_m2m_wifi_enable_ap(&strM2MAPConfig);
	if (M2M_SUCCESS != ret) {
		return ret;
	}

	/* Keep in AP mode for a while. */
	nm_bsp_sleep(HOLD_TIME_IN_MODE);

	/* Stop AP mode. */
	ret = os_m2m_wifi_disable_ap();
	if (M2M_SUCCESS != ret) {
		return ret;
	}

	osprintf("AP mode, end\r\n");

	return ret;
}

/**
 * \brief P2P mode
 *
 * Initializes the P2P mode for a while and terminate.
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
	
	//os_m2m_wifi_set_p2p_control_ifc(P2P_AP_CONCURRENCY_INTERFACE); // Required if AP-P2P concurrent mode
	
	/* Start P2P with channel number. */
	ret = os_m2m_wifi_p2p(MAIN_WLAN_P2P_CHANNEL);
	if (M2M_SUCCESS != ret) {
		osprintf("start P2P failed\r\n");
		return ret;
	}

	/* Keep in P2P mode for a while. */
	nm_bsp_sleep(HOLD_TIME_IN_MODE);

	/* Stop P2P mode. */
	ret = os_m2m_wifi_p2p_disconnect();
	if (M2M_SUCCESS != ret) {
		return ret;
	}

	osprintf("P2P mode, end\r\n");

	return ret;
}
	

static void wifi_cb(uint8_t u8MsgType, void *pvMsg)
{
	switch (u8MsgType)
	{
		case M2M_WIFI_RESP_CON_STATE_CHANGED : {
			tstrM2mWifiStateChanged *ctx = (tstrM2mWifiStateChanged*)pvMsg;
			if (ctx->u8IfcId == AP_INTERFACE) {
				if (ctx->u8CurrState == M2M_WIFI_CONNECTED) {
					osprintf("wifi_cb: AP M2M_WIFI_CONNECTED %02x-%02x-%02x-%02x-%02x-%02x\r\n",
							ctx->u8MAcAddr[0], ctx->u8MAcAddr[1], ctx->u8MAcAddr[2],
							ctx->u8MAcAddr[3], ctx->u8MAcAddr[4], ctx->u8MAcAddr[5]);
				}
				if (ctx->u8CurrState == M2M_WIFI_DISCONNECTED) {
					osprintf("wifi_cb: AP M2M_WIFI_DISCONNECTED %02x-%02x-%02x-%02x-%02x-%02x, reason: %s, %d\r\n",
							ctx->u8MAcAddr[0], ctx->u8MAcAddr[1], ctx->u8MAcAddr[2],
							ctx->u8MAcAddr[3], ctx->u8MAcAddr[4], ctx->u8MAcAddr[5],
							ctx->u8ErrCode == M2M_ERR_STATION_IS_LEAVING ? "M2M_STATION_IS_LEAVING": 
							ctx->u8ErrCode == M2M_ERR_LINK_LOSS ? "M2M_LINK_LOSS":
							ctx->u8ErrCode == M2M_ERR_AUTH_FAIL ? "M2M_AUTH_FAIL":"UNKNOWN", ctx->u8ErrCode);
				}
			}
			else if (ctx->u8IfcId == P2P_INTERFACE) {
				if (ctx->u8CurrState == M2M_WIFI_CONNECTED) {
					osprintf("wifi_cb: P2P: M2M_WIFI_CONNECTED\r\n");
				}
				if(ctx->u8CurrState == M2M_WIFI_DISCONNECTED) {
					osprintf("wifi_cb: P2P: M2M_WIFI_DISCONNECTED\r\n");
				}
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
 * \brief mode changes entry function.
 */
void mode_change(void *argument)
{
	int8_t ret;
	/* Initialize the network stack. */
	net_init();
	
	tstrWifiInitParam param;
	memset(&param, 0, sizeof(param));
	param.pfAppWifiCb = wifi_cb;
	ret = os_m2m_wifi_init(&param);
	if (M2M_SUCCESS != ret) {
		osprintf("m2m_wifi_init call error!(%d)\r\n", ret);
		while (1) {
		}
	}

	/**
	 * Station mode.
	 * Device started as station mode basically.
	 */
	if (1) {
	}

	/**
	 * AP mode.
	 * Turn On and off AP mode.
	 */
	ret = enable_disable_ap_mode();
	if (M2M_SUCCESS != ret) {
		osprintf("enable_disable_ap_mode call error!\r\n");
		while (1) {
		}
	}

	nm_bsp_sleep(DELAY_FOR_MODE_CHANGE);

	/**
	 * P2P mode.
	 * Turn On and off P2P mode.
	 */
	ret = enable_disable_p2p_mode();
	if (M2M_SUCCESS != ret) {
		osprintf("enable_disable_p2p_mode call error!\r\n");
		while (1) {
		}
	}
	while(1){
	}
}
