/**
 * \file
 *
 * \brief Device Information Service
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
 *Support</a>
 */

/**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the Device Information service
 */
/*- Includes ---------------------------------------------------------------*/

#include "device_info.h"

/** characteristics of the device information service */
device_info_char_value_t char_value;

bool volatile dis_notification_flag[DIS_TOTAL_CHARATERISTIC_NUM] = {false};

/**@brief Initialize the dis service related information. */
void dis_init_service(dis_gatt_service_handler_t *device_info_serv)
{
	device_info_serv->serv_handle = 0;
	device_info_serv->serv_uuid.type = AT_BLE_UUID_16;
	device_info_serv->serv_uuid.uuid[0] = (uint8_t) DIS_SERVICE_UUID;
	device_info_serv->serv_uuid.uuid[1] = (uint8_t) (DIS_SERVICE_UUID >> 8);
	
	//Characteristic Info for Manufacturer Name String
	device_info_serv->serv_chars[0].char_val_handle = 0;          /* handle stored here */
	device_info_serv->serv_chars[0].uuid.type = AT_BLE_UUID_16;
	device_info_serv->serv_chars[0].uuid.uuid[0] = (uint8_t) DIS_CHAR_MANUFACTURER_NAME_UUID;          /* UUID : Manufacturer Name String */
	device_info_serv->serv_chars[0].uuid.uuid[1] = (uint8_t) (DIS_CHAR_MANUFACTURER_NAME_UUID >> 8);   /* UUID : Manufacturer Name String */
	device_info_serv->serv_chars[0].properties = AT_BLE_CHAR_READ; /* Properties */
	
	memcpy(char_value.manufacturer_name,DEFAULT_MANUFACTURER_NAME,DIS_CHAR_MANUFACTURER_NAME_INIT_LEN);
	device_info_serv->serv_chars[0].init_value = char_value.manufacturer_name;
	
	device_info_serv->serv_chars[0].value_init_len = DIS_CHAR_MANUFACTURER_NAME_INIT_LEN;
	device_info_serv->serv_chars[0].value_max_len = DIS_CHAR_MANUFACTURER_NAME_MAX_LEN;
#if BLE_PAIR_ENABLE
	device_info_serv->serv_chars[0].value_permissions = AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR;   /* permissions */
#else
	device_info_serv->serv_chars[0].value_permissions = AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR;   /* permissions */
#endif
	device_info_serv->serv_chars[0].user_desc = NULL;           /* user defined name */
	device_info_serv->serv_chars[0].user_desc_len = 0;
	device_info_serv->serv_chars[0].user_desc_max_len = 0;
	device_info_serv->serv_chars[0].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;             /*user description permissions*/
	device_info_serv->serv_chars[0].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*client config permissions*/
	device_info_serv->serv_chars[0].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*server config permissions*/
	device_info_serv->serv_chars[0].user_desc_handle = 0;             /*user desc handles*/
	device_info_serv->serv_chars[0].client_config_handle = 0;         /*client config handles*/
	device_info_serv->serv_chars[0].server_config_handle = 0;         /*server config handles*/
	device_info_serv->serv_chars[0].presentation_format = NULL;       /* presentation format */
	
	
	//Characterisitc Info for Model Number String
	device_info_serv->serv_chars[1].char_val_handle = 0;          /* handle stored here */
	device_info_serv->serv_chars[1].uuid.type = AT_BLE_UUID_16;
	device_info_serv->serv_chars[1].uuid.uuid[0] = (uint8_t) DIS_CHAR_MODEL_NUMBER_UUID;          /* UUID : Serial Number String*/
	device_info_serv->serv_chars[1].uuid.uuid[1] = (uint8_t) (DIS_CHAR_MODEL_NUMBER_UUID >> 8);          /* UUID : Serial Number String*/
	device_info_serv->serv_chars[1].properties = AT_BLE_CHAR_READ; /* Properties */
	
	memcpy(char_value.default_model_number,DEFAULT_MODEL_NUMBER,DIS_CHAR_MODEL_NUMBER_INIT_LEN);
	device_info_serv->serv_chars[1].init_value = char_value.default_model_number;
	
	device_info_serv->serv_chars[1].value_init_len = DIS_CHAR_MODEL_NUMBER_INIT_LEN;
	device_info_serv->serv_chars[1].value_max_len = DIS_CHAR_MODEL_NUMBER_MAX_LEN;
#if BLE_PAIR_ENABLE
	device_info_serv->serv_chars[1].value_permissions = AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR;   /* permissions */
#else
	device_info_serv->serv_chars[1].value_permissions = AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR;   /* permissions */
#endif
	device_info_serv->serv_chars[1].user_desc = NULL;           /* user defined name */
	device_info_serv->serv_chars[1].user_desc_len = 0;
	device_info_serv->serv_chars[1].user_desc_max_len = 0;
	device_info_serv->serv_chars[1].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;             /*user description permissions*/
	device_info_serv->serv_chars[1].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*client config permissions*/
	device_info_serv->serv_chars[1].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*server config permissions*/
	device_info_serv->serv_chars[1].user_desc_handle = 0;             /*user desc handles*/
	device_info_serv->serv_chars[1].client_config_handle = 0;         /*client config handles*/
	device_info_serv->serv_chars[1].server_config_handle = 0;         /*server config handles*/
	device_info_serv->serv_chars[1].presentation_format = NULL;       /* presentation format */


	//Characteristic Info for Serial String
	device_info_serv->serv_chars[2].char_val_handle = 0;          /* handle stored here */
	device_info_serv->serv_chars[2].uuid.type = AT_BLE_UUID_16;
	device_info_serv->serv_chars[2].uuid.uuid[0] = (uint8_t) DIS_CHAR_SERIAL_NUMBER_UUID;          /* UUID : Hardware Revision String*/
	device_info_serv->serv_chars[2].uuid.uuid[1] = (uint8_t) (DIS_CHAR_SERIAL_NUMBER_UUID >> 8);          /* UUID : Hardware Revision String*/
	device_info_serv->serv_chars[2].properties = AT_BLE_CHAR_READ; /* Properties */
	
	memcpy(char_value.default_serial_number,DEFAULT_SERIAL_NUMBER,DIS_CHAR_SERIAL_NUMBER_INIT_LEN);
	device_info_serv->serv_chars[2].init_value = char_value.default_serial_number;
	
	device_info_serv->serv_chars[2].value_init_len = DIS_CHAR_SERIAL_NUMBER_INIT_LEN;
	device_info_serv->serv_chars[2].value_max_len = DIS_CHAR_SERIAL_NUMBER_MAX_LEN;
#if BLE_PAIR_ENABLE
	device_info_serv->serv_chars[2].value_permissions = AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR;   /* permissions */
#else
	device_info_serv->serv_chars[2].value_permissions = AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR;   /* permissions */
#endif
	device_info_serv->serv_chars[2].user_desc = NULL;           /* user defined name */
	device_info_serv->serv_chars[2].user_desc_len = 0;
	device_info_serv->serv_chars[2].user_desc_max_len = 0;
	device_info_serv->serv_chars[2].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;             /*user description permissions*/
	device_info_serv->serv_chars[2].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*client config permissions*/
	device_info_serv->serv_chars[2].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*server config permissions*/
	device_info_serv->serv_chars[2].user_desc_handle = 0;             /*user desc handles*/
	device_info_serv->serv_chars[2].client_config_handle = 0;         /*client config handles*/
	device_info_serv->serv_chars[2].server_config_handle = 0;         /*server config handles*/
	device_info_serv->serv_chars[2].presentation_format = NULL;       /* presentation format */


	//Characteristic Info for Hardware Revision String
	device_info_serv->serv_chars[3].char_val_handle = 0;          /* handle stored here */
	device_info_serv->serv_chars[3].uuid.type = AT_BLE_UUID_16;
	device_info_serv->serv_chars[3].uuid.uuid[0] = (uint8_t) DIS_CHAR_HARDWARE_REVISION_UUID;          /* UUID : Firmware Revision String*/
	device_info_serv->serv_chars[3].uuid.uuid[1] = (uint8_t) (DIS_CHAR_HARDWARE_REVISION_UUID >> 8);          /* UUID : Firmware Revision String*/
	device_info_serv->serv_chars[3].properties = AT_BLE_CHAR_READ; /* Properties */
	
	memcpy(char_value.default_hardware_revision,DEFAULT_HARDWARE_REVISION,DIS_CHAR_HARDWARE_REVISION_INIT_LEN);
	device_info_serv->serv_chars[3].init_value = char_value.default_hardware_revision;
	
	device_info_serv->serv_chars[3].value_init_len = DIS_CHAR_HARDWARE_REVISION_INIT_LEN;
	device_info_serv->serv_chars[3].value_max_len = DIS_CHAR_HARDWARE_REVISION_MAX_LEN;
#if BLE_PAIR_ENABLE
	device_info_serv->serv_chars[3].value_permissions = AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR;   /* permissions */
#else
	device_info_serv->serv_chars[3].value_permissions = AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR;   /* permissions */
#endif
	device_info_serv->serv_chars[3].user_desc = NULL;           /* user defined name */
	device_info_serv->serv_chars[3].user_desc_len = 0;
	device_info_serv->serv_chars[3].user_desc_max_len = 0;
	device_info_serv->serv_chars[3].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;             /*user description permissions*/
	device_info_serv->serv_chars[3].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*client config permissions*/
	device_info_serv->serv_chars[3].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*server config permissions*/
	device_info_serv->serv_chars[3].user_desc_handle = 0;             /*user desc handles*/
	device_info_serv->serv_chars[3].client_config_handle = 0;         /*client config handles*/
	device_info_serv->serv_chars[3].server_config_handle = 0;         /*server config handles*/
	device_info_serv->serv_chars[3].presentation_format = NULL;       /* presentation format */
	
	
	//Characteristic Info for Firmware  Revision
	device_info_serv->serv_chars[4].char_val_handle = 0;          /* handle stored here */
	device_info_serv->serv_chars[4].uuid.type = AT_BLE_UUID_16;
	device_info_serv->serv_chars[4].uuid.uuid[0] = (uint8_t) DIS_CHAR_FIRMWARE_REIVSION_UUID;          /* UUID : Software Revision */
	device_info_serv->serv_chars[4].uuid.uuid[1] = (uint8_t) (DIS_CHAR_FIRMWARE_REIVSION_UUID >> 8);          /* UUID : Software Revision */
	device_info_serv->serv_chars[4].properties = AT_BLE_CHAR_READ; /* Properties */
	
	memcpy(char_value.default_firmware_revision,DEFAULT_FIRMWARE_REIVSION,DIS_CHAR_FIRMWARE_REIVSION_INIT_LEN);
	device_info_serv->serv_chars[4].init_value = char_value.default_firmware_revision;
	
	device_info_serv->serv_chars[4].value_init_len = DIS_CHAR_FIRMWARE_REIVSION_INIT_LEN;
	device_info_serv->serv_chars[4].value_max_len = DIS_CHAR_FIRMWARE_REIVSION_MAX_LEN;
#if BLE_PAIR_ENABLE
	device_info_serv->serv_chars[4].value_permissions = AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR;   /* permissions */
#else
	device_info_serv->serv_chars[4].value_permissions = AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR;   /* permissions */
#endif
	device_info_serv->serv_chars[4].user_desc = NULL;           /* user defined name */
	device_info_serv->serv_chars[4].user_desc_len = 0;
	device_info_serv->serv_chars[4].user_desc_max_len = 0;
	device_info_serv->serv_chars[4].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;             /*user description permissions*/
	device_info_serv->serv_chars[4].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*client config permissions*/
	device_info_serv->serv_chars[4].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*server config permissions*/
	device_info_serv->serv_chars[4].user_desc_handle = 0;             /*user desc handles*/
	device_info_serv->serv_chars[4].client_config_handle = 0;         /*client config handles*/
	device_info_serv->serv_chars[4].server_config_handle = 0;         /*server config handles*/
	device_info_serv->serv_chars[4].presentation_format = NULL;       /* presentation format */
	
	//Characteristic Info for Software  Revision
	device_info_serv->serv_chars[5].char_val_handle = 0;          /* handle stored here */
	device_info_serv->serv_chars[5].uuid.type = AT_BLE_UUID_16;
	device_info_serv->serv_chars[5].uuid.uuid[0] = (uint8_t) DIS_CHAR_SOFTWARE_REVISION_UUID;          /* uuid : software revision */
	device_info_serv->serv_chars[5].uuid.uuid[1] = (uint8_t) (DIS_CHAR_SOFTWARE_REVISION_UUID >> 8);          /* uuid : software revision */
	device_info_serv->serv_chars[5].properties = AT_BLE_CHAR_READ; /* properties */
	
	memcpy(char_value.default_software_revision,DEFAULT_SOFTWARE_REVISION,DIS_CHAR_SOFTWARE_REVISION_INIT_LEN);
	device_info_serv->serv_chars[5].init_value = char_value.default_software_revision;
	
	device_info_serv->serv_chars[5].value_init_len = DIS_CHAR_SOFTWARE_REVISION_INIT_LEN;
	device_info_serv->serv_chars[5].value_max_len = DIS_CHAR_SOFTWARE_REVISION_MAX_LEN;
#if BLE_PAIR_ENABLE
	device_info_serv->serv_chars[5].value_permissions = AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR;   /* permissions */
#else
	device_info_serv->serv_chars[5].value_permissions = AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR;   /* permissions */
#endif
	device_info_serv->serv_chars[5].user_desc = NULL;           /* user defined name */
	device_info_serv->serv_chars[5].user_desc_len = 0;
	device_info_serv->serv_chars[5].user_desc_max_len = 0;
	device_info_serv->serv_chars[5].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;             /*user description permissions*/
	device_info_serv->serv_chars[5].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*client config permissions*/
	device_info_serv->serv_chars[5].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*server config permissions*/
	device_info_serv->serv_chars[5].user_desc_handle = 0;             /*user desc handles*/
	device_info_serv->serv_chars[5].client_config_handle = 0;         /*client config handles*/
	device_info_serv->serv_chars[5].server_config_handle = 0;         /*server config handles*/
	device_info_serv->serv_chars[5].presentation_format = NULL;       /* presentation format */
	
	//Characteristic Info for SystemID  Number
	device_info_serv->serv_chars[6].char_val_handle = 0;          /* handle stored here */
	device_info_serv->serv_chars[6].uuid.type = AT_BLE_UUID_16;
	device_info_serv->serv_chars[6].uuid.uuid[0] = (uint8_t) DIS_CHAR_SYSTEM_ID_UUID;          /* UUID : Software Revision */
	device_info_serv->serv_chars[6].uuid.uuid[1] = (uint8_t) (DIS_CHAR_SYSTEM_ID_UUID >> 8);          /* UUID : Software Revision */
	device_info_serv->serv_chars[6].properties = AT_BLE_CHAR_READ; /* Properties */
	
	memcpy(char_value.default_system_id.manufacturer_id, SYSTEM_ID_MANUFACTURER_ID, SYSTEM_ID_MANUFACTURER_ID_LEN);
	memcpy(char_value.default_system_id.org_unique_id, SYSTEM_ID_ORG_UNIQUE_ID, SYSTEM_ID_ORG_UNIQUE_ID_LEN);
	device_info_serv->serv_chars[6].init_value = (uint8_t *) &char_value.default_system_id;					/*Initial Value*/
	
	device_info_serv->serv_chars[6].value_init_len = DIS_CHAR_SYSTEM_ID_INIT_LEN;
	device_info_serv->serv_chars[6].value_max_len = DIS_CHAR_SYSTEM_ID_MAX_LEN;
#if BLE_PAIR_ENABLE
	device_info_serv->serv_chars[6].value_permissions = AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR;   /* permissions */
#else
	device_info_serv->serv_chars[6].value_permissions = AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR;   /* permissions */
#endif
	device_info_serv->serv_chars[6].user_desc = NULL;           /* user defined name */
	device_info_serv->serv_chars[6].user_desc_len = 0;
	device_info_serv->serv_chars[6].user_desc_max_len = 0;
	device_info_serv->serv_chars[6].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;             /*user description permissions*/
	device_info_serv->serv_chars[6].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*client config permissions*/
	device_info_serv->serv_chars[6].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*server config permissions*/
	device_info_serv->serv_chars[6].user_desc_handle = 0;             /*user desc handles*/
	device_info_serv->serv_chars[6].client_config_handle = 0;         /*client config handles*/
	device_info_serv->serv_chars[6].server_config_handle = 0;         /*server config handles*/
	device_info_serv->serv_chars[6].presentation_format = NULL;       /* presentation format */
	
	//Characteristic Info for PnP ID 
	device_info_serv->serv_chars[7].char_val_handle = 0;          /* handle stored here */
	device_info_serv->serv_chars[7].uuid.type = AT_BLE_UUID_16;
	device_info_serv->serv_chars[7].uuid.uuid[0] = (uint8_t) DIS_CHAR_PNP_ID_UUID;          /* UUID : Software Revision */
	device_info_serv->serv_chars[7].uuid.uuid[1] = (uint8_t) (DIS_CHAR_PNP_ID_UUID >> 8);          /* UUID : Software Revision */
	device_info_serv->serv_chars[7].properties = AT_BLE_CHAR_READ; /* Properties */
	
	char_value.default_pnp_id.vendor_id_source = PNP_ID_VENDOR_ID_SOURCE;					/*characteristic value initialization */
	char_value.default_pnp_id.vendor_id = PNP_ID_VENDOR_ID;
	char_value.default_pnp_id.product_id= PNP_ID_PRODUCT_ID;
	char_value.default_pnp_id.product_version= PNP_ID_PRODUCT_VERSION;
	device_info_serv->serv_chars[7].init_value = (uint8_t *) &char_value.default_pnp_id;					/*Initial Value*/
	
	device_info_serv->serv_chars[7].value_init_len = DIS_CHAR_PNP_ID_INIT_LEN;
	device_info_serv->serv_chars[7].value_max_len = DIS_CHAR_PNP_ID_MAX_LEN;
#if BLE_PAIR_ENABLE
	device_info_serv->serv_chars[7].value_permissions = AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR;   /* permissions */
#else
	device_info_serv->serv_chars[7].value_permissions = AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR;   /* permissions */
#endif
	device_info_serv->serv_chars[7].user_desc = NULL;           /* user defined name */
	device_info_serv->serv_chars[7].user_desc_len = 0;
	device_info_serv->serv_chars[7].user_desc_max_len = 0;
	device_info_serv->serv_chars[7].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;             /*user description permissions*/
	device_info_serv->serv_chars[7].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*client config permissions*/
	device_info_serv->serv_chars[7].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*server config permissions*/
	device_info_serv->serv_chars[7].user_desc_handle = 0;             /*user desc handles*/
	device_info_serv->serv_chars[7].client_config_handle = 0;         /*client config handles*/
	device_info_serv->serv_chars[7].server_config_handle = 0;         /*server config handles*/
	device_info_serv->serv_chars[7].presentation_format = NULL;       /* presentation format */
	
	//Characteristic Info for IEEE 11073-20601 Regulatory Certification Data List
	device_info_serv->serv_chars[8].char_val_handle = 0;          /* handle stored here */
	device_info_serv->serv_chars[8].uuid.type = AT_BLE_UUID_16;
	device_info_serv->serv_chars[8].uuid.uuid[0] = (uint8_t) DIS_CHAR_IEEE_REG_CERT_DATA_LIST_UUID;          /* UUID : Software Revision */
	device_info_serv->serv_chars[8].uuid.uuid[1] = (uint8_t) (DIS_CHAR_IEEE_REG_CERT_DATA_LIST_UUID >> 8);          /* UUID : Software Revision */
	device_info_serv->serv_chars[8].properties = AT_BLE_CHAR_READ; /* Properties */
	device_info_serv->serv_chars[8].init_value = char_value.ieee_reg_cert_data_list;					/*Initial Value*/
	device_info_serv->serv_chars[8].value_init_len = DIS_CHAR_IEEE_REG_CERT_DATA_LIST_INIT_LEN;
	device_info_serv->serv_chars[8].value_max_len = DIS_CHAR_IEEE_REG_CERT_DATA_LIST_MAX_LEN;
#if BLE_PAIR_ENABLE
	device_info_serv->serv_chars[8].value_permissions = AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR;   /* permissions */
#else
	device_info_serv->serv_chars[8].value_permissions = AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR;   /* permissions */
#endif
	device_info_serv->serv_chars[8].user_desc = NULL;           /* user defined name */
	device_info_serv->serv_chars[8].user_desc_len = 0;
	device_info_serv->serv_chars[8].user_desc_max_len = 0;
	device_info_serv->serv_chars[8].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;             /*user description permissions*/
	device_info_serv->serv_chars[8].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*client config permissions*/
	device_info_serv->serv_chars[8].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*server config permissions*/
	device_info_serv->serv_chars[8].user_desc_handle = 0;             /*user desc handles*/
	device_info_serv->serv_chars[8].client_config_handle = 0;         /*client config handles*/
	device_info_serv->serv_chars[8].server_config_handle = 0;         /*server config handles*/
	device_info_serv->serv_chars[8].presentation_format = NULL;       /* presentation format */
}

