/**
 * \file
 *
 * \brief Eddystone Beacons Support
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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
 
/****************************************************************************************
 *							        Includes	                                        *
****************************************************************************************/

#include "eddystone.h"

/****************************************************************************************
 *						     Macros	                                     			    *
****************************************************************************************/

/** Extract LSB and MSB from 16-bit value */
#define LOW_BYTE(x)			(uint8_t)(((uint16_t)x) & 0xFF)
#define HIGH_BYTE(x)		(uint8_t)((((uint16_t)x) >> 8) & 0xFF)

/** Convert ms to 0.625ms units; Required to set advertisement intervals */
#define CONVERT_MS_TO_625_US_UNIT(x)		((x * 16)/10)

/** Eddystone specific defines */
#define EDDYSTONE_SERV_DATA_LEN_OFFSET		(4)
#define EDDYSTONE_FRAME_TYPE_OFFSET			(8)
#define EDDYSTONE_FRAME_HEADER_LEN			(8)
#define EDDYSTONE_MAX_URL_ENC_LEN			(18)

/** Eddystone URL configuration service defines */
#define ESURL_CONF_CHAR_UUID_DIFF_OFFSET		(12)
#define ESURL_LOCK_CODE_MAX_SIZE				(16)
#define ESURL_URI_FLAG_SIZE						(1)
#define EDDYSTONE_TX_POWER_MODES_NUM			(4)

/** Possible lock states */
#define LOCKED		(1)
#define UNLOCKED	(0)

#define EDDYSTONE_HEADER(adv_data, index)	do {\
												uint8_t pos = index;\
												adv_data[index++] = 0x03;\
												adv_data[index++] = (uint8_t)COMPLETE_LIST_16BIT_SERV_UUIDS;\
												adv_data[index++] = LOW_BYTE(EDDYSTONE_16_BIT_UUID);\
												adv_data[index++] = HIGH_BYTE(EDDYSTONE_16_BIT_UUID);\
												adv_data[index++] = 0x00;\
												adv_data[index++] = (uint8_t)SERVICE_DATA;\
												adv_data[index++] = adv_data[pos + 2];\
												adv_data[index++] = adv_data[pos + 3];\
											} while(0)
											
/****************************************************************************************
*							       Enumerations	                                        *
****************************************************************************************/

/**@brief One byte of the UUID value that changes for each characteristics
 *  of the URL configuration service 
 */
typedef enum {
	UUID_CHAR_LOCK_STATE = 0x81,
	UUID_CHAR_LOCK = 0x82,
	UUID_CHAR_UNLOCK = 0x83,
	UUID_CHAR_URI_DATA = 0x84,
	UUID_CHAR_FLAGS = 0x85,
	UUID_CHAR_ADV_TX_POWER = 0x86,
	UUID_CHAR_TX_POWER_MODE = 0x87,
	UUID_CHAR_BEACON_PERIOD = 0x88,
	UUID_CHAR_RESET = 0x89,
	UUID_CHAR_RESERVED = 0x90,
} esurl_conf_char_uuid_value_t;


/****************************************************************************************
 *							        Structures                                     		*
****************************************************************************************/

/**@brief Eddystone-UID beacon data */
typedef struct esuid_data_t {
	/** Tx power at 0 meters, in dBm; Valid range [-100 dBm, +20 dBm]. */
	int8_t tx_power_0m;
	/** Radio TX power to be set in the device to achieve the above TX power at 0 meters */			
	at_ble_tx_power_level_t radio_tx_power;
	/** 16-byte Beacon ID. */
	beacon_id_t beacon_id;
} esuid_data_t;

/**@brief Eddystone-URL beacon data */
typedef struct esurl_data_t
{
	/** State that shows beacon is LOCKED or UNLOCKED */
	uint8_t lock_state;
	/** 16-byte single time lock code */
	uint8_t lock_code[ESURL_LOCK_CODE_MAX_SIZE];
	uint8_t unlock_code[ESURL_LOCK_CODE_MAX_SIZE];
	uint8_t lock_code_len;
	/** URI data in encoded form */
	char uri_data[EDDYSTONE_URI_DATA_MAX_SIZE];
	uint8_t uri_len;
	uint8_t uri_flags;
	/** Advertised TX power level at 0 meters */
	int8_t adv_tx_power_level[EDDYSTONE_TX_POWER_MODES_NUM];
	/** Radio TX power level set in the BLE device */
	at_ble_tx_power_level_t radio_tx_power_level[EDDYSTONE_TX_POWER_MODES_NUM];
	/** Four TX power modes supported in Eddystone */
	eddystone_tx_power_mode_t tx_power_mode;
	/** Beacon interval in ms */
	uint8_t beacon_period[2]; 
	
} esurl_data_t;

/****************************************************************************************
 *						Function Prototypes                                     		*
****************************************************************************************/

/** Event handling callback functions */
at_ble_status_t esurl_char_changed_event(void *params);
at_ble_status_t esurl_write_authorize_event(void *params);
at_ble_status_t esurl_read_authorize_event(void *params);
at_ble_status_t esurl_dev_connected_event(void *params);
at_ble_status_t esurl_dev_disconnected_event(void *params);
at_ble_status_t esurl_paired_event(void *params);

/** Functions to reset the UID and URL related parameters to default values */
static void esuid_data_reset(void);
static void esurl_data_reset(void);
static at_ble_status_t esurl_conf_chars_reset(void);

/****************************************************************************************
 *							Globals                                   					*
****************************************************************************************/
esurl_conf_gatt_service_t esurl_conf_service;
volatile eddystone_mode_t es_mode = EDDYSTONE_BEACON_MODE;
static at_ble_handle_t es_conf_conn_handle;
volatile uint32_t eddystone_100ms_counter = 0;

static volatile uint8_t eddystone_adv_enabled = 0;
static uint8_t esurl_conf_char_initialized = 0;
static uint8_t es_adv_data[AT_BLE_ADV_MAX_SIZE];
static uint8_t es_url_adv_data[AT_BLE_ADV_MAX_SIZE];
static uint8_t es_scan_data[AT_BLE_ADV_MAX_SIZE];

/** Eddystone-URL related data */
esurl_data_t esurl_data;
/** Eddystone-UID related data */
esuid_data_t esuid_data;

/** Eddystone GAP events handler initialization */
const ble_gap_event_cb_t esurl_conf_gap_cb = {
	.connected = esurl_dev_connected_event,
	.disconnected = esurl_dev_disconnected_event,
	.pair_done = esurl_paired_event,
};

/** Eddystone GATT server events handler initialization */
const ble_gatt_server_event_cb_t esurl_conf_gatt_server_cb = {
	.characteristic_changed = esurl_char_changed_event,
	.write_authorize_request = esurl_write_authorize_event,
};

/****************************************************************************************
 *							Functions                                 					*
****************************************************************************************/

/**@brief Creates Eddystone-UID frame from 16-byte beacon ID and calibrated TX power value.
 *  It also set the radio TX power of the device 
 */
