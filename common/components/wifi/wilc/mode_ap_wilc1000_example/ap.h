/**
 * \file
 *
 * \brief AP mode configuration.
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

#ifndef AP_H_INCLUDED
#define AP_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//#define    P2P_MODE_CONCURRENCY
/** P2P mode Settings */
#define MAIN_WLAN_DEVICE_NAME        "WILC_P2P" /* < P2P Device Name */
#define MAIN_WLAN_P2P_CHANNEL        (M2M_WIFI_CH_11) /* < P2P Channel number */

/** AP mode Settings */
#define MAIN_WLAN_SSID				"WILC1000" /* < SSID */
#define MAIN_WLAN_CHANNEL			M2M_WIFI_CH_6 /* < Channel number */
#define MAIN_WLAN_MAX_STA_COUNT		0 /* < Max supported stations for AP. Val 0 considered as default 8 stations, Val > 8 are considered upto 8 stations */

/** Security mode. Define one of the below */
//#define USE_SEC_OPEN /* < No Security */
//#define USE_WEP
#define USE_WPA_PSK


#if defined USE_WPA_PSK
#define  WPA_PSK_KEY				"12345678"

#elif defined USE_WEP
#define WEP_KEY_INDEX				M2M_WIFI_WEP_KEY_INDEX_1
#define WEP_KEY						"1234567890"//"0123456789abcdef0123456789"  /* < Security Key in WEP Mode. 10 digit or 26 digit */
#define WEP_AUTH_TYPE				WEP_ANY

#endif

void mode_ap(void *argument);
#ifdef __cplusplus
}
#endif

#endif /* AP_H_INCLUDED */