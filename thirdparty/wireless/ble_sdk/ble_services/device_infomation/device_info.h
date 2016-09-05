/**
 * \file
 *
 * \brief Device information Service declarations
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel
 *Support</a>
 */

// <<< Use Configuration Wizard in Context Menu >>>
// <h> Device Infomation Service Configuration
// =======================
#ifndef __DEVICE_INFO_H__
#define __DEVICE_INFO_H__

/****************************************************************************************
*							        Includes	                                     							*
****************************************************************************************/

#include "ble_manager.h"

/****************************************************************************************
*							        Macros	                                     							*
****************************************************************************************/

/** @brief User should set the default manufacturer name before calling dis_init_service API */
//	<s.20>Manufacture Name
//	<i> String Descriptor describing Manufacture Name.
//	<id> default_manufacturer_name
#define DEFAULT_MANUFACTURER_NAME					("ATMEL")

/** @brief User should set the length based on DEFAULT_MANUFACTURER_NAME */
#define DIS_CHAR_MANUFACTURER_NAME_INIT_LEN			(0x05)

/** @brief User can set the maximum length for manufacture name */
#define DIS_CHAR_MANUFACTURER_NAME_MAX_LEN			(0x14)

/** @brief User should set the default model number before calling dis_init_service API */
//	<s.20>Model Number
//	<i>String Descriptor describing Model Number.
//	<id> default_model_number
#define DEFAULT_MODEL_NUMBER						("BLE DEVICE")

/** @brief User should set the length based on DEFAULT_MODEL_NUMBER*/
#define DIS_CHAR_MODEL_NUMBER_INIT_LEN				(0x0a)

/** @brief User can set the maximum length for model number */
#define DIS_CHAR_MODEL_NUMBER_MAX_LEN				(0x14)

/** @brief User should set the default serial number before calling dis_init_service API */
//  <s.20>Serial Number
//  <i>String Descriptor describing Serial Number.
//	<id> default_serial_number
#define DEFAULT_SERIAL_NUMBER						("BTLC1000/SAMB11")

/** @brief User should set the length based on  DEFAULT_SERIAL_NUMBER*/
#define DIS_CHAR_SERIAL_NUMBER_INIT_LEN			0x0f

/** @brief User can set the maximum length for serial number */
#define DIS_CHAR_SERIAL_NUMBER_MAX_LEN			0x14

/** @brief User should set the default hardware revision before calling dis_init_service API */
//  <s.20>Hardware Revision
//  <i>String Descriptor describing Hardware Revision.
//	<id> default_hardware_revision
#define DEFAULT_HARDWARE_REVISION					("Rev A")

/** @brief User should set the length based on  DEFAULT_HARDWARE_REVISION*/
#define DIS_CHAR_HARDWARE_REVISION_INIT_LEN		0x05

/** @brief User can set the maximum length for hardware revision */
#define DIS_CHAR_HARDWARE_REVISION_MAX_LEN		0x14

/** @brief User should set the default software revision before calling dis_init_service API */
//  <s.20>Software Revision
//  <i>String Descriptor describing Software Revision.
//	<id> default_hardware_revision
#define DEFAULT_SOFTWARE_REVISION					("SW_BETA")

/** @brief User should set the length based on  DEFAULT_SOFTWARE_REVISION*/
#define DIS_CHAR_SOFTWARE_REVISION_INIT_LEN		0x07

/** @brief User can set the maximum length for software revision */
#define DIS_CHAR_SOFTWARE_REVISION_MAX_LEN		0x14

/** @brief User should set the default firmware revision before calling dis_init_service API */
//  <s.20>Firmware Revision
//  <i>String Descriptor describing Firmware Revision.
//	<id> default_firmware_reivsion
#define DEFAULT_FIRMWARE_REIVSION					("FW_BETA")

/** @brief User should set the length based on  DEFAULT_FIRMWARE_REIVSION*/
#define DIS_CHAR_FIRMWARE_REIVSION_INIT_LEN		0x07

/** @brief User can set the maximum length for firmware revision */
#define DIS_CHAR_FIRMWARE_REIVSION_MAX_LEN		0x14

/** @brief number of device information service characteristics */
#define DIS_TOTAL_CHARATERISTIC_NUM             0x09

/** @brief system id characteristic initial length*/
#define DIS_CHAR_SYSTEM_ID_INIT_LEN sizeof(system_id_char_value_t)

/** @brief system id characteristic maximum length*/
#define DIS_CHAR_SYSTEM_ID_MAX_LEN	sizeof(system_id_char_value_t)

/** @brief PnP id characteristic initial length */
#define DIS_CHAR_PNP_ID_INIT_LEN				0x07