static at_ble_status_t eddystone_create_uid_frame(uint8_t *adv_data)
{
	uint8_t index = 0;

	EDDYSTONE_HEADER(adv_data, index);
	
	// Eddystone frame now
	adv_data[index++] = EDDYSTONE_UID;
	adv_data[index++] = esuid_data.tx_power_0m;
	// Set the Radio TX power now
	at_ble_tx_power_set(esuid_data.radio_tx_power);
	
	// Copy 16-byte Beacon ID
	memcpy(adv_data + index, esuid_data.beacon_id.value, sizeof(esuid_data.beacon_id.value));
	index += sizeof(esuid_data.beacon_id.value);

	// RFU
	adv_data[index++] = 0x00;
	adv_data[index++] = 0x00;
	
	// Set the Service data length
	adv_data[EDDYSTONE_SERV_DATA_LEN_OFFSET] = 3 + 20;
	
	return AT_BLE_SUCCESS;
}

/**@brief  Creates Eddystone-URL frame from URI data and advertised TX power value.
 *  It also sets the radio TX power of the device 
 */
static at_ble_status_t eddystone_create_url_frame(uint8_t *adv_data)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	uint8_t index = 0, url_index = 0;
		
	EDDYSTONE_HEADER(adv_data, index);
	
	// Eddystone frame type
	adv_data[index++] = EDDYSTONE_URL;
	// Advertised TX power at 0m
	adv_data[index++] = esurl_data.adv_tx_power_level[esurl_data.tx_power_mode];
	// Set radio TX power now
	at_ble_tx_power_set(esurl_data.radio_tx_power_level[esurl_data.tx_power_mode]);
	
	// Need not encode the URL, just copy
	memcpy(adv_data + index, esurl_data.uri_data, esurl_data.uri_len);
	index += esurl_data.uri_len;
	url_index += esurl_data.uri_len;
	
	// Check if the length of the URL is within limit
	if (index > (EDDYSTONE_FRAME_HEADER_LEN + 2 + EDDYSTONE_MAX_URL_ENC_LEN)) {
		DBG_LOG("Error: URL length exceeded limit");
		return AT_BLE_FAILURE;
	}
	// Set the Service data length
	adv_data[EDDYSTONE_SERV_DATA_LEN_OFFSET] = index - 5;
	
	return status;
}

/**@brief  Creates Eddystone-TLM frame from battery voltage value, beacon temperature, 
 *  advertisement PDU count and 100ms counter. 
 */
static at_ble_status_t eddystone_create_tlm_frame(uint8_t *adv_data, eddystone_tlm_data_t *tlm_data)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	static uint32_t adv_cnt = 0;
	uint8_t index = 0;

	EDDYSTONE_HEADER(adv_data, index);
	
	// Eddystone frame now
	adv_data[index++] = EDDYSTONE_TLM;
	adv_data[index++] = EDDYSTONE_TLM_FRAME_VERSION;
	
	// Battery voltage in Big-Endian format
	adv_data[index++] = HIGH_BYTE(tlm_data->battery_voltage_mV);
	adv_data[index++] = LOW_BYTE(tlm_data->battery_voltage_mV);
	
	// Beacon temperature
	adv_data[index++] = HIGH_BYTE(tlm_data->temperature);
	adv_data[index++] = LOW_BYTE(tlm_data->temperature);
	
	// Advertisement PDU count
	status = at_ble_get_adv_cnt(&adv_cnt);
	if (status != AT_BLE_SUCCESS) return status;
	
	adv_data[index++] = ((uint8_t *)&adv_cnt)[3];
	adv_data[index++] = ((uint8_t *)&adv_cnt)[2];
	adv_data[index++] = ((uint8_t *)&adv_cnt)[1];
	adv_data[index++] = ((uint8_t *)&adv_cnt)[0];
	
	// Seconds counter (0.1 sec units)
	adv_data[index++] = ((uint8_t *)&eddystone_100ms_counter)[3];
	adv_data[index++] = ((uint8_t *)&eddystone_100ms_counter)[2];
	adv_data[index++] = ((uint8_t *)&eddystone_100ms_counter)[1];
	adv_data[index++] = ((uint8_t *)&eddystone_100ms_counter)[0];
	
	// Set the Service data length
	adv_data[EDDYSTONE_SERV_DATA_LEN_OFFSET] = 3 + 14;
	
	return status;
}

/**@brief  Starts the Eddystone advertisement */
static at_ble_status_t eddystone_adv_start(uint8_t *adv_data)
{
	static at_ble_adv_data_update_on_the_fly_t adv_update;
	at_ble_status_t status = AT_BLE_FAILURE;
	uint8_t length = 0;
	uint16_t adv_interval = convert_byte_array_to_16_bit(esurl_data.beacon_period);
	adv_interval = CONVERT_MS_TO_625_US_UNIT(adv_interval);
	
	// Beacon period ''0' disables beacon
	if(adv_interval == 0) {
		es_mode = EDDYSTONE_IDLE_MODE;
		return AT_BLE_FAILURE;
	}
	
	// Based on Eddystone frame type, the length of Adv data is calculated
	switch(adv_data[EDDYSTONE_FRAME_TYPE_OFFSET])
	{
		case EDDYSTONE_UID:
		length = EDDYSTONE_TOTAL_UID_FRAME_LENGTH;
		break;
		case EDDYSTONE_URL:
		length = adv_data[EDDYSTONE_SERV_DATA_LEN_OFFSET] + 5;
		break;
		case EDDYSTONE_TLM:
		length = EDDYSTONE_TOTAL_TLM_FRAME_LENGTH;
		break;
		default:
		DBG_LOG("Invalid Eddystone frame type!");
		return AT_BLE_FAILURE;
	}

	if (eddystone_adv_enabled) {
		// Advertisement is in progress, just change the adv data
		adv_update.adv_data = adv_data;
		adv_update.adv_data_len = length;
		adv_update.presence_bit_mask = ADV_DATA_PRESENT;
		status = at_ble_adv_data_update_on_the_fly(&adv_update);

		} else {
		status = at_ble_adv_data_set((uint8_t *)adv_data, length,
		NULL, 0);
		if(AT_BLE_SUCCESS != status)
		{
			return status;
		}

		//DBG_LOG("Adv interval: %d (625us units)", adv_interval);
		// Save the advertisement interval
		status = at_ble_adv_start(AT_BLE_ADV_TYPE_NONCONN_UNDIRECTED, AT_BLE_ADV_BROADCASTER_MODE,
		NULL, AT_BLE_ADV_FP_ANY, adv_interval, 0, 0);
		if (AT_BLE_SUCCESS == status) {
			eddystone_adv_enabled = 1;
			es_mode = EDDYSTONE_BEACON_MODE;
		}
	}
	
	return status;
}

/**@brief  Resets the UID related parameters to their default values */
static void esuid_data_reset(void)
{
	esuid_data_t *e = &esuid_data;
	e->tx_power_0m = CONF_ES_CALIB_TX_PWR_0m;
	e->radio_tx_power = CONF_ES_RADIO_TX_PWR_0m;
	memcpy(e->beacon_id.namespace_id, CONF_ES_NAMESPACE_ID, 10);
	memcpy(e->beacon_id.instance_id, CONF_ES_INSTANCE_ID, 6);
}

