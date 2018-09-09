/**
 * \file
 *
 * \brief Bas MultiPeripheral/ Find Me Central declarations
 *
 * Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
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

/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Atmel
 * Support</a>
 */

#ifndef __MUTLIROLE_MULTICONNECT_H__
#define __MUTLIROLE_MULTICONNECT_H__



typedef enum bas_notification_state{
	BAS_NOTIFICATION_DISABLED,
	BAS_NOTIFICATION_ENABLED,
	BAS_NOTIFICATION_SENT,
	BAS_NOTIFICATION_CONFIRMED
}bas_notification_state_t;

typedef enum fmp_scan_state{
	FMP_SCANNING,
	FMP_IDLE,
	FMP_CONNECTED,
}fmp_scan_state_t;

typedef struct bas_app_state{
	at_ble_handle_t conn_handle;
	bas_notification_state_t notification_state;
	bool bas_level_notified;
}bas_app_state_t;

typedef struct fmp_app_state {
	at_ble_addr_t fmp_peer_address;
	at_ble_handle_t fmp_conn_handle;
}fmp_app_state_t;

#define FMP_ASCII_TO_DECIMAL_VALUE      ('0')
#define AD_TYPE_COMPLETE_LIST_UUID		0x03
#define BATTERY_UPDATE_INTERVAL	(1) //1 second
#define BATTERY_MAX_LEVEL		(100)
#define BATTERY_MIN_LEVEL		(0)
/** @brief APP_BAS_FAST_ADV between 0x0020 and 0x4000 in 0.625 ms units (20ms to 10.24s). */
#define APP_BAS_FAST_ADV				(1600) //1600 ms

/** @brief APP_BAS_ADV_TIMEOUT Advertising time-out between 0x0001 and 0x028F in seconds, 0x0000 disables time-out.*/
#define APP_BAS_ADV_TIMEOUT				(60) // 60 Secs

#define BAS_MAX_DEVICE_CONNECTION (7)

/**@brief Connect to a peer device
*
*/
at_ble_status_t app_connect_request(at_ble_scan_info_t *scan_buffer,
uint8_t index);


/**@brief Find me sending the alert level to peer devices after checking the rssi 
*
*/
static void fmp_simulation_task(void);
 
#endif /*__MUTLIROLE_MULTICONNECT_H__*/
