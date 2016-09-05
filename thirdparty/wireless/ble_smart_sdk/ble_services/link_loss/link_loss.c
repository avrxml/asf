/**
 * \file
 *
 * \brief Linkloss service
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
 *    Atmel microcontroller product.
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

 /**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the Device Information Service
 */
/****************************************************************************************
*							        Includes	                                     	*
****************************************************************************************/

#include "link_loss.h"

#if defined LLS_GATT_SERVER
/****************************************************************************************
*							        Globals	                                     		*
****************************************************************************************/

uint8_t linkloss_initial_value = 0;
static const char* lls_gs_str[] = {"NO_ALERT", "MILD_ALERT", "HIGH_ALERT"};

/****************************************************************************************
*							        Implementations	                                    *
****************************************************************************************/



/** @brief Linkloss service initialization
  * 
  */
void init_linkloss_service(gatt_service_handler_t *linkloss_serv)
{
	 
	 linkloss_serv->serv_handle = 0;
	 linkloss_serv->serv_uuid.type = AT_BLE_UUID_16;
	
	 linkloss_serv->serv_uuid.uuid[0] = (uint8_t) LINK_LOSS_SERVICE_UUID	; 
	 linkloss_serv->serv_uuid.uuid[1] = (uint8_t) (LINK_LOSS_SERVICE_UUID >> 8);
	 
	 //Characteristic Info for Alert Value
	 
	 /* handle stored here */
	 linkloss_serv->serv_chars.char_val_handle = 0;
	           
	 /*16bit UUID : Alert Level */
	 linkloss_serv->serv_chars.uuid.type = AT_BLE_UUID_16;
	
	 linkloss_serv->serv_chars.uuid.uuid[0] = (uint8_t)  ALERT_LEVEL_CHAR_UUID;
	 linkloss_serv->serv_chars.uuid.uuid[1] = (uint8_t) (ALERT_LEVEL_CHAR_UUID >> 8);
	
	 /* Properties */
	 linkloss_serv->serv_chars.properties = AT_BLE_CHAR_READ | AT_BLE_CHAR_WRITE; 
	
	 /* value */
	 linkloss_serv->serv_chars.init_value = &linkloss_initial_value ; 
	 linkloss_serv->serv_chars.value_init_len = sizeof(int8_t);
	 linkloss_serv->serv_chars.value_max_len = sizeof(int8_t);
	
	 /* permissions */
	 #if BLE_PAIR_ENABLE
		linkloss_serv->serv_chars.value_permissions = (AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR
														 | AT_BLE_ATTR_WRITABLE_REQ_AUTHN_NO_AUTHR);   
	 #else
		linkloss_serv->serv_chars.value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR
														| AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);		
	 #endif
	 
	 /* user defined name */
	 linkloss_serv->serv_chars.user_desc = NULL;           
	 linkloss_serv->serv_chars.user_desc_len = 0;
	 linkloss_serv->serv_chars.user_desc_max_len = 0;
	
	 /*user description permissions*/
	 linkloss_serv->serv_chars.user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;             
	
	 /*client config permissions*/
	 linkloss_serv->serv_chars.client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         
	
	  /*server config permissions*/
	 linkloss_serv->serv_chars.server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;        
	
	  /*user desc handles*/
	 linkloss_serv->serv_chars.user_desc_handle = 0;            
	
	  /*client config handles*/
	 linkloss_serv->serv_chars.client_config_handle = 0;        
	
	 /*server config handles*/
	 linkloss_serv->serv_chars.server_config_handle = 0;         
	
	 /* presentation format */
	 linkloss_serv->serv_chars.presentation_format = NULL;       
	
}
 

/** @brief linkloss service definition
  * 
  */
at_ble_status_t lls_primary_service_define(gatt_service_handler_t *lls_service)
{
	
	return(at_ble_primary_service_define(&lls_service->serv_uuid,
											&lls_service->serv_handle,
											NULL, LLS_INCLUDED_SERVICE_COUNT,  
											&lls_service->serv_chars, LLS_CHARACTERISTIC_COUNT));
}