/**@brief Resets the URL related parameters to their default values */
static void esurl_data_reset(void)
{
	esurl_data.lock_state = CONF_ES_DEFAULT_LOCK_STATE;
	
	memcpy(esurl_data.lock_code, CONF_ES_DEFAULT_LOCK_CODE, CONF_ES_DEFAULT_LOCK_CODE_SIZE);
	esurl_data.lock_code_len = CONF_ES_DEFAULT_LOCK_CODE_SIZE;
	memcpy(esurl_data.uri_data, CONF_ES_DEFAULT_URL, CONF_ES_DEFAULT_URL_SIZE);
	esurl_data.uri_len = CONF_ES_DEFAULT_URL_SIZE;
	esurl_data.uri_flags = EDDYSTONE_URI_FLAG;
	
	esurl_data.adv_tx_power_level[TX_POWER_MODE_HIGH] = (int8_t) CONF_ES_DEFAULT_ADV_TX_PWR_HIGH;
	esurl_data.adv_tx_power_level[TX_POWER_MODE_MEDIUM] = (int8_t) CONF_ES_DEFAULT_ADV_TX_PWR_MEDIUM;
	esurl_data.adv_tx_power_level[TX_POWER_MODE_LOW] = (int8_t) CONF_ES_DEFAULT_ADV_TX_PWR_LOW;
	esurl_data.adv_tx_power_level[TX_POWER_MODE_LOWEST] = (int8_t) CONF_ES_DEFAULT_ADV_TX_PWR_LOWEST;
	
	esurl_data.radio_tx_power_level[TX_POWER_MODE_HIGH] = CONF_ES_DEFAULT_RADIO_TX_PWR_HIGH;
	esurl_data.radio_tx_power_level[TX_POWER_MODE_MEDIUM] = CONF_ES_DEFAULT_RADIO_TX_PWR_MEDIUM;
	esurl_data.radio_tx_power_level[TX_POWER_MODE_LOW] = CONF_ES_DEFAULT_RADIO_TX_PWR_LOW;
	esurl_data.radio_tx_power_level[TX_POWER_MODE_LOWEST] = CONF_ES_DEFAULT_RADIO_TX_PWR_LOWEST;
	
	esurl_data.tx_power_mode = TX_POWER_MODE_LOW;
	
	esurl_data.beacon_period[0] = LOW_BYTE(CONF_ES_DEFAULT_BEACON_PERIOD_MS);
	esurl_data.beacon_period[1] = HIGH_BYTE(CONF_ES_DEFAULT_BEACON_PERIOD_MS);
}

/**@brief  Resets all the configuration service characteristics to their default values */
static at_ble_status_t esurl_conf_chars_reset(void)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	// Reset all data
	esurl_data_reset();
	
	// Reset all the characteristics values
	status = at_ble_characteristic_value_set(esurl_conf_service.chars[CHAR_LOCK_STATE].char_val_handle, &esurl_data.lock_state, sizeof(esurl_data.lock_state));
	if (status != AT_BLE_SUCCESS) return status;
	status = at_ble_characteristic_value_set(esurl_conf_service.chars[CHAR_URI_DATA].char_val_handle, (uint8_t *) esurl_data.uri_data, esurl_data.uri_len);
	if (status != AT_BLE_SUCCESS) return status;
	status = at_ble_characteristic_value_set(esurl_conf_service.chars[CHAR_URI_FLAGS].char_val_handle, &esurl_data.uri_flags, 0x01);
	if (status != AT_BLE_SUCCESS) return status;
	status = at_ble_characteristic_value_set(esurl_conf_service.chars[CHAR_ADV_TX_POWER_LEVELS].char_val_handle, (uint8_t *) esurl_data.adv_tx_power_level, EDDYSTONE_TX_POWER_MODES_NUM);
	if (status != AT_BLE_SUCCESS) return status;
	status = at_ble_characteristic_value_set(esurl_conf_service.chars[CHAR_TX_POWER_MODE].char_val_handle,  &esurl_data.tx_power_mode, 0x01);
	if (status != AT_BLE_SUCCESS) return status;
	status = at_ble_characteristic_value_set(esurl_conf_service.chars[CHAR_BEACON_PERIOD].char_val_handle, esurl_data.beacon_period, 0x02);
	
	return status;
}

/**@brief  Initializes all Eddystone parameters */
at_ble_status_t eddystone_init(void)
{
	esuid_data_reset();
	esurl_data_reset();
	return AT_BLE_SUCCESS;
}

/**@brief  Update Eddystone URL in the encoded form */
at_ble_status_t eddystone_set_encoded_url(uint8_t *url_val, uint8_t len)
{
	if(len > EDDYSTONE_MAX_URL_ENC_LEN) {
		return AT_BLE_FAILURE;
	}
	memcpy(esurl_data.uri_data, url_val, len);
	esurl_data.uri_len = len;
	
	if(esurl_conf_char_initialized) {
		return at_ble_characteristic_value_set(esurl_conf_service.chars[CHAR_URI_DATA].char_val_handle, (uint8_t *) esurl_data.uri_data, esurl_data.uri_len);
	}
	return AT_BLE_FAILURE;
}

/**@brief  Update the advertised TX power mode in Eddystone-URL */
at_ble_status_t eddystone_set_power_mode(eddystone_tx_power_mode_t tx_power_mode)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	esurl_data.tx_power_mode = tx_power_mode;
	// Set the characteristics
	if (esurl_conf_char_initialized) {
		status = at_ble_characteristic_value_set(esurl_conf_service.chars[CHAR_TX_POWER_MODE].char_val_handle, &esurl_data.tx_power_mode, 0x01);
	}
	return status;
}

/**@brief  Update the beacon interval value. Restart sending the Eddystone frames to use the updated beacon interval */
at_ble_status_t eddystone_set_beacon_interval(uint16_t beacon_interval_ms)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	esurl_data.beacon_period[0] = LOW_BYTE(beacon_interval_ms);
	esurl_data.beacon_period[1] = HIGH_BYTE(beacon_interval_ms);
	
	// Update the characteristics
	if (esurl_conf_char_initialized) {
		status = at_ble_characteristic_value_set(esurl_conf_service.chars[CHAR_BEACON_PERIOD].char_val_handle, esurl_data.beacon_period, 0x02);
	}
	return status;
}

/**@brief  Update the Eddystone UID value */
at_ble_status_t eddystone_set_beacon_id(beacon_id_t *uid)
{
	// 10-bytes of Namespace ID
	memcpy(esuid_data.beacon_id.namespace_id, uid->namespace_id, 10);
	// 6-bytes of Instance ID
	memcpy(esuid_data.beacon_id.instance_id, uid->instance_id, 6);
	return AT_BLE_SUCCESS;
}

