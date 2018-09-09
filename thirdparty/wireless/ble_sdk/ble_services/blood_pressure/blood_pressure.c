/**
 * \file
 *
 * \brief Blood Pressure Service
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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Atmel
 * Support</a>
 */

/************************************************************************/
/*							Includes									*/
/************************************************************************/
#include "ble_manager.h"
#include "blood_pressure.h"

/************************************************************************/
/*							Globals										 */
/************************************************************************/
/** initial heart rate measurement value */
uint16_t blp_measurement_value = DEFAULT_VALUE;

/** initial body sensor location value */
uint8_t intermediate_cuff_pressure_value = DEFAULT_VALUE;

/** initial blood pressure feature value */
uint16_t blood_pressure_feature_value = 0x001f;



/************************************************************************/
/*							Implementation								*/
/************************************************************************/

/**@brief Blood Pressure service and characteristic initialization(Called only once
 * by user).
 *
 * @param[in] Blood Pressure service instance
 *
 * @return none
 */
void blp_init_service(blp_gatt_service_handler_t *blood_pressure_serv)
{
	blood_pressure_serv->serv_handle = 0;
	blood_pressure_serv->serv_uuid.type = AT_BLE_UUID_16;
	blood_pressure_serv->serv_uuid.uuid[0] = (uint8_t)BLOOD_PRESSURE_SERVICE_UUID;
	blood_pressure_serv->serv_uuid.uuid[1]
		= (uint8_t)(BLOOD_PRESSURE_SERVICE_UUID >> 8);

	/*Characteristic Info for Blood Pressure Measurement*/

	/* handle stored here */
	blood_pressure_serv->serv_chars[0].char_val_handle = 0;
	blood_pressure_serv->serv_chars[0].uuid.type = AT_BLE_UUID_16;

	/* UUID : Blood Pressure Measurement Characteristic */
	blood_pressure_serv->serv_chars[0].uuid.uuid[0]
		= (uint8_t)BLOOD_PRESSURE_MEASUREMENT_CHAR_UUID;
	blood_pressure_serv->serv_chars[0].uuid.uuid[1]
		= (uint8_t)(BLOOD_PRESSURE_MEASUREMENT_CHAR_UUID >> 8);

	/* Properties */
	blood_pressure_serv->serv_chars[0].properties = AT_BLE_CHAR_INDICATE;

	blood_pressure_serv->serv_chars[0].init_value
		= (uint8_t *)&blp_measurement_value;
	blood_pressure_serv->serv_chars[0].value_init_len = sizeof(uint16_t);

	blood_pressure_serv->serv_chars[0].value_max_len = BLP_MM_FLAGS_SIZE +
		BLP_SYSTOLIC_VAL_SIZE + BLP_DIASTOLIC_VAL_SIZE + BLP_MAP_VAL_SIZE+
		BLP_MAX_TIME_STAMP_SIZE + BLP_MAX_PULSE_RATE_SIZE + BLP_MAX_USER_ID_SIZE+
		BLP_MAX_MM_STATUS_SIZE;
		
	/* Permissions for characteristics */
	blood_pressure_serv->serv_chars[0].value_permissions
											= AT_BLE_ATTR_NO_PERMISSIONS;
	/* user defined */
	blood_pressure_serv->serv_chars[0].user_desc = NULL;
	blood_pressure_serv->serv_chars[0].user_desc_len = 0;
	blood_pressure_serv->serv_chars[0].user_desc_max_len = 0;
	/*user description permissions*/
	blood_pressure_serv->serv_chars[0].user_desc_permissions
		= AT_BLE_ATTR_NO_PERMISSIONS;
	/*client config permissions*/
	#if BLE_PAIR_ENABLE
			blood_pressure_serv->serv_chars[0].client_config_permissions
			=	( AT_BLE_ATTR_WRITABLE_REQ_AUTHN_NO_AUTHR);
	#else 
			blood_pressure_serv->serv_chars[0].client_config_permissions
			= AT_BLE_ATTR_NO_PERMISSIONS;
	#endif 

	/*server config permissions*/
	blood_pressure_serv->serv_chars[0].server_config_permissions
		= AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	blood_pressure_serv->serv_chars[0].user_desc_handle = 0;
	/*client config handles*/
	blood_pressure_serv->serv_chars[0].client_config_handle = 0;
	/*server config handles*/
	blood_pressure_serv->serv_chars[0].server_config_handle = 0;
	/* presentation format */
	blood_pressure_serv->serv_chars[0].presentation_format = NULL;

	/* Characteristic Info for Intermediate Cuff Pressure */
	blood_pressure_serv->serv_chars[1].char_val_handle = 0; 
	         /* handle stored here */
	blood_pressure_serv->serv_chars[1].uuid.type = AT_BLE_UUID_16;
	/* UUID : Intermediate Cuff Pressure*/
	blood_pressure_serv->serv_chars[1].uuid.uuid[0]
		= (uint8_t)INTERMEDIATE_CUFF_PRESSURE_CHAR_UUID;
	/* UUID : Body Sensor location*/
	blood_pressure_serv->serv_chars[1].uuid.uuid[1]
		= (uint8_t)(INTERMEDIATE_CUFF_PRESSURE_CHAR_UUID >> 8);
	/* Properties */
	blood_pressure_serv->serv_chars[1].properties = AT_BLE_CHAR_NOTIFY;

	blood_pressure_serv->serv_chars[1].init_value = &intermediate_cuff_pressure_value;

	blood_pressure_serv->serv_chars[1].value_init_len = sizeof(uint8_t);
	blood_pressure_serv->serv_chars[1].value_max_len = BLP_MM_FLAGS_SIZE +
	BLP_SYSTOLIC_VAL_SIZE + BLP_DIASTOLIC_VAL_SIZE + BLP_MAP_VAL_SIZE+
	BLP_MAX_TIME_STAMP_SIZE + BLP_MAX_PULSE_RATE_SIZE + BLP_MAX_USER_ID_SIZE+
	BLP_MAX_MM_STATUS_SIZE;

		/* permissions */
		blood_pressure_serv->serv_chars[1].value_permissions
							= AT_BLE_ATTR_NO_PERMISSIONS;
	
	/* user defined name */
	blood_pressure_serv->serv_chars[1].user_desc = NULL;
	blood_pressure_serv->serv_chars[1].user_desc_len = 0;
	blood_pressure_serv->serv_chars[1].user_desc_max_len = 0;
	/*user description permissions*/
	blood_pressure_serv->serv_chars[1].user_desc_permissions
		= AT_BLE_ATTR_NO_PERMISSIONS;
		
	/*client config permissions*/
	#if BLE_PAIR_ENABLE
	blood_pressure_serv->serv_chars[1].client_config_permissions
									= (AT_BLE_ATTR_WRITABLE_REQ_AUTHN_NO_AUTHR);
	#else
	blood_pressure_serv->serv_chars[1].client_config_permissions
	= AT_BLE_ATTR_NO_PERMISSIONS;
	#endif

	/*server config permissions*/
	blood_pressure_serv->serv_chars[1].server_config_permissions
		= AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	blood_pressure_serv->serv_chars[1].user_desc_handle = 0;
	/*client config handles*/
	blood_pressure_serv->serv_chars[1].client_config_handle = 0;
	/*server config handles*/
	blood_pressure_serv->serv_chars[1].server_config_handle = 0;
	/* presentation format */
	blood_pressure_serv->serv_chars[1].presentation_format = NULL;

	/* Characteristic Info for blood Pressure feature Characteristic */
	/* handle stored here */
	blood_pressure_serv->serv_chars[2].char_val_handle = 0;
	blood_pressure_serv->serv_chars[2].uuid.type = AT_BLE_UUID_16;
	/* UUID : Heart Rate Control Point*/
	blood_pressure_serv->serv_chars[2].uuid.uuid[0]
		= (uint8_t)BLOOD_PRESSURE_FEATURE_CHAR_UUID;
	/* UUID : Heart Rate Control Point*/
	blood_pressure_serv->serv_chars[2].uuid.uuid[1]
		= (uint8_t)(BLOOD_PRESSURE_FEATURE_CHAR_UUID >> 8);
	/* Properties */
	blood_pressure_serv->serv_chars[2].properties = AT_BLE_CHAR_READ;

	/* Initial Value */
	blood_pressure_serv->serv_chars[2].init_value =(uint8_t *) &blood_pressure_feature_value;

	blood_pressure_serv->serv_chars[2].value_init_len = sizeof(uint16_t);
	blood_pressure_serv->serv_chars[2].value_max_len = sizeof(uint16_t);
	
	#if BLE_PAIR_ENABLE
			/* permissions */
			blood_pressure_serv->serv_chars[2].value_permissions
						= AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR;
	#else
			/* permissions */
			blood_pressure_serv->serv_chars[2].value_permissions
						= AT_BLE_ATTR_NO_PERMISSIONS;
	#endif

	/* user defined name */
	blood_pressure_serv->serv_chars[2].user_desc = NULL;
	blood_pressure_serv->serv_chars[2].user_desc_len = 0;
	blood_pressure_serv->serv_chars[2].user_desc_max_len = 0;
	/*user description permissions*/
	blood_pressure_serv->serv_chars[2].user_desc_permissions
		= AT_BLE_ATTR_NO_PERMISSIONS;
	/*client config permissions*/
	blood_pressure_serv->serv_chars[2].client_config_permissions
		= AT_BLE_ATTR_NO_PERMISSIONS;
	/*server config permissions*/
	blood_pressure_serv->serv_chars[2].server_config_permissions
		= AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	blood_pressure_serv->serv_chars[2].user_desc_handle = 0;
	/*client config handles*/
	blood_pressure_serv->serv_chars[2].client_config_handle = 0;
	/*server config handles*/
	blood_pressure_serv->serv_chars[2].server_config_handle = 0;
	/* presentation format */
	blood_pressure_serv->serv_chars[2].presentation_format = NULL;
}