/** @brief PnP id characteristic maximum length */
#define DIS_CHAR_PNP_ID_MAX_LEN					0x07

/** @brief IEEE regulatory certification data list characteristic minimum length */
#define DIS_CHAR_IEEE_REG_CERT_DATA_LIST_INIT_LEN	0x01

/** @brief IEEE regulatory certification data list characteristic maximum length */
#define DIS_CHAR_IEEE_REG_CERT_DATA_LIST_MAX_LEN	0x0a

/** @brief PnP ID characteristic value configure by user*/
#define PNP_ID_VENDOR_ID_SOURCE		0x01
#define PNP_ID_VENDOR_ID			0x2222
#define PNP_ID_PRODUCT_ID			0x3333
#define PNP_ID_PRODUCT_VERSION		0x0001

/** @brief system ID characteristic default values */
#define SYSTEM_ID_MANUFACTURER_ID_LEN	0x05
#define SYSTEM_ID_ORG_UNIQUE_ID_LEN		0x03	
#define SYSTEM_ID_MANUFACTURER_ID		"\x00\x00\x00\x00\x00"
#define SYSTEM_ID_ORG_UNIQUE_ID			"\x00\x04\x25"

/** @brief Macro used for updating manufacturing string after defining the service using dis_primary_service_define*/
#define UPDATE_MANUFACTURER_STRING(ptr,info_data, conn_handle) do {  \
	if ( (dis_info_update(ptr,DIS_MANUFACTURER_NAME,info_data, conn_handle)) != AT_BLE_SUCCESS) { \
		DBG_LOG("Updating Manufacturer string failed");  \
	}\
} while (0)

/** @brief Macro used for updating model number after defining the service using dis_primary_service_define*/
#define UPDATE_MODEL_NUMBER(ptr,info_data, conn_handle) do {   \
	if (dis_info_update(ptr,DIS_MODEL_NUMBER,info_data, conn_handle) != AT_BLE_SUCCESS) { \
		DBG_LOG("Updating model number failed");  \
	}\
} while (0)

/** @brief Macro used for updating serial number after defining the service using dis_primary_service_define*/
#define UPDATE_SERIAL_NUMBER(ptr,info_data, conn_handle) do{   \
	if (dis_info_update(ptr,DIS_SERIAL_NUMBER,info_data, conn_handle) != AT_BLE_SUCCESS) { \
		DBG_LOG("Updating serial number failed");  \
	}\
} while (0)

/** @brief Macro used for updating hardware revision after defining the service using dis_primary_service_define*/
#define UPDATE_HARDWARE_REVISION(ptr,info_data, conn_handle) do{   \
	if (dis_info_update(ptr,DIS_HARDWARE_REVISION,info_data, conn_handle) != AT_BLE_SUCCESS) { \
		DBG_LOG("Updating hardware revision failed");  \
	}\
} while (0)

/** @brief Macro used for updating firmware revision after defining the service using dis_primary_service_define*/
#define UPDATE_FIRMWARE_REVISION(ptr,info_data, conn_handle) do{   \
	if (dis_info_update(ptr,DIS_FIRMWARE_REVISION,info_data, conn_handle) != AT_BLE_SUCCESS) { \
		DBG_LOG("Updating firmware revision failed");  \
	}\
} while (0)

/** @brief Macro used for updating software revision after defining the service using dis_primary_service_define*/
#define UPDATE_SOFTWARE_REVISION(ptr,info_data, conn_handle) do{   \
	if (dis_info_update(ptr,DIS_SOFTWARE_REVISION,info_data, conn_handle) != AT_BLE_SUCCESS) { \
		DBG_LOG("Updating software revision failed");  \
	}\
} while (0)

/** @brief Macro used for updating system ID after defining the service using dis_primary_service_define*/
#define UPDATE_SYSTEM_ID(ptr,info_data, conn_handle) do {   \
	if (dis_info_update(ptr,DIS_SYSTEM_ID,info_data, conn_handle) != AT_BLE_SUCCESS) { \
		DBG_LOG("Updating system id failed");  \
	}\
} while (0)

/** @brief Macro used for updating PnP ID after defining the service using dis_primary_service_define*/
#define UPDATE_PNP_ID(ptr,info_data, conn_handle) do {   \
	if (dis_info_update(ptr,DIS_PNP_ID,info_data, conn_handle) != AT_BLE_SUCCESS) { \
		DBG_LOG("Updating PnP ID failed");  \
	}\
} while (0)

/** @brief Macro used for updating IEEE regulatory certification data list after defining the service using dis_primary_service_define*/
#define UPDATE_IEEE_REG_CERT_DATA_LIST(ptr,info_data, conn_handle) do {   \
	if (dis_info_update(ptr,DIS_IEEE_REG_CERT_DATA_LIST,info_data, conn_handle) != AT_BLE_SUCCESS) { \
		DBG_LOG("Updating IEEE regulatory certification data list failed");  \
	}\
} while (0)