/**@brief  Update the calibrated TX power present in the Eddystone-UID frame */
at_ble_status_t eddystone_set_uid_calib_tx_power(int8_t tx_power_0m, at_ble_tx_power_level_t radio_tx_power)
{
	esuid_data.tx_power_0m = tx_power_0m;
	esuid_data.radio_tx_power = radio_tx_power;
	return AT_BLE_SUCCESS;
}

/**@brief  Get current mode of the Eddystone beacon */
eddystone_mode_t eddystone_get_mode(void)
{
	return es_mode;
}

/**@brief  Start sending the Eddystone UID frames */
at_ble_status_t eddystone_uid_frame_send() {
	//if (es_mode != EDDYSTONE_BEACON_MODE) {
		// Stopping adv is required to reset the advertisement interval
		eddystone_adv_stop();
	//}
	at_ble_status_t status = eddystone_create_uid_frame(es_adv_data);
	if (status == AT_BLE_SUCCESS) {
		status = eddystone_adv_start(es_adv_data);
	}
	return status;
}

/**@brief  Start sending the Eddystone URL frames */
at_ble_status_t eddystone_url_frame_send() {
	if (es_mode == EDDYSTONE_URL_CONFIG_MODE_ADV) {
		// Stopping adv is required to reset the advertisement interval
		//eddystone_adv_stop();
	} else if (es_mode == EDDYSTONE_URL_CONFIG_MODE_CONNECTED) {
		// Config mode is still connected; Disconnect and defer starting beacons
		return at_ble_disconnect(es_conf_conn_handle, AT_BLE_TERMINATED_BY_USER);
	}
	
	// NOTE: Stop advertisement - A workaround for BLUSDK-781 issue
	eddystone_adv_stop();
	at_ble_status_t status = eddystone_create_url_frame(es_url_adv_data);
	if (status == AT_BLE_SUCCESS) {
		status = eddystone_adv_start(es_url_adv_data);
	} 
	return status;
}

/**@brief  Start sending the Eddystone TLM frames */
at_ble_status_t eddystone_tlm_frame_send(eddystone_tlm_data_t *tlm_data) {
	//if (es_mode != EDDYSTONE_BEACON_MODE) {
		// Stopping adv is required to reset the advertisement interval
		eddystone_adv_stop();
	//}
	at_ble_status_t status = eddystone_create_tlm_frame(es_adv_data, tlm_data);
	if (status == AT_BLE_SUCCESS) {
		status = eddystone_adv_start(es_adv_data);
	}
	return status;
}

/**@brief  Stop the Eddystone advertisement */
at_ble_status_t eddystone_adv_stop(void)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	/* Advertisement is stopped */
	status = at_ble_adv_stop();
	if(AT_BLE_SUCCESS == status) {
		eddystone_adv_enabled = 0;
		es_mode = EDDYSTONE_IDLE_MODE;
	}
	return status;
}

/**@brief  Start the advertisement for Eddystone configuration service */
at_ble_status_t esurl_conf_adv_start(uint16_t adv_interval)
{
	uint8_t adv_len = 0;
	uint8_t scan_len = 0;
	at_ble_status_t status = AT_BLE_SUCCESS;
	
	// Create adv packet
	es_adv_data[adv_len++] = ADV_TYPE_SIZE + AT_BLE_UUID_128_LEN;  // Length
	es_adv_data[adv_len++] = (uint8_t)COMPLETE_LIST_128BIT_SERV_UUIDS;
	memcpy(es_adv_data + adv_len, EDDYSTONE_URL_CONF_SERVICE_UUID, AT_BLE_UUID_128_LEN);
	adv_len += AT_BLE_UUID_128_LEN;
	
	es_adv_data[adv_len++] = ADV_TYPE_SIZE + TX_POWER_LEVEL_SIZE;
	es_adv_data[adv_len++] = TX_POWER_LEVEL;
	es_adv_data[adv_len++] = esurl_data.adv_tx_power_level[TX_POWER_MODE_MEDIUM];
	// Set the radio TX power now
	at_ble_tx_power_set(esurl_data.radio_tx_power_level[TX_POWER_MODE_MEDIUM]);
	
	es_scan_data[scan_len++] = ADV_TYPE_SIZE + strlen(CONF_ES_CONF_ADV_DATA_COMPLETE_LOCAL_NAME);
	es_scan_data[scan_len++] = COMPLETE_LOCAL_NAME;
	memcpy(es_scan_data + scan_len, CONF_ES_CONF_ADV_DATA_COMPLETE_LOCAL_NAME, strlen(CONF_ES_CONF_ADV_DATA_COMPLETE_LOCAL_NAME));
	scan_len += strlen(CONF_ES_CONF_ADV_DATA_COMPLETE_LOCAL_NAME);
	
	if(eddystone_adv_enabled) {
		// Advertisement is in progress, so stop it
		eddystone_adv_stop();
	}
	
	// Set the adv packet with the stack
	status = at_ble_adv_data_set(es_adv_data, adv_len,
								 es_scan_data, scan_len);
			
	if(AT_BLE_SUCCESS != status) {
		return status;
	}
			
	// Start advertising connectable packets
	status = at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE,
							  NULL, AT_BLE_ADV_FP_ANY, adv_interval, ESURL_CONF_ADV_TIMEOUT, 0);
			
	if (AT_BLE_SUCCESS == status) {
		eddystone_adv_enabled = 1;
		es_mode = EDDYSTONE_URL_CONFIG_MODE_ADV;
	}
	
	return status;
}

/**@brief Initialize the Eddystone URL configuration service */
at_ble_status_t esurl_conf_service_init(esurl_conf_gatt_service_t *serv)
{
	at_ble_status_t status;
	uint8_t dummy = 0;
	// Reset all data
	esurl_data_reset();
	
	serv->handle = 0;
	serv->uuid.type = AT_BLE_UUID_128;
	memcpy(serv->uuid.uuid, EDDYSTONE_URL_CONF_SERVICE_UUID, AT_BLE_UUID_128_LEN);
	
	/* Initialize all the Characteristics of Eddystone configuration service */
	{
		/* Lock state characteristic */
		serv->chars[CHAR_LOCK_STATE].char_val_handle = 0;  /* Handle will be stored by stack */
		serv->chars[CHAR_LOCK_STATE].uuid.type = AT_BLE_UUID_128;
		/* Copy the same base UUID */
		memcpy(serv->chars[CHAR_LOCK_STATE].uuid.uuid, EDDYSTONE_URL_CONF_SERVICE_UUID, AT_BLE_UUID_128_LEN);
		/* Set Lock state's UUID (only one byte changes) */
		serv->chars[CHAR_LOCK_STATE].uuid.uuid[ESURL_CONF_CHAR_UUID_DIFF_OFFSET] = UUID_CHAR_LOCK_STATE;
		serv->chars[CHAR_LOCK_STATE].properties = AT_BLE_CHAR_READ;
		serv->chars[CHAR_LOCK_STATE].init_value = &esurl_data.lock_state;
		serv->chars[CHAR_LOCK_STATE].value_init_len = sizeof(esurl_data.lock_state);
		serv->chars[CHAR_LOCK_STATE].value_max_len = sizeof(esurl_data.lock_state);
	#if BLE_PAIR_ENABLE
		serv->chars[CHAR_LOCK_STATE].value_permissions = AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR;
	#else
		serv->chars[CHAR_LOCK_STATE].value_permissions = AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR;
	#endif
		serv->chars[CHAR_LOCK_STATE].user_desc = NULL;
		serv->chars[CHAR_LOCK_STATE].user_desc_len = 0;
		serv->chars[CHAR_LOCK_STATE].user_desc_max_len = 0;
		serv->chars[CHAR_LOCK_STATE].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_LOCK_STATE].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_LOCK_STATE].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_LOCK_STATE].user_desc_handle = 0;
		serv->chars[CHAR_LOCK_STATE].client_config_handle = 0;
		serv->chars[CHAR_LOCK_STATE].server_config_handle = 0;
		serv->chars[CHAR_LOCK_STATE].presentation_format = NULL;
	}
	
