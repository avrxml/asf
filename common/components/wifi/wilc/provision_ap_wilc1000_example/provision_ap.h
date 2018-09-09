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

/** Wi-Fi Settings */

/** AP mode Settings */
#define MAIN_WLAN_SSID                 "WILC3000_AP" /* < SSID */
#define MAIN_WLAN_AUTH                 M2M_WIFI_SEC_OPEN /* < Security manner */
#define MAIN_WLAN_CHANNEL              M2M_WIFI_CH_6 // (6) /* < Channel number */
#define MAIN_WLAN_PSK				   "12345678" /* Replace Assigned < Password for Destination SSID > */

#define MAIN_WIFI_M2M_PRODUCT_NAME        "PROVISION AP TEST"

#define MAIN_WIFI_M2M_SERVER_IP			0xFFFFFFFF //((uint32_t)0xc0a80102UL) /* 255.255.255.255 */
#define MAIN_WIFI_M2M_SERVER_PORT		80

/** Using IP address. */
#define IPV4_BYTE(val, index)          ((val >> (index * 8)) & 0xFF)

/** Receive buffer size. */
#define MAIN_WIFI_M2M_BUFFER_SIZE      1024


typedef sint8			SOCKET;


void provision_ap_task(void *argument);


/** Header Settings */
#define HEADER_VERSION1					0x80000000
#define RUN_NOW							0x00000001



#ifdef __cplusplus
}
#endif

#endif // APPS_H_INCLUDED
