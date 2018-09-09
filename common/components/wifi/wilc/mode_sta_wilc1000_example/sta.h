/**
 * \file
 *
 * \brief STA mode configuration.
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

#ifndef STA_H_INCLUDED
#define STA_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* Define a Security type */
//#define 	STA_MODE_OPEN_SECURITY
/*!< Wi-Fi network is not secured.
*/
//#define  	STA_MODE_WPA_WPA2_PERSONAL_SECURITY
/*!< Wi-Fi network is secured with WPA/WPA2 personal(PSK).
*/
#define 	STA_MODE_WEP_SECURITY
/*!< Security type WEP (40 or 104) OPEN OR SHARED.
*/
//#define 	STA_MODE_WPA_WPA2_ENTERPRISE_SECURITY
 /*!< Wi-Fi network is secured with WPA/WPA2 Enterprise.IEEE802.1x user-name/password authentication.
 */
 
//#define STA_MODE_STATIC_IP //for using Static IP with STA mode

//#define    P2P_MODE_CONCURRENCY
/** P2P mode Settings */
#define MAIN_WLAN_DEVICE_NAME        "WILC_P2P" /* < P2P Device Name */
#define MAIN_WLAN_P2P_CHANNEL        (M2M_WIFI_CH_11) /* < P2P Channel number */

/** Wi-Fi Settings */
#define MAIN_WLAN_SSID        "DEMO_AP" /* < Destination SSID */
#if defined(STA_MODE_WPA_WPA2_PERSONAL_SECURITY)
#define MAIN_WLAN_PSK         "12345678" /* < Password for Destination SSID */

#elif defined(STA_MODE_WEP_SECURITY)
//#define WEP_SEC_TYPE_KEY_40
#define WEP_SEC_TYPE_KEY_104
#define MAIN_WLAN_WEP_KEY_INDEX         1 /**< WEP key index */
#define MAIN_WLAN_WEP_AUTH_TYPE   	 	WEP_ANY
#if defined(WEP_SEC_TYPE_KEY_40)
#define MAIN_WLAN_WEP_KEY               "1234567890" /**< 64 bit WEP key. In case of WEP64, 10 hexadecimal (base 16) characters (0-9 and A-F) ) */
#elif defined(WEP_SEC_TYPE_KEY_104)
#define MAIN_WLAN_WEP_KEY	            "1234567890abcdef1234567890" /**< 128 bit WEP key. In case of WEP128, 26 hexadecimal (base 16) characters (0-9 and A-F) ) */
#endif

#elif defined(STA_MODE_WPA_WPA2_ENTERPRISE_SECURITY)
#define MAIN_WLAN_802_1X_USR_NAME       "atmeluser" /**< RADIUS user account name */
#define MAIN_WLAN_802_1X_PWD            "12345678" /**< RADIUS user account password */
#endif
void mode_sta(void *argument);

#ifdef __cplusplus
}
#endif

#endif /* STA_H_INCLUDED */
