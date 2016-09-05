/**
 * \file
 *
 * \brief OTAU GATT Server Service
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
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel micro controller product.
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

/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel
 * Support</a>
 */

/************************************************************************/
/*							Includes									*/
/************************************************************************/
#include "ble_manager.h"
#include "otau_service.h"

/************************************************************************/
/*							Globals										*/
/************************************************************************/
/** Initial OTAU characteristics default value */
static const uint16_t otau_char_default_val = OTAU_CHARACTERISTICS_DEFAULT_VALUE;



/************************************************************************/
/*							Implementation								*/
/************************************************************************/

/**@brief its an utility function to reverse the memory content of given length
 *
 * @param[in] buf pointer to memory location
 * @param[in] len length of the memory needs to be reversed
 *
 * @return at_ble_status_t AT_BLE_SUCCESS or AT_BLE_FAILURE
 */
void reverse_byte_buffer(uint8_t *buf, uint16_t len)
{
	uint16_t mid_pos = len/2;
	uint16_t idx;
	uint16_t tail = len-1;
	uint8_t data;
	
	for(idx = 0; idx < mid_pos; idx++)
	{
		data = buf[idx];
		buf[idx] = buf[tail];
		buf[tail--] = data;	 
	}
}

/**@brief OTAU GATT service and characteristic initialization to default values
 *
 * @param[in] OTAU service instance pointer
 *
 * @return at_ble_status_t AT_BLE_SUCCESS or AT_BLE_FAILURE
 */
