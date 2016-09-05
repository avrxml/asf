/**
 *
 * \file
 *
 * \brief IOT QTouch Sensor Demo.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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

#ifndef DEMO_H_INCLUDED
#define DEMO_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/** Default SSID and passphrase of the network to connect to. */
#define DEFAULT_SSID						"default"
#define	DEFAULT_PWD							"default"
#define DEFAULT_AUTH						M2M_WIFI_SEC_WPA_PSK
#define DEFAULT_CHANNEL						M2M_WIFI_CH_ALL

/** Sensor name. */
#define DEMO_PRODUCT_NAME					"Touch"

/* Using broadcast address for simplicity. */
#define DEMO_SERVER_IP						"255.255.255.255"
#define DEMO_SERVER_PORT					(6666)
#define DEMO_REPORT_INTERVAL				(100)
#define DEMO_WLAN_AP_IP_ADDRESS				{192,168,1,10}
#define DEMO_WLAN_AP_DOMAIN_NAME			"atmelconfig.com"

#define DEMO_WLAN_AP_NAME					"WINC1500_TOUCH_MyAP"// Access Point Name.
#define DEMO_WLAN_AP_CHANNEL				1// Channel to use.
#define DEMO_WLAN_AP_WEP_INDEX				0// Wep key index.
#define DEMO_WLAN_AP_WEP_SIZE				WEP_40_KEY_STRING_SIZE// Wep key size.
#define DEMO_WLAN_AP_WEP_KEY				"1234567890"// Wep key.
#define DEMO_WLAN_AP_SECURITY				M2M_WIFI_SEC_OPEN// Security mode.
#define DEMO_WLAN_AP_MODE					0 //Visible

#define CREDENTIAL_ENTRY_BUTTON				PIN_PA15

void demo_start(void);

#ifdef __cplusplus
}
#endif

#endif /* DEMO_H_INCLUDED */
