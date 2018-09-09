/**
 * \file
 *
 * \brief Eddystone Beacons Support
 *
 * Copyright (c) 2017-2018 Microchip Technology Inc. and its subsidiaries.
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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef __EDDYSTONE_BEACON_H__
#define __EDDYSTONE_BEACON_H__

#include "at_ble_api.h"
#include "ble_manager.h"
#include "conf_eddystone.h"

/****************************************************************************************
 *							        Macros	                                     		            *
****************************************************************************************/

/** @brief  16-bit Eddystone Service UUID */
#define EDDYSTONE_16_BIT_UUID				0xFEAA

/**@brief Total number of bytes for the Eddystone UID frame. */
#define EDDYSTONE_TOTAL_UID_FRAME_LENGTH	(28)

/**@brief Total number of bytes for the Eddystone TLM frame. */
#define EDDYSTONE_TOTAL_TLM_FRAME_LENGTH	(22)

/**@brief Version number of the supported TLM frame. */
#define EDDYSTONE_TLM_FRAME_VERSION			(0x00)


/** Eddystone-URL configuration service defines */

/** @brief  128-bit UUID of the Eddystone-URL configuration service */
#define EDDYSTONE_URL_CONF_SERVICE_UUID		("\xd8\x81\xc9\x1a\xb9\x99\x96\xab\xba\x40\x86\x87\x80\x20\x0c\xee")

/** @brief  Advertisement interval  between 0x0020 and 0x4000 in 0.625ms units [20ms to 10.24s]. */
#define ESURL_CONF_ADV_INTERVAL				(160)	// 100 ms

/** @brief Advertisement timeout between 0x0001 and 0x3FFF in seconds, 0x0000 disables time-out. */
#define ESURL_CONF_ADV_TIMEOUT				(655)	// 10 min

/** @brief  Maximum number of bytes in the encoded URL including the encoded scheme prefix */
#define EDDYSTONE_URI_DATA_MAX_SIZE			(17 + 1)

/** @brief  Value of the URI flag characteristics */
#define EDDYSTONE_URI_FLAG					(0x10)

/** @brief  Minimum beacon interval in ms (100ms) */
#define EDDYSTONE_BEACON_INTERVAL_MS_MIN	(100)

/** @brief Maximum beacon interval in ms (10240ms or 10.24s) */
#define EDDYSTONE_BEACON_INTERVAL_MS_MAX	(10240)

/**@brief Conversion between float and 8:8 fixed point format. */
#define FLOAT_2_FIX88(a)					((int16_t)((a)*256.0))
#define FIX88_2_FLOAT(a)					((float)(a/256.0))

/**@brief Conversion between integer and 8:8 fixed point format. */
#define INT_2_FIX88(a)						(((int16_t)(a)) << 8)
#define FIX88_2_INT(a)						((int16_t)((a) >> 8))

/****************************************************************************************
 *							    Enumerations	                                   	            *
****************************************************************************************/

/** @brief  Three Types of Eddystone frames */
typedef enum 
{
	EDDYSTONE_UID = 0x00,
	EDDYSTONE_URL = 0x10,
	EDDYSTONE_TLM = 0x20,
} eddystone_frame_t;

/** @brief  TX power modes supported */
typedef enum
{
	TX_POWER_MODE_LOWEST = 0x00,
	TX_POWER_MODE_LOW,    /* Default Mode */
	TX_POWER_MODE_MEDIUM,
	TX_POWER_MODE_HIGH,
} eddystone_tx_power_mode_t;

/** Different modes present in Eddystone application */
typedef enum
{
	EDDYSTONE_IDLE_MODE = 0x00,
	EDDYSTONE_BEACON_MODE,
	EDDYSTONE_URL_CONFIG_MODE_ADV,
	EDDYSTONE_URL_CONFIG_MODE_CONNECTED,
} eddystone_mode_t;