at_ble_status_t otau_service_get_default(otau_gatt_service_handler_t *otau_service)
{
	if (otau_service == NULL)
	{
		return AT_BLE_FAILURE;
	}
	
	/* OTAU is handled in single connection, It good to keep the track of connection handle for OTAU */
	otau_service->conn_hanle = DEFAULT_CONNECTION_HANDLE;
	
	/* ------------------------ OTAU Service Related definition ------------------------------------- */
	/** Set default OTAU service handle. Its automatically set by stack, so its desired to set 0 */
	otau_service->service_handle = DEFAULT_OTAU_SERVICE_HANDLE;
	/** OTAU service UUID type */
	otau_service->service_uuid.type = AT_BLE_UUID_128;	
	/* set OTAU service UUID */
	memcpy(otau_service->service_uuid.uuid, OTAU_SERVICE_UUID, AT_BLE_UUID_128_LEN);
	reverse_byte_buffer(otau_service->service_uuid.uuid, AT_BLE_UUID_128_LEN);
	
	/* ------------------------ OTAU Endpoint-1 Characteristics Related definition ------------------------------------- */
	/**< The stack will store the char handle. so value handle is set to 0 */
	otau_service->chars[OTAU_INDICATION_CHAR_IDX].char_val_handle = DEFAULT_OTAU_CHAR_HANDLE;
	/** OTAU Characteristics UUID type */
	otau_service->chars[OTAU_INDICATION_CHAR_IDX].uuid.type = AT_BLE_UUID_128;	
	/* set characteristics UUID of OTAU */
	memcpy(otau_service->chars[OTAU_INDICATION_CHAR_IDX].uuid.uuid, OTAU_CHARACTERISTICS_ENDPOINT1_UUID, AT_BLE_UUID_128_LEN);
	reverse_byte_buffer(otau_service->chars[OTAU_INDICATION_CHAR_IDX].uuid.uuid, AT_BLE_UUID_128_LEN);
	
	/* set the properties of OTAU characteristics */
	otau_service->chars[OTAU_INDICATION_CHAR_IDX].properties = AT_BLE_CHAR_INDICATE;
	
	/**< initial value of this characteristic  */
	otau_service->chars[OTAU_INDICATION_CHAR_IDX].init_value = (uint8_t *)&otau_char_default_val;
	 /**< initial value length */
	otau_service->chars[OTAU_INDICATION_CHAR_IDX].value_init_len = sizeof(otau_char_default_val);
	/**< maximum possible length of the char. value */
	otau_service->chars[OTAU_INDICATION_CHAR_IDX].value_max_len = OTAU_CHARACTERISTICS_ENDPOINT1_MAX_SIZE;	
	 /**< Value permissions */
	otau_service->chars[OTAU_INDICATION_CHAR_IDX].value_permissions = AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR|AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR;;
	
	/**< no user description is desired then set to NULL */
	otau_service->chars[OTAU_INDICATION_CHAR_IDX].user_desc = NULL;
	/** no user description, so it is desired to set to 0 */
	otau_service->chars[OTAU_INDICATION_CHAR_IDX].user_desc_len = 0;
	/** no user description, so it is desired to set to 0 */
	otau_service->chars[OTAU_INDICATION_CHAR_IDX].user_desc_max_len = 0;
	/** no user description, so it is desired to set user description handle to 0 */
	otau_service->chars[OTAU_INDICATION_CHAR_IDX].user_desc_handle = 0;
	/* No user description, so its desired to set no permission */
	otau_service->chars[OTAU_INDICATION_CHAR_IDX].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	
	/* no presentation format, so it is necessary set to NULL */
	otau_service->chars[OTAU_INDICATION_CHAR_IDX].presentation_format = NULL;	
	
	/* Set the Maximum security be default to Client configuration permission */
	otau_service->chars[OTAU_INDICATION_CHAR_IDX].client_config_permissions = AT_BLE_ATTR_WRITABLE_REQ_AUTHN_REQ_AUTHR;
	/** client config handle is set by stack, so it is desired to set client config handle to 0 */
	otau_service->chars[OTAU_INDICATION_CHAR_IDX].client_config_handle = 0;
	
	/* Server configuration not allowed, so its desired to set no permission */
	otau_service->chars[OTAU_INDICATION_CHAR_IDX].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/** server config handle is set by stack, so it is desired to set server config handle to 0 */
	otau_service->chars[OTAU_INDICATION_CHAR_IDX].server_config_handle = 0;
	
	/* ------------------------ OTAU Endpoint-2 Characteristics Related definition ------------------------------------- */
	/**< The stack will store the char handle. so value handle is set to 0 */
	otau_service->chars[OTAU_WRITE_CHAR_IDX].char_val_handle = DEFAULT_OTAU_CHAR_HANDLE;
	/** OTAU Characteristics UUID type */
	otau_service->chars[OTAU_WRITE_CHAR_IDX].uuid.type = AT_BLE_UUID_128;
	/* set characteristics UUID of OTAU */
	memcpy(otau_service->chars[OTAU_WRITE_CHAR_IDX].uuid.uuid, OTAU_CHARACTERISTICS_ENDPOINT2_UUID, AT_BLE_UUID_128_LEN);
	reverse_byte_buffer(otau_service->chars[OTAU_WRITE_CHAR_IDX].uuid.uuid, AT_BLE_UUID_128_LEN);
	
	/* set the properties of OTAU characteristics */
	otau_service->chars[OTAU_WRITE_CHAR_IDX].properties = AT_BLE_CHAR_WRITE;
	
	/**< initial value of this characteristic  */
	otau_service->chars[OTAU_WRITE_CHAR_IDX].init_value = (uint8_t *)&otau_char_default_val;
	/**< initial value length */
	otau_service->chars[OTAU_WRITE_CHAR_IDX].value_init_len = sizeof(otau_char_default_val);
	/**< maximum possible length of the char. value */
	otau_service->chars[OTAU_WRITE_CHAR_IDX].value_max_len = OTAU_CHARACTERISTICS_ENDPOINT2_MAX_SIZE;
	/**< Value permissions */
	otau_service->chars[OTAU_WRITE_CHAR_IDX].value_permissions = AT_BLE_ATTR_WRITABLE_REQ_AUTHN_REQ_AUTHR;
	
	/**< no user description is desired then set to NULL */
	otau_service->chars[OTAU_WRITE_CHAR_IDX].user_desc = NULL;
	/** no user description, so it is desired to set to 0 */
	otau_service->chars[OTAU_WRITE_CHAR_IDX].user_desc_len = 0;
	/** no user description, so it is desired to set to 0 */
	otau_service->chars[OTAU_WRITE_CHAR_IDX].user_desc_max_len = 0;
	/** no user description, so it is desired to set user description handle to 0 */
	otau_service->chars[OTAU_WRITE_CHAR_IDX].user_desc_handle = 0;
	/* No user description, so its desired to set no permission */
	otau_service->chars[OTAU_WRITE_CHAR_IDX].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	
	/* no presentation format, so it is necessary set to NULL */
	otau_service->chars[OTAU_WRITE_CHAR_IDX].presentation_format = NULL;
	
	/* Set the Maximum security be default to Client configuration permission */
	otau_service->chars[OTAU_WRITE_CHAR_IDX].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/** client config handle is set by stack, so it is desired to set client config handle to 0 */
	otau_service->chars[OTAU_WRITE_CHAR_IDX].client_config_handle = 0;
	
	/* Server configuration not allowed, so its desired to set no permission */
	otau_service->chars[OTAU_WRITE_CHAR_IDX].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/** server config handle is set by stack, so it is desired to set server config handle to 0 */
	otau_service->chars[OTAU_WRITE_CHAR_IDX].server_config_handle = 0;
	
	/* ------------------------ OTAU Endpoint-3 Characteristics Related definition ------------------------------------- */
	/**< The stack will store the char handle. so value handle is set to 0 */
	otau_service->chars[OTAU_READ_CHAR_IDX].char_val_handle = DEFAULT_OTAU_CHAR_HANDLE;
	/** OTAU Characteristics UUID type */
	otau_service->chars[OTAU_READ_CHAR_IDX].uuid.type = AT_BLE_UUID_128;
	/* set characteristics UUID of OTAU */
	memcpy(otau_service->chars[OTAU_READ_CHAR_IDX].uuid.uuid, OTAU_CHARACTERISTICS_ENDPOINT3_UUID, AT_BLE_UUID_128_LEN);
	reverse_byte_buffer(otau_service->chars[OTAU_READ_CHAR_IDX].uuid.uuid, AT_BLE_UUID_128_LEN);
	
	/* set the properties of OTAU characteristics */
	otau_service->chars[OTAU_READ_CHAR_IDX].properties = AT_BLE_CHAR_READ;
	
	/**< initial value of this characteristic  */
	otau_service->chars[OTAU_READ_CHAR_IDX].init_value = (uint8_t *)&otau_char_default_val;
	/**< initial value length */
	otau_service->chars[OTAU_READ_CHAR_IDX].value_init_len = sizeof(otau_char_default_val);
	/**< maximum possible length of the char. value */
	otau_service->chars[OTAU_READ_CHAR_IDX].value_max_len = OTAU_CHARACTERISTICS_ENDPOINT3_MAX_SIZE;
	/**< Value permissions */
	otau_service->chars[OTAU_READ_CHAR_IDX].value_permissions = AT_BLE_ATTR_READABLE_REQ_AUTHN_REQ_AUTHR;
	
	/**< no user description is desired then set to NULL */
	otau_service->chars[OTAU_READ_CHAR_IDX].user_desc = NULL;
	/** no user description, so it is desired to set to 0 */
	otau_service->chars[OTAU_READ_CHAR_IDX].user_desc_len = 0;
	/** no user description, so it is desired to set to 0 */
	otau_service->chars[OTAU_READ_CHAR_IDX].user_desc_max_len = 0;
	/** no user description, so it is desired to set user description handle to 0 */
	otau_service->chars[OTAU_READ_CHAR_IDX].user_desc_handle = 0;
	/* No user description, so its desired to set no permission */
	otau_service->chars[OTAU_READ_CHAR_IDX].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	
	/* no presentation format, so it is necessary set to NULL */
	otau_service->chars[OTAU_READ_CHAR_IDX].presentation_format = NULL;
	
	/* Set the Maximum security be default to Client configuration permission */
	otau_service->chars[OTAU_READ_CHAR_IDX].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/** client config handle is set by stack, so it is desired to set client config handle to 0 */
	otau_service->chars[OTAU_READ_CHAR_IDX].client_config_handle = 0;
	
	/* Server configuration not allowed, so its desired to set no permission */
	otau_service->chars[OTAU_READ_CHAR_IDX].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/** server config handle is set by stack, so it is desired to set server config handle to 0 */
	otau_service->chars[OTAU_READ_CHAR_IDX].server_config_handle = 0;
	
	return AT_BLE_SUCCESS;
}