/** @brief Setting the alert value of linkloss service
  * 
  */
uint8_t lls_set_alert_value(at_ble_characteristic_changed_t *change_params, gatt_service_handler_t *lls_handler)
{
	uint8_t ret_value = INVALID_LLS_PARAM;
	
	if (change_params->char_handle == lls_handler->serv_chars.char_val_handle) {
		if (change_params->char_new_value[0] <= LLS_HIGH_ALERT) {
			DBG_LOG("The current alert level for linkloss is %s",
					lls_gs_str[change_params->char_new_value[0]]);
			ret_value = change_params->char_new_value[0];
		} else {
			DBG_LOG("These value are reserved for future, 0 - 2 is expected range but got %x",
					change_params->char_new_value[0]);
		}
	} 
	return ret_value;
}

#endif //LLS_GATT_SERVER

#if defined LLS_GATT_CLIENT
/**@brief write a characteristic  to Link Loss Characteristics
 *
 * if with_response is True, write completion will be reported via @ref
 *AT_BLE_CHARACTERISTIC_WRITE_RESPONSE event
 *
 * @param[in] conn_handle handle of the connection
 * @param[in] char_handle handle of the characteristic
 * @param[in] alert level need to write
 *
 * @return @ref AT_BLE_SUCCESS operation completed successfully
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t lls_alert_level_write(at_ble_handle_t conn_handle,
		at_ble_handle_t char_handle,
		link_loss_alert_level_t alert_level)
{
	/* for link loss alert write with response */
	return (at_ble_characteristic_write(conn_handle,
	       char_handle,
	       LLS_WRITE_OFFSET,
	       LLS_WRITE_LENGTH,
	       (uint8_t *)&alert_level,
	       LLS_NO_SIGNED_WRITE,
	       LLS_WRITE_WITH_RESPONSE));
}

/**@brief Send the Read request to link loss handler
 * Read value will be reported via @ref AT_BLE_CHARACTERISTIC_READ_RESPONSE
 *event
 *
 * @param[in] conn_handle handle of the connection
 * @param[in] char_handle handle of the characteristic
 * @return @ref AT_BLE_SUCCESS operation completed successfully
 * @return @ref AT_BLE_INVALID_PARAM Invalid arguments.
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t lls_alert_level_read(at_ble_handle_t conn_handle,
		at_ble_handle_t char_handle)
{
	if (char_handle == LLS_INVALID_CHAR_HANDLE) {
		return (AT_BLE_ATT_INVALID_HANDLE);
	} else {
		return (at_ble_characteristic_read(conn_handle,
		       char_handle,
		       LLS_READ_OFFSET,
		       LLS_READ_LENGTH));
	}
}

/**@brief Read Link Loss Alert Value
 *
 * @param[in] read_value read response data available form
 *at_ble_characteristic_read_response_t
 * @return Link Loss Alert level .
 * @return LLS_READ_RESP_INVALID if value are other than alert levels
 */
int8_t lls_alert_read_response(at_ble_characteristic_read_response_t *read_resp,
		gatt_lls_char_handler_t *lls_handler)
{
	if (read_resp->char_handle == lls_handler->char_handle) {
		memcpy(lls_handler->char_data,
				&read_resp->char_value[LLS_READ_OFFSET],
				LLS_READ_LENGTH);

		DBG_LOG("Link Loss Alert level set to");

		switch (read_resp->char_value[LLS_READ_OFFSET]) {
		case LLS_NO_ALERT:
		{
			DBG_LOG_CONT(" NO ALERT ");
		}
		break;

		case LLS_MILD_ALERT:
		{
			DBG_LOG_CONT(" MILD ALERT ");
		}
		break;

		case LLS_HIGH_ALERT:
		{
			DBG_LOG_CONT(" HIGH ALERT ");
		}
		break;
		}
		return (read_resp->char_value[LLS_READ_OFFSET]);
	}

	return(LLS_READ_RESP_INVALID);
}
#endif //LLS_GATT_CLIENT