/** @brief Different characteristics present in Eddystone URL configuration service */
typedef enum {
	CHAR_LOCK_STATE = 0x00,
	/* Optional characteristics enabled when lock feature is enabled */
#if defined(ES_URL_CONF_ENABLE_LOCK) && (ES_URL_CONF_ENABLE_LOCK == 1)
	CHAR_LOCK,
	CHAR_UNLOCK,
#endif
	CHAR_URI_DATA,
	CHAR_URI_FLAGS,
	CHAR_ADV_TX_POWER_LEVELS,
	CHAR_TX_POWER_MODE,
	CHAR_BEACON_PERIOD,
	CHAR_RESET,
	ESURL_CONF_CHAR_NUM,
} esurl_conf_serv_characteristic_t;

/****************************************************************************************
*							        Structures                                     			     *
****************************************************************************************/

/** @brief Eddystone-UID beacon structure. */
typedef union beacon_id_t
{
	/** 16-byte ID in big-endian format. */
	uint8_t value[16];						
	struct {
		/** 10-byte Namespace ID in big-endian format. */
		uint8_t namespace_id[10];
		/** 6-byte Instance ID in big-endian format. */	
		uint8_t instance_id[6];				
	};
} beacon_id_t;

/** @brief Eddystone-TLM frame data  */
typedef struct eddystone_tlm_data_t
{
	/** Batter voltage in mV units. */
	uint16_t battery_voltage_mV;
	/** Temperature in 8:8 fixed point representation. */
	int16_t temperature;
} eddystone_tlm_data_t;

/** @brief URL configuration service GATT characteristics */
typedef struct esurl_conf_gatt_service_t
{
	at_ble_uuid_t	uuid;
	at_ble_handle_t	handle;
	at_ble_characteristic_t	chars[ESURL_CONF_CHAR_NUM];
} esurl_conf_gatt_service_t;

/****************************************************************************************
 *								Globals								             *
 ****************************************************************************************/

/** @brief  Eddystone GAP events callback handler */
extern const ble_gap_event_cb_t esurl_conf_gap_cb;

/** @brief  Eddystone GATT server events callback handler */ 
extern const ble_gatt_server_event_cb_t esurl_conf_gatt_server_cb;

/** @brief  Eddystone-URL configuration service information */
extern esurl_conf_gatt_service_t esurl_conf_service;

/** @brief  100ms counter value used in the creation of TLM frame. 
 * This counter variable needs to be updated by the application for every 100ms */
extern volatile uint32_t eddystone_100ms_counter;

/****************************************************************************************
 *						            Functions		                                                       *
 ****************************************************************************************/

/**@brief Eddystone initialization; all UID and URL data are reset to default values. 
 * 
 * @return @ref AT_BLE_SUCCESS  Initialized successfully.
 * @return @ref AT_BLE_FAILURE  Error in initialization.
 */
at_ble_status_t eddystone_init(void);

/**@brief  Start sending Eddystone UID beacons continuously
 *  The UID frame is created out of parameters like Beacon ID and calibrated tx power at 0m.
 *
 * @return @ref AT_BLE_SUCCESS  Successfully started UID beacons.
 * @return @ref at_ble_status_t  Failed to start the beacons.
 */
at_ble_status_t eddystone_uid_frame_send(void);

/**@brief  Start sending Eddystone URL beacons continuously
 *  The URL frame is created out of encoded URL and advertised tx power level at 0m.
 *
 * @return @ref AT_BLE_SUCCESS  Successfully started URL beacons.
 * @return @ref at_ble_status_t  Failed to start the beacons.
 */
at_ble_status_t eddystone_url_frame_send(void);

/**@brief  Start sending Eddystone TLM beacons continuously 
 *  The TLM frame is created out of battery voltage value, beacon temperature, 
 *  advertisement PDU count and time since boot up.
 *
 * @return @ref AT_BLE_SUCCESS Successfully started TLM frames.
 * @return @ref at_ble_status_t  Failed to start the beacons.
 */
at_ble_status_t eddystone_tlm_frame_send(eddystone_tlm_data_t *tlm_data);