/**@brief Register a OTAU service instance into database.
 *
 * @param[in] otau_service_instance OTAU service instance
 * @param[in] service_type  primary service or secondary service
 *
 * @return @ref AT_BLE_SUCCESS operation completed successfully
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t otau_gatt_service_define(otau_gatt_service_handler_t *otau_service_instance, 
											at_ble_service_type_t service_type)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	if (service_type == PRIMARY_SERVICE)
	{
		status = at_ble_primary_service_define(&otau_service_instance->service_uuid,
											&otau_service_instance->service_handle,
											OTAU_INCLUDED_SERVICE_LIST,
											OTAU_INCLUDED_SERVICE_COUNT,
											otau_service_instance->chars,
											OTAU_MAX_CHARACTERISTICS);
	} 
	else if(service_type == SECONDARY_SERVICE)
	{
		status = at_ble_secondary_service_define(&otau_service_instance->service_uuid,
					&otau_service_instance->service_handle,
					OTAU_INCLUDED_SERVICE_LIST,
					OTAU_INCLUDED_SERVICE_COUNT,
					otau_service_instance->chars,
					OTAU_MAX_CHARACTERISTICS);
	}
	
	return status;
}

/**@brief otau_characteritics_changed_handler for characteristic CCCD write
 *
 * @param[in] otau_service_instance of type otau_gatt_service_handler_t
 * @param[in] params of type at_ble_characteristic_changed_t
 *
 * @return number representing the changed characteristic in case of failure CCCD_WRITE_UNUSED
 *			CCCD_WRITE_FAILED will be returned
 */