/**@brief Register a BLP service instance inside stack.
 *
 * @param[in] blp_primary_service blp service instance
 *
 * @return @ref AT_BLE_SUCCESS operation completed successfully
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t blp_primary_service_define(
		blp_gatt_service_handler_t *blp_primary_service)
{
	return(at_ble_primary_service_define(&blp_primary_service->serv_uuid,
	       &blp_primary_service->serv_handle,
	       NULL, 0,
	       blp_primary_service->serv_chars, BLP_TOTAL_CHARATERISTIC_NUM));
}

/**@brief handler for characteristic write called by profile
 *
 * @param[in] blp_gatt_service_handler_t service handler
 *
 * @return number representing the changed characteristic
 */
uint8_t blp_char_change_handler(blp_gatt_service_handler_t *blp_primary_service,
		at_ble_characteristic_changed_t *params)
{
	if (params->char_handle ==
			blp_primary_service->serv_chars[0].client_config_handle) {
		if (params->char_new_value[0] == BLP_INDICATION) {
			return BLP_INDICATION_ENABLE;
		} else if (params->char_new_value[0] == false) {
			return BLP_INDICATION_DISABLE;
		} 
	}
	
	if (params->char_handle ==
	blp_primary_service->serv_chars[1].client_config_handle) {
		if (params->char_new_value[0] == BLP_NOTIFICATION) {
			return BLP_NOTIFICATION_ENABLE;
		} else if (params->char_new_value[0] == false) {
			return BLP_NOTIFICATION_DISABLE;
		}
	} 
	return 0xff;
}