#if defined(ES_URL_CONF_ENABLE_LOCK) && (ES_URL_CONF_ENABLE_LOCK == 1)
	{
		/* Lock characteristic (single-use lock-code) */
		serv->chars[CHAR_LOCK].char_val_handle = 0;    /* Valid handle will be stored by stack */
		serv->chars[CHAR_LOCK].uuid.type = AT_BLE_UUID_128;
		/* Copy the same base UUID */
		memcpy(serv->chars[CHAR_LOCK].uuid.uuid, EDDYSTONE_URL_CONF_SERVICE_UUID, AT_BLE_UUID_128_LEN);
		/* Set Lock UUID */
		serv->chars[CHAR_LOCK].uuid.uuid[ESURL_CONF_CHAR_UUID_DIFF_OFFSET] = UUID_CHAR_LOCK;
		serv->chars[CHAR_LOCK].properties = AT_BLE_CHAR_WRITE;
		serv->chars[CHAR_LOCK].init_value = esurl_data.lock_code;
	
		serv->chars[CHAR_LOCK].value_init_len = esurl_data.lock_code_len;
		serv->chars[CHAR_LOCK].value_max_len = ESURL_LOCK_CODE_MAX_SIZE;
	#if BLE_PAIR_ENABLE
		serv->chars[CHAR_LOCK].value_permissions = AT_BLE_ATTR_WRITABLE_REQ_AUTHN_REQ_AUTHR;  /* Needs Authorization */
	#else
		serv->chars[CHAR_LOCK].value_permissions = AT_BLE_ATTR_WRITABLE_NO_AUTHN_REQ_AUTHR;
	#endif
		serv->chars[CHAR_LOCK].user_desc = NULL;
		serv->chars[CHAR_LOCK].user_desc_len = 0;
		serv->chars[CHAR_LOCK].user_desc_max_len = 0;
		serv->chars[CHAR_LOCK].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_LOCK].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_LOCK].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_LOCK].user_desc_handle = 0;
		serv->chars[CHAR_LOCK].client_config_handle = 0;
		serv->chars[CHAR_LOCK].server_config_handle = 0;
		serv->chars[CHAR_LOCK].presentation_format = NULL;
	}
	
	{
		/* Unlock characteristic (single-use lock-code) */
		serv->chars[CHAR_UNLOCK].char_val_handle = 0;    /* Valid handle will be stored by stack */
		serv->chars[CHAR_UNLOCK].uuid.type = AT_BLE_UUID_128;
		/* Copy the same base UUID */
		memcpy(serv->chars[CHAR_UNLOCK].uuid.uuid, EDDYSTONE_URL_CONF_SERVICE_UUID, AT_BLE_UUID_128_LEN);
		/* Set Unlock UUID */
		serv->chars[CHAR_UNLOCK].uuid.uuid[ESURL_CONF_CHAR_UUID_DIFF_OFFSET] = UUID_CHAR_UNLOCK;
		serv->chars[CHAR_UNLOCK].properties = AT_BLE_CHAR_WRITE;
		serv->chars[CHAR_UNLOCK].init_value = esurl_data.lock_code;
		serv->chars[CHAR_UNLOCK].value_init_len = esurl_data.lock_code_len;
		serv->chars[CHAR_UNLOCK].value_max_len = ESURL_LOCK_CODE_MAX_SIZE;
	#if BLE_PAIR_ENABLE
		serv->chars[CHAR_UNLOCK].value_permissions = AT_BLE_ATTR_WRITABLE_REQ_AUTHN_REQ_AUTHR;    /* Needs Authorization */
	#else
		serv->chars[CHAR_UNLOCK].value_permissions = AT_BLE_ATTR_WRITABLE_NO_AUTHN_REQ_AUTHR;
	#endif
	
		serv->chars[CHAR_UNLOCK].user_desc = NULL;
		serv->chars[CHAR_UNLOCK].user_desc_len = 0;
		serv->chars[CHAR_UNLOCK].user_desc_max_len = 0;
		serv->chars[CHAR_UNLOCK].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_UNLOCK].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_UNLOCK].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_UNLOCK].user_desc_handle = 0;
		serv->chars[CHAR_UNLOCK].client_config_handle = 0;
		serv->chars[CHAR_UNLOCK].server_config_handle = 0;
		serv->chars[CHAR_UNLOCK].presentation_format = NULL;
	}