uint16_t otau_characteritics_changed_handler(otau_gatt_service_handler_t *otau_service_instance,
		at_ble_characteristic_changed_t *params)
{
	uint16_t cccd_value;
	
	if (((params->conn_handle == otau_service_instance->conn_hanle) || 
		(otau_service_instance->conn_hanle == DEFAULT_CONNECTION_HANDLE)) &&
		(params->char_handle == otau_service_instance->chars[OTAU_INDICATION_CHAR_IDX].client_config_handle))
	{
		if (otau_service_instance->conn_hanle == DEFAULT_CONNECTION_HANDLE)
		{
			/* Update the connection handle, based on the OTAU indication enable */
			otau_service_instance->conn_hanle = params->conn_handle;
		}
		
		if (params->status == AT_BLE_SUCCESS)
		{
			if ((params->char_len == CCCD_VALUE_LENGTH) && (params->char_offset == CCCD_VALUE_OFFSET))
			{
				
				cccd_value = ((params->char_new_value[CCCD_VALUE_OFFSET+1] << 8) | params->char_new_value[CCCD_VALUE_OFFSET]);
				if ((cccd_value == CCCD_NOTIFICATION_ENABLED) ||
				    (cccd_value == CCCD_INDICATION_ENABLED) ||
					(cccd_value == CCCD_NOTIFICATION_INDICATION_ENABLED) ||
					(cccd_value == CCCD_NOTIFICATION_INDICATION_DISABLED))					
				{
					return cccd_value;
				}
			}
		}
		
		DBG_LOG("CCCD Write failed");
		return CCCD_WRITE_FAILED;
	}
	
	return CCCD_WRITE_UNUSED;
}