/****************************************************************************************
*							        Enumerations	                                       							*
****************************************************************************************/

/**@brief The type of the information*/
typedef enum {
	/* manufacturer name characteristic */
	DIS_MANUFACTURER_NAME= 0,
	/* model number characteristic */
	DIS_MODEL_NUMBER,
	/* serial number characteristic */
	DIS_SERIAL_NUMBER,
	/* Hardware revision characteristic */
	DIS_HARDWARE_REVISION,
	/* Firmware revision characteristic */
	DIS_FIRMWARE_REVISION,
	/* Software revision characteristic */
	DIS_SOFTWARE_REVISION,
	/* System id characteristic */
	DIS_SYSTEM_ID,
	/* PnP ID characteristic */
	DIS_PNP_ID,
	/* IEEE regulatory certification data list characteristic */
	DIS_IEEE_REG_CERT_DATA_LIST,
	/* must be the last element */
	DIS_END_VALUE,
} dis_info_type;


/****************************************************************************************
*							        Structures                                     							*
****************************************************************************************/


typedef struct dis_gatt_service_handler
{
		at_ble_uuid_t	serv_uuid;
		at_ble_handle_t	serv_handle;
		at_ble_characteristic_t	serv_chars[DIS_TOTAL_CHARATERISTIC_NUM];	
}dis_gatt_service_handler_t;


/** @brief system ID characteristic value information */
typedef struct{
	/// manufacturer identifier
	uint8_t manufacturer_id[5];
	/// organizational unique identifier 
	uint8_t org_unique_id[3];
}system_id_char_value_t;

COMPILER_PACK_SET(1)

/** @brief pnp characteristic value information */
typedef struct {
	/// vendor id source
	uint8_t vendor_id_source;
	/// vendor id
	uint16_t vendor_id;
	/// product id
	uint16_t product_id;
	/// product version
	uint16_t product_version;
}pnp_id_char_value_t;

COMPILER_PACK_RESET()

/** @brief characteristic value information */
typedef struct {
	/// manufacturer name 
	uint8_t manufacturer_name[DIS_CHAR_MANUFACTURER_NAME_MAX_LEN];
	/// model number
	uint8_t default_model_number[DIS_CHAR_MODEL_NUMBER_MAX_LEN];
	/// serial number
	uint8_t default_serial_number[DIS_CHAR_SERIAL_NUMBER_MAX_LEN];
	/// hardware revision
	uint8_t default_hardware_revision[DIS_CHAR_HARDWARE_REVISION_MAX_LEN];
	/// default firmware revision
	uint8_t default_firmware_revision[DIS_CHAR_FIRMWARE_REIVSION_MAX_LEN];
	/// software revision
	uint8_t default_software_revision[DIS_CHAR_SOFTWARE_REVISION_MAX_LEN];
	/// system id
	system_id_char_value_t default_system_id;
	/// PnP ID
	pnp_id_char_value_t default_pnp_id;
	/// ieee regulatory certification data list
	uint8_t ieee_reg_cert_data_list[DIS_CHAR_IEEE_REG_CERT_DATA_LIST_MAX_LEN];
}device_info_char_value_t;

/** @brief Configurable Client characteristic data for a given dis info type*/
typedef struct{
	// length of the data to be updated
	uint16_t data_len;
	// data to be updated
	uint8_t *info_data;
}dis_info_data;

/****************************************************************************************
*                                       Functions                                       *
****************************************************************************************/

/**@brief Update the DIS characteristic value after defining the services using dis_primary_service_define
 *
 * @param[in] dis_serv  dis service instance
 * @param[in] info_type dis characteristic type to be updated
 * @param[in] info_data data need to be updated
 * @return @ref AT_BLE_SUCCESS operation completed successfully .
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t dis_info_update(dis_gatt_service_handler_t *dis_serv , dis_info_type info_type, dis_info_data* info_data, at_ble_handle_t conn_handle);


/**@brief DIS service and characteristic initialization(Called only once by user).
 *
 * @param[in] device_info_serv dis service instance
 *
 * @return none
 */
void dis_init_service(dis_gatt_service_handler_t *device_info_serv );

/**@brief Register a dis service instance inside stack. 
 *
 * @param[in] dis_primary_service dis service instance
 *
 * @return @ref AT_BLE_SUCCESS operation completed successfully
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t dis_primary_service_define(dis_gatt_service_handler_t *dis_primary_service);

#endif /*__DEVICE_INFO_H__*/
// </h>

// <<< end of configuration section >>>
