/**
 * \file
 *
 * \brief OTA Firmware Update Configuration
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

#ifndef OTA_FW_UPDATE_H_INCLUDED
#define OTA_FW_UPDATE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/** Wi-Fi AP Settings. */
#define MAIN_WLAN_SSID                       "DEMO_AP" /**< Destination SSID */
#define MAIN_WLAN_AUTH                       M2M_WIFI_SEC_WPA_PSK /**< Security manner */
#define MAIN_WLAN_PSK                        "12345678" /**< Password for Destination SSID */

/** Content URI for download. */
#define MAIN_HTTP_FILE_URL					 "http://192.168.0.100:8080/ota.bin"

/** Maximum size for packet buffer. */
#define MAIN_BUFFER_MAX_SIZE				 (1446)

typedef enum {
	NOT_READY = 0, /*!< Not ready. */
	STORAGE_READY = 0x01, /*!< Storage is ready. */
	WIFI_CONNECTED = 0x02, /*!< Wi-Fi is connected. */
	GET_REQUESTED = 0x04, /*!< GET request is sent. */
	DOWNLOADING = 0x08, /*!< Running to download. */
	COMPLETED = 0x10, /*!< Download completed. */
	CANCELED = 0x20 /*!< Download canceled. */
} download_state;


/** Sector details of flash1 */
#define SECT0_SMALLERSECTOR_SIZE		8192  //8KB
#define SECT0_LARGERSECTOR_SIZE			49152 //48KB
#define SECTOR_SIZE						65536 //64KB
#define SMALLERSECTOR_COUNT				2
#define NO_OF_SECTORS					16

void ota_firmware_update(void *argument);

#ifdef __cplusplus
}
#endif

#endif /* OTA_FW_UPDATE_H_INCLUDED */
