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
#ifndef APPS_H_INCLUDED
#define APPS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
	#endif

/* Wi-Fi Settings */

/* for WPA Security */ 
#define MAIN_WLAN_SSID		           "DEMO_AP" /**< Destination SSID */
#define MAIN_WLAN_AUTH					M2M_WIFI_SEC_WPA_PSK /* < Security manner */
#define MAIN_WLAN_PSK                   "12345678" /**< Password for Destination SSID */

/* for WEP Security */ 
//#define MAIN_WLAN_SSID		           "DEMO_AP" /**< Destination SSID */
//#define MAIN_WLAN_AUTH					M2M_WIFI_SEC_WEP /* < Security manner */
#define MAIN_WLAN_WEP_KEY_INDEX          1 /**< WEP key index */
#define MAIN_WLAN_WEP_KEY_40            "1234567890" /**< 64 bit WEP key. In case of WEP64, 10 hexadecimal (base 16) characters (0-9 and A-F) ) */
#define MAIN_WLAN_WEP_KEY_104           "1234567890abcdef1234567890" /**< 128 bit WEP key. In case of WEP128, 26 hexadecimal (base 16) characters (0-9 and A-F) ) */

void tcp_client_task(void *argument);

#define MAIN_WIFI_M2M_PRODUCT_NAME        "WILC TCP CLIENT TEST"

#define MAIN_WIFI_M2M_SERVER_IP           ((uint32_t)0xc0a80102UL) //0xFFFFFFFF /* 255.255.255.255 */
#define MAIN_WIFI_M2M_SERVER_PORT         (6666)
#define MAIN_WIFI_M2M_REPORT_INTERVAL     (1000)

#define MAIN_WIFI_M2M_BUFFER_SIZE          1460

#define MAIN_WIFI_M2M_PRODUCT_NAME1        "NMCTemp1"
#define MAIN_WIFI_M2M_PRODUCT_NAME2        "NMCTemp2"


/** TCP Client Settings */
#define HEADER_VERSION1						0x80000000
#define RUN_NOW								0x00000001

typedef sint8			SOCKET;	

#ifdef __cplusplus
}
#endif

#endif // APPS_H_INCLUDED