#endif // ES_URL_CONF_ENABLE_LOCK
	{
		/* URI Data characteristic */
		serv->chars[CHAR_URI_DATA].char_val_handle = 0;    /* Valid handle will be stored by stack */
		serv->chars[CHAR_URI_DATA].uuid.type = AT_BLE_UUID_128;
		/* Copy the same base UUID */
		memcpy(serv->chars[CHAR_URI_DATA].uuid.uuid, EDDYSTONE_URL_CONF_SERVICE_UUID, AT_BLE_UUID_128_LEN);
		/* Set URI Data UUID */
		serv->chars[CHAR_URI_DATA].uuid.uuid[ESURL_CONF_CHAR_UUID_DIFF_OFFSET] = UUID_CHAR_URI_DATA;
		serv->chars[CHAR_URI_DATA].properties = (AT_BLE_CHAR_READ | AT_BLE_CHAR_WRITE);
		serv->chars[CHAR_URI_DATA].init_value = (uint8_t *) esurl_data.uri_data;
		serv->chars[CHAR_URI_DATA].value_init_len = CONF_ES_DEFAULT_URL_SIZE;
		serv->chars[CHAR_URI_DATA].value_max_len = EDDYSTONE_URI_DATA_MAX_SIZE;
		/* Both read and write */
	#if BLE_PAIR_ENABLE
		serv->chars[CHAR_URI_DATA].value_permissions = (AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_REQ_AUTHN_REQ_AUTHR);
	#else
		serv->chars[CHAR_URI_DATA].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_REQ_AUTHR);
	#endif
		serv->chars[CHAR_URI_DATA].user_desc = NULL;
		serv->chars[CHAR_URI_DATA].user_desc_len = 0;
		serv->chars[CHAR_URI_DATA].user_desc_max_len = 0;
		serv->chars[CHAR_URI_DATA].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_URI_DATA].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_URI_DATA].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_URI_DATA].user_desc_handle = 0;
		serv->chars[CHAR_URI_DATA].client_config_handle = 0;
		serv->chars[CHAR_URI_DATA].server_config_handle = 0;
		serv->chars[CHAR_URI_DATA].presentation_format = NULL;
	}

	{
		/* URI Flags characteristic */
		serv->chars[CHAR_URI_FLAGS].char_val_handle = 0;    /* Valid handle will be stored by stack */
		serv->chars[CHAR_URI_FLAGS].uuid.type = AT_BLE_UUID_128;
		/* Copy the same base UUID */
		memcpy(serv->chars[CHAR_URI_FLAGS].uuid.uuid, EDDYSTONE_URL_CONF_SERVICE_UUID, AT_BLE_UUID_128_LEN);
		/* Set URI Data UUID */
		serv->chars[CHAR_URI_FLAGS].uuid.uuid[ESURL_CONF_CHAR_UUID_DIFF_OFFSET] = UUID_CHAR_FLAGS;
		serv->chars[CHAR_URI_FLAGS].properties = (AT_BLE_CHAR_READ | AT_BLE_CHAR_WRITE);
		serv->chars[CHAR_URI_FLAGS].init_value = &esurl_data.uri_flags;
		serv->chars[CHAR_URI_FLAGS].value_init_len = 0x01;
		serv->chars[CHAR_URI_FLAGS].value_max_len = 0x01;
		/* Both read and write */
	#if BLE_PAIR_ENABLE
		serv->chars[CHAR_URI_FLAGS].value_permissions = (AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_REQ_AUTHN_REQ_AUTHR);
	#else
		serv->chars[CHAR_URI_FLAGS].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_REQ_AUTHR);
	#endif
	
		serv->chars[CHAR_URI_FLAGS].user_desc = NULL;
		serv->chars[CHAR_URI_FLAGS].user_desc_len = 0;
		serv->chars[CHAR_URI_FLAGS].user_desc_max_len = 0;
		serv->chars[CHAR_URI_FLAGS].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_URI_FLAGS].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_URI_FLAGS].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_URI_FLAGS].user_desc_handle = 0;
		serv->chars[CHAR_URI_FLAGS].client_config_handle = 0;
		serv->chars[CHAR_URI_FLAGS].server_config_handle = 0;
		serv->chars[CHAR_URI_FLAGS].presentation_format = NULL;
	}
	
	{
		/* Advertised TX Power Levels characteristic */
		serv->chars[CHAR_ADV_TX_POWER_LEVELS].char_val_handle = 0;    /* Valid handle will be stored by stack */
		serv->chars[CHAR_ADV_TX_POWER_LEVELS].uuid.type = AT_BLE_UUID_128;
		/* Copy the same base UUID */
		memcpy(serv->chars[CHAR_ADV_TX_POWER_LEVELS].uuid.uuid, EDDYSTONE_URL_CONF_SERVICE_UUID, AT_BLE_UUID_128_LEN);
		/* Set Adv TX Power Levels UUID */
		serv->chars[CHAR_ADV_TX_POWER_LEVELS].uuid.uuid[ESURL_CONF_CHAR_UUID_DIFF_OFFSET] = UUID_CHAR_ADV_TX_POWER;
		serv->chars[CHAR_ADV_TX_POWER_LEVELS].properties = (AT_BLE_CHAR_READ | AT_BLE_CHAR_WRITE);
		serv->chars[CHAR_ADV_TX_POWER_LEVELS].init_value = (uint8_t *) esurl_data.adv_tx_power_level;
		serv->chars[CHAR_ADV_TX_POWER_LEVELS].value_init_len = EDDYSTONE_TX_POWER_MODES_NUM;
		serv->chars[CHAR_ADV_TX_POWER_LEVELS].value_max_len = EDDYSTONE_TX_POWER_MODES_NUM;
		/* Both read and write */
	#if BLE_PAIR_ENABLE
		serv->chars[CHAR_ADV_TX_POWER_LEVELS].value_permissions = (AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_REQ_AUTHN_REQ_AUTHR);
	#else
		serv->chars[CHAR_ADV_TX_POWER_LEVELS].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_REQ_AUTHR);
	#endif	
		serv->chars[CHAR_ADV_TX_POWER_LEVELS].user_desc = NULL;
		serv->chars[CHAR_ADV_TX_POWER_LEVELS].user_desc_len = 0;
		serv->chars[CHAR_ADV_TX_POWER_LEVELS].user_desc_max_len = 0;
		serv->chars[CHAR_ADV_TX_POWER_LEVELS].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_ADV_TX_POWER_LEVELS].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_ADV_TX_POWER_LEVELS].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_ADV_TX_POWER_LEVELS].user_desc_handle = 0;
		serv->chars[CHAR_ADV_TX_POWER_LEVELS].client_config_handle = 0;
		serv->chars[CHAR_ADV_TX_POWER_LEVELS].server_config_handle = 0;
		serv->chars[CHAR_ADV_TX_POWER_LEVELS].presentation_format = NULL;
	}
	
	{
		/* TX Power Mode characteristic */
		serv->chars[CHAR_TX_POWER_MODE].char_val_handle = 0;    /* Value handle will be stored by stack */
		serv->chars[CHAR_TX_POWER_MODE].uuid.type = AT_BLE_UUID_128;
		/* Copy the same base UUID */
		memcpy(serv->chars[CHAR_TX_POWER_MODE].uuid.uuid, EDDYSTONE_URL_CONF_SERVICE_UUID, AT_BLE_UUID_128_LEN);
		/* Set TX Power Mode UUID */
		serv->chars[CHAR_TX_POWER_MODE].uuid.uuid[ESURL_CONF_CHAR_UUID_DIFF_OFFSET] = UUID_CHAR_TX_POWER_MODE;
		serv->chars[CHAR_TX_POWER_MODE].properties = (AT_BLE_CHAR_READ | AT_BLE_CHAR_WRITE);
		serv->chars[CHAR_TX_POWER_MODE].init_value = &esurl_data.tx_power_mode;
		serv->chars[CHAR_TX_POWER_MODE].value_init_len = 0x01;  /* TX Power mode is 1 byte */
		serv->chars[CHAR_TX_POWER_MODE].value_max_len = 0x01;
		/* Both read and write */
	#if BLE_PAIR_ENABLE
		serv->chars[CHAR_TX_POWER_MODE].value_permissions = (AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_REQ_AUTHN_REQ_AUTHR);
	#else
		serv->chars[CHAR_TX_POWER_MODE].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_REQ_AUTHR);
	#endif
		serv->chars[CHAR_TX_POWER_MODE].user_desc = NULL;
		serv->chars[CHAR_TX_POWER_MODE].user_desc_len = 0;
		serv->chars[CHAR_TX_POWER_MODE].user_desc_max_len = 0;
		serv->chars[CHAR_TX_POWER_MODE].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_TX_POWER_MODE].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_TX_POWER_MODE].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_TX_POWER_MODE].user_desc_handle = 0;
		serv->chars[CHAR_TX_POWER_MODE].client_config_handle = 0;
		serv->chars[CHAR_TX_POWER_MODE].server_config_handle = 0;
		serv->chars[CHAR_TX_POWER_MODE].presentation_format = NULL;
	}
	
	{
		/* Beacon Period characteristic */
		serv->chars[CHAR_BEACON_PERIOD].char_val_handle = 0;    /* Value handle will be stored by stack */
		serv->chars[CHAR_BEACON_PERIOD].uuid.type = AT_BLE_UUID_128;
		/* Copy the same base UUID */
		memcpy(serv->chars[CHAR_BEACON_PERIOD].uuid.uuid, EDDYSTONE_URL_CONF_SERVICE_UUID, AT_BLE_UUID_128_LEN);
		/* Set Beacon Period UUID */
		serv->chars[CHAR_BEACON_PERIOD].uuid.uuid[ESURL_CONF_CHAR_UUID_DIFF_OFFSET] = UUID_CHAR_BEACON_PERIOD;
		serv->chars[CHAR_BEACON_PERIOD].properties = (AT_BLE_CHAR_READ | AT_BLE_CHAR_WRITE);
		serv->chars[CHAR_BEACON_PERIOD].init_value = esurl_data.beacon_period;
		serv->chars[CHAR_BEACON_PERIOD].value_init_len = 0x02;  /* 16-bit beacon period */
		serv->chars[CHAR_BEACON_PERIOD].value_max_len = 0x02;
		/* Both read and write */
	#if BLE_PAIR_ENABLE
		serv->chars[CHAR_BEACON_PERIOD].value_permissions = (AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_REQ_AUTHN_REQ_AUTHR);
	#else
		serv->chars[CHAR_BEACON_PERIOD].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_REQ_AUTHR);
	#endif
		serv->chars[CHAR_BEACON_PERIOD].user_desc = NULL;
		serv->chars[CHAR_BEACON_PERIOD].user_desc_len = 0;
		serv->chars[CHAR_BEACON_PERIOD].user_desc_max_len = 0;
		serv->chars[CHAR_BEACON_PERIOD].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_BEACON_PERIOD].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_BEACON_PERIOD].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_BEACON_PERIOD].user_desc_handle = 0;
		serv->chars[CHAR_BEACON_PERIOD].client_config_handle = 0;
		serv->chars[CHAR_BEACON_PERIOD].server_config_handle = 0;
		serv->chars[CHAR_BEACON_PERIOD].presentation_format = NULL;
	}
	
	{
		/* Reset Characteristic */
		serv->chars[CHAR_RESET].char_val_handle = 0;    /* Value handle will be stored by stack */
		serv->chars[CHAR_RESET].uuid.type = AT_BLE_UUID_128;
		/* Copy the same base UUID */
		memcpy(serv->chars[CHAR_RESET].uuid.uuid, EDDYSTONE_URL_CONF_SERVICE_UUID, AT_BLE_UUID_128_LEN);
		/* Set Reset UUID */
		serv->chars[CHAR_RESET].uuid.uuid[ESURL_CONF_CHAR_UUID_DIFF_OFFSET] = UUID_CHAR_RESET;
		serv->chars[CHAR_RESET].properties = (AT_BLE_CHAR_WRITE);
		serv->chars[CHAR_RESET].init_value = &dummy;
		serv->chars[CHAR_RESET].value_init_len = 0x01;
		serv->chars[CHAR_RESET].value_max_len = 0x01;
		/* Only writeable */
	#if BLE_PAIR_ENABLE
		serv->chars[CHAR_RESET].value_permissions = (AT_BLE_ATTR_WRITABLE_REQ_AUTHN_REQ_AUTHR);
	#else
		serv->chars[CHAR_RESET].value_permissions = (AT_BLE_ATTR_WRITABLE_NO_AUTHN_REQ_AUTHR);
	#endif
		serv->chars[CHAR_RESET].user_desc = NULL;
		serv->chars[CHAR_RESET].user_desc_len = 0;
		serv->chars[CHAR_RESET].user_desc_max_len = 0;
		serv->chars[CHAR_RESET].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_RESET].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_RESET].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		serv->chars[CHAR_RESET].user_desc_handle = 0;
		serv->chars[CHAR_RESET].client_config_handle = 0;
		serv->chars[CHAR_RESET].server_config_handle = 0;
		serv->chars[CHAR_RESET].presentation_format = NULL;
	}
	
	status = (at_ble_primary_service_define(&(serv->uuid),
		&(serv->handle),
		NULL, 0,
		serv->chars, ESURL_CONF_CHAR_NUM));
	if(status == AT_BLE_SUCCESS) {
		esurl_conf_char_initialized = 1;	
	}
	return status;
}