/**@brief  Stop transmitting Eddystone beacons
 *
 * @return @ref AT_BLE_SUCCESS  Stopped beacons successfully.
 * @return @ref at_ble_status_t  Failed to stop beacon transmissions.
 */
at_ble_status_t eddystone_adv_stop(void);

/**@brief  Update the Eddystone URL value
 *
 * @param[in] url_val  Pointer to the encoded URL data.
 * @param[in] len  Number of bytes in the URL data.
 * @return @ref AT_BLE_SUCCESS  Updated URL value successfully
 * @return @ref AT_BLE_FAILURE  Failed to update the URL value.
 */
at_ble_status_t eddystone_set_encoded_url(uint8_t *url_val, uint8_t len);

/**@brief  Update the TX power mode to be advertised. 
 * This change in TX power mode will set the radio TX power accordingly.
 *
 * @param[in]  tx_power_mode  One of the four power modes supported by Eddystone (@ref eddystone_tx_power_mode_t)
 * @return @ref AT_BLE_SUCCESS  Updated TX power mode successfully.
 * @return @ref AT_BLE_FAILURE  Failed to update the TX power mode.
 */
at_ble_status_t eddystone_set_power_mode(eddystone_tx_power_mode_t tx_power_mode);

/**@brief  Update the beacon interval.  This update requires a restart of the Eddystone frames
 *  to use this new beacon interval value.
 *
 * @param[in]  beacon_interval_ms  Interval period in ms [100ms to 10240ms]
 * @return @ref AT_BLE_SUCCESS  Updated the beacon interval successfully.
 * @return @ref AT_BLE_FAILURE  Failed to set the beacon interval.
 */
at_ble_status_t eddystone_set_beacon_interval(uint16_t beacon_interval_ms);

/**@brief  Update the Eddystone-UID (beacon ID).
 *
 * @param[in]  uid  The 16-byte UID value to be set; It must be in big endian format.
 *				UID consists of 10-byte Namespace ID and 6-byte Instance ID.
 * @return @ref AT_BLE_SUCCESS  Updated the beacon ID successfully.
 * @return @ref AT_BLE_FAILURE  Failed to set the beacon ID.
 */
at_ble_status_t eddystone_set_beacon_id(beacon_id_t *uid);

/**@brief  Update the calibrated TX power value present in the Eddystone-UID frame.
 *
 * @param[in]  tx_power_0m  Calibrated TX power level at 0 meters which is advertised in the UID frame.
 * @param[in]  radio_tx_power  Radio TX power to be set in the BLE device to achieve the advertised TX power at 0 meters.
 * @return @ref AT_BLE_SUCCESS  Updated the TX power levels successfully.
 * @return @ref AT_BLE_FAILURE  Failed to update the TX power levels.
 */
at_ble_status_t eddystone_set_uid_calib_tx_power(int8_t tx_power_0m, at_ble_tx_power_level_t radio_tx_power);

/**@brief  Obtain the current mode of the Eddystone beacon.
 *
 * @return @ref eddystone_mode_t  Returns one of the three modes in which the beacon operates.
 */
eddystone_mode_t eddystone_get_mode(void);

/**@brief  Eddystone URL Configuration Service and Characteristics initialization.
 *  It initializes the given service structure with required default values and registers it with the BLE stack.
 *  This configuration service should be used only when Eddystone-URL beacons are used.
 *  (Must be called only once by the user.)
 *
 * @param[in] serv  Pointer to store Eddystone configuration service information.
 * @return  Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t.
 */
at_ble_status_t esurl_conf_service_init(esurl_conf_gatt_service_t *serv);

/**@brief Start the Eddystone-URL configuration service.
 *  It starts the advertisement enabling any Central device to connect to the service.
 *
 * @param[in]  adv_interval  Advertising interval between @ref AT_BLE_ADV_INTERVAL_MIN and @ref AT_BLE_ADV_INTERVAL_MAX in 0.625 ms units (20ms to 10.24s).
 * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t.
 */
at_ble_status_t esurl_conf_adv_start(uint16_t adv_interval);

#endif /* __EDDYSTONE_BEACON_H__  */