/**@brief Register a dis service instance inside stack. */
at_ble_status_t dis_primary_service_define(dis_gatt_service_handler_t *dis_primary_service)
{
	
	return(at_ble_primary_service_define(&dis_primary_service->serv_uuid,
	&dis_primary_service->serv_handle,
	NULL, 0,
	dis_primary_service->serv_chars,DIS_TOTAL_CHARATERISTIC_NUM));
}

/**@brief  Update the DIS characteristic value after defining the services using dis_primary_service_define*/
at_ble_status_t dis_info_update(dis_gatt_service_handler_t *dis_serv , dis_info_type info_type, dis_info_data* info_data, at_ble_handle_t conn_handle)
{
	if (info_data->data_len > dis_serv->serv_chars[info_type].value_max_len)
	{
		DBG_LOG("invalid length parameter");
		return AT_BLE_FAILURE;
	}
	
	//updating application data
	memcpy(&(dis_serv->serv_chars[info_type].init_value), info_data->info_data,info_data->data_len);
	
	//updating the characteristic value
	if ((at_ble_characteristic_value_set(dis_serv->serv_chars[info_type].char_val_handle, info_data->info_data, info_data->data_len)) != AT_BLE_SUCCESS){
		DBG_LOG("updating the characteristic failed\r\n");
	} else {
		return AT_BLE_SUCCESS;
	}
        ALL_UNUSED(conn_handle);
	return AT_BLE_FAILURE;
}