/**@brief  Callback to handle the device connected event */
at_ble_status_t esurl_dev_connected_event(void *params)
{
	at_ble_connected_t *conn = (at_ble_connected_t *) params;
	if (conn->conn_status == AT_BLE_SUCCESS) {
		// Clear off adv flag
		eddystone_adv_enabled = 0;
		es_mode = EDDYSTONE_URL_CONFIG_MODE_CONNECTED;
		es_conf_conn_handle = conn->handle;
	}
	return conn->conn_status;
}

/**@brief  Callback to handle the device disconnected event */
at_ble_status_t esurl_dev_disconnected_event(void *params)
{
	es_mode = EDDYSTONE_IDLE_MODE;
	// Start the advertisement for the URL beacon
	eddystone_url_frame_send();
	DBG_LOG("Eddystone beacon mode");
	return AT_BLE_SUCCESS;
}

/**@brief  Callback to handle the device paired event */
at_ble_status_t esurl_paired_event(void *params)
{
	DBG_LOG("Paired");
	return AT_BLE_SUCCESS;
}

/**@brief  Callback to handle Characteristics changed event */
at_ble_status_t esurl_char_changed_event(void *params)
{
	at_ble_characteristic_changed_t *ch;
	at_ble_status_t status = AT_BLE_FAILURE;
	ch = (at_ble_characteristic_changed_t *)(params);
	
	DBG_LOG("Char changed: %d, %d, %d", ch->char_handle, ch->char_len, ch->char_offset);

	return status;
}

/**@brief  Callback to handle write authorization for the URL configuration characteristics */
at_ble_status_t esurl_write_authorize_event(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	at_ble_characteristic_write_request_t *wr;
	wr = (at_ble_characteristic_write_request_t *) params;
	uint16_t value = 0;
	
#if defined(ES_URL_CONF_ENABLE_LOCK) && (ES_URL_CONF_ENABLE_LOCK == 1)
	// Unlock
	if (wr->char_handle == esurl_conf_service.chars[CHAR_UNLOCK].char_val_handle) {
		if (wr->length != esurl_data.lock_code_len) {
			// Invalid length
			status = at_ble_write_authorize_reply(params, AT_BLE_ATT_INVALID_ATTRIBUTE_VAL_LEN);
		} else {
			if (0 == memcmp(wr->value, esurl_data.lock_code, esurl_data.lock_code_len)) {
				// Lock code is correct, unlock now
				esurl_data.lock_state = UNLOCKED;
				status = at_ble_characteristic_value_set(esurl_conf_service.chars[CHAR_LOCK_STATE].char_val_handle, (uint8_t *)&(esurl_data.lock_state), 0x01);
				if (status == AT_BLE_SUCCESS) {
					at_ble_write_authorize_reply(params, AT_BLE_SUCCESS);
					DBG_LOG("Unlocked");
				} else {
					// Unable to set lock state characteristics; Fatal error
					status = at_ble_write_authorize_reply(params, AT_BLE_ATT_INSUFF_AUTHOR);
				}
			} else {
				status = at_ble_write_authorize_reply(params, AT_BLE_ATT_INSUFF_AUTHOR);
			}
		}
	}
	
	// Lock
	else if (wr->char_handle == esurl_conf_service.chars[CHAR_LOCK].char_val_handle) {
		if ((wr->length != ESURL_LOCK_CODE_MAX_SIZE)) {
			// Invalid length
			status = at_ble_write_authorize_reply(params, AT_BLE_ATT_INVALID_ATTRIBUTE_VAL_LEN);
		} else {
			if (esurl_data.lock_state == LOCKED) {
				// Already locked => Insufficient authorization
				status = at_ble_write_authorize_reply(params, AT_BLE_ATT_INSUFF_AUTHOR);
			} else {
				// Lock now with single use lock code
				esurl_data.lock_state = LOCKED;
				status = at_ble_characteristic_value_set(esurl_conf_service.chars[CHAR_LOCK_STATE].char_val_handle, (uint8_t *)&(esurl_data.lock_state), 0x01);
				memcpy(esurl_data.lock_code, wr->value, wr->length);
				esurl_data.lock_code_len = wr->length;
				status = at_ble_write_authorize_reply(params, AT_BLE_SUCCESS);
				DBG_LOG("Locked");
			}
		}
	} 
	else {
		if (esurl_data.lock_state != UNLOCKED) {
			// Not unlocked yet; Unable to authorize anything
			status = at_ble_write_authorize_reply(params, AT_BLE_ATT_INSUFF_AUTHOR);
			return status;
		} else
#endif
		{
			// URI data
			if (wr->char_handle == esurl_conf_service.chars[CHAR_URI_DATA].char_val_handle) {
				memcpy(esurl_data.uri_data, wr->value, wr->length);
				esurl_data.uri_len = wr->length;
				status = at_ble_write_authorize_reply(params, AT_BLE_SUCCESS);
				// Set characteristic in DB again, as length is not updating properly
				status = at_ble_characteristic_value_set(esurl_conf_service.chars[CHAR_URI_DATA].char_val_handle, (uint8_t *) esurl_data.uri_data, esurl_data.uri_len);
			}
			// URI flag
			else if (wr->char_handle == esurl_conf_service.chars[CHAR_URI_FLAGS].char_val_handle) {
				if (wr->length != ESURL_URI_FLAG_SIZE) {
					// Invalid length
					status = at_ble_write_authorize_reply(params, AT_BLE_ATT_INVALID_ATTRIBUTE_VAL_LEN);
				} else {
					// All good; Read the first byte
					esurl_data.uri_flags = wr->value[0];
					status = at_ble_write_authorize_reply(params, AT_BLE_SUCCESS);
				}
			}
	
			// Adv Tx Power Levels
			else if (wr->char_handle == esurl_conf_service.chars[CHAR_ADV_TX_POWER_LEVELS].char_val_handle) {
				if (wr->length != EDDYSTONE_TX_POWER_MODES_NUM) {
					// Invalid length
					status = at_ble_write_authorize_reply (params, AT_BLE_ATT_INVALID_ATTRIBUTE_VAL_LEN);
				} else {
					// All good
					memcpy(esurl_data.adv_tx_power_level, wr->value, wr->length);
					status = at_ble_write_authorize_reply(params, AT_BLE_SUCCESS);
					// TODO: change the tx power
				}
			}
			// TX Power Mode  
			else if (wr->char_handle == esurl_conf_service.chars[CHAR_TX_POWER_MODE].char_val_handle) {
				if (wr->length != 0x01) {
					// Invalid length
					status = at_ble_write_authorize_reply(params, AT_BLE_ATT_INVALID_ATTRIBUTE_VAL_LEN);
				} else {
					if ((wr->value[0] > TX_POWER_MODE_HIGH) || (wr->value[0] < TX_POWER_MODE_LOWEST)) {
						// Invalid value
						status = at_ble_write_authorize_reply(params, AT_BLE_ATT_WRITE_NOT_PERMITTED);
					} else {
						// All good
						esurl_data.tx_power_mode = wr->value[0];
						status = at_ble_write_authorize_reply(params, AT_BLE_SUCCESS);
						// TODO: change the device tx power
					}
				}
			}

			// Beacon period
			else if (wr->char_handle == esurl_conf_service.chars[CHAR_BEACON_PERIOD].char_val_handle) {
				if (wr->length != 0x02) {
					status = at_ble_write_authorize_reply(params, AT_BLE_ATT_INVALID_ATTRIBUTE_VAL_LEN);
				} else {
					// All good
					value = convert_byte_array_to_16_bit(wr->value);
					if (0 == value) {
						// Beacon has to be disabled.
					} else if (value < EDDYSTONE_BEACON_INTERVAL_MS_MIN) {
						convert_16_bit_to_byte_array(EDDYSTONE_BEACON_INTERVAL_MS_MIN, wr->value);
					} else if (value > EDDYSTONE_BEACON_INTERVAL_MS_MAX) {
						convert_16_bit_to_byte_array(EDDYSTONE_BEACON_INTERVAL_MS_MAX, wr->value);
					}
					memcpy(esurl_data.beacon_period, wr->value, wr->length);
					status = at_ble_write_authorize_reply(params, AT_BLE_SUCCESS);
			
					// Rewrite characteristic with validated beacon period.
					//status = at_ble_characteristic_value_set(esurl_conf_service.chars[CHAR_BEACON_PERIOD].char_val_handle, (uint8_t *) esurl_data.beacon_period, 0x02);
				}
			}
	
			// Reset
			else if (wr->char_handle == esurl_conf_service.chars[CHAR_RESET].char_val_handle) {
				if (wr->length != 0x01) {
					// Invalid length
					status = at_ble_write_authorize_reply(params, AT_BLE_ATT_INVALID_ATTRIBUTE_VAL_LEN);
				} else {
					// All good
					status = at_ble_write_authorize_reply(params, AT_BLE_SUCCESS);
					if (wr->value[0] != 0) {
						// Issue the reset
						esurl_conf_chars_reset();
					}
				}
			} 
			else {
				status = at_ble_write_authorize_reply(params, AT_BLE_ATT_WRITE_NOT_PERMITTED);
			}
		}
#if defined(ES_URL_CONF_ENABLE_LOCK) && (ES_URL_CONF_ENABLE_LOCK == 1)
	}
#endif
	return status;
}

