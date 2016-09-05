/**
 * \file
 *
 * \brief Simple Broadcaster Application
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
 * Support</a>
 */

/**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the Simple Broadcaster Application
 */
/*- Includes ---------------------------------------------------------------*/

#include "simple_broadcaster_app.h"

/* Indicate to user that beacon advertisement started in broadcaster mode*/
static void ble_device_broadcaster_ind(void);

uint8_t adv_data[MAX_ADV_LEN];
uint8_t scan_rsp_data[MAX_SCAN_LEN];
uint8_t adv_length;
uint8_t scan_length;

/* /user can set the advertisement type, it is set as scannable undirected by
 * default. */
uint32_t adv_type = ADV_TYPE_NONCONN_UNDIRECTED;

/* /user can set the data type, it is set as part of advertisement data by
 * default. */
data_type_t data_type = ADVERTISEMENT_DATA;

/* BLE device advertisement in broadcast mode indicator */
static void ble_device_broadcaster_ind(void)
{
	/* Toggle the application LED */
	LED_On(LED0);
}

static status_t brd_set_data_type(uint8_t type)
{
	if (type == ADVERTISEMENT_DATA) {
		data_type = ADVERTISEMENT_DATA;
	} else if (type == SCAN_RESP_DATA &&
			adv_type == ADV_TYPE_SCANNABLE_UNDIRECTED) {
		data_type = SCAN_RESP_DATA;
	} else {
		return STATUS_INVALID_PARAM;
	}

	return STATUS_SUCCEEDED;
}

/** @brief Starts advertising
 *
 *@param[in] none
 *@return none
 *
 */
static void brd_start_broadcast(void)
{
	at_ble_status_t status;
	at_ble_adv_mode_t adv_mode;
	adv_mode = AT_BLE_ADV_BROADCASTER_MODE;

	if (at_ble_adv_data_set(adv_data, adv_length, scan_rsp_data, scan_length) != AT_BLE_SUCCESS) {
		DBG_LOG("BLE Broadcast data set failed");
		return;
	} else {
		DBG_LOG("BLE Broadcast data set success");
	}

	if ((status     = at_ble_adv_start(adv_type,
					adv_mode,
					NULL, AT_BLE_ADV_FP_ANY,
					APP_BROADCAST_FAST_ADV,
					APP_BROADCAST_ADV_TIMEOUT,
					0)) != AT_BLE_SUCCESS) {
		DBG_LOG("BLE Broadcast start failed(%d)", status);
	} else {
		DBG_LOG("Started Broadcasting");

		/* Indicate to user that beacon advertisement started in
		 * broadcaster mode*/
		ble_device_broadcaster_ind();
	}
}

/** @brief Set advertisement data with complete list of service uuid of 16bits
 *
 * @param[in] list_uuid16 list of service uuid of 16bits
 * @param[in] length length of the list
 * @return @ref STATUS_SUCCEEDED adding the list to advertisement data success
 * @return @ref STATUS_MAX_LENGTH_REACHED advertisement data does not have
 * enough space to hold the list
 * @return @ref STATUS_INVALID_PARAM parameters passed are invalid
 *
 */
static status_t brd_adv_comp_list_service_uuid16(uint8_t *list_uuid16, uint8_t length)
{
	if (length <= 0 || !list_uuid16) {
		return STATUS_INVALID_PARAM;
	}

	if (data_type == ADVERTISEMENT_DATA) {
		if ((adv_length + length) > (MAX_ADV_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			adv_data[adv_length++] = length + ADV_TYPE_LEN;
			adv_data[adv_length++] = ADV_DATA_TYPE_COMPLETE_SERVICE_UUID16;
			memcpy((adv_data + adv_length), list_uuid16, length);
			adv_length += length;
		}
	} else if (data_type == SCAN_RESP_DATA) {
		if ((scan_length + length) > (MAX_SCAN_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			scan_rsp_data[scan_length++] = length + ADV_TYPE_LEN;
			scan_rsp_data[scan_length++] = ADV_DATA_TYPE_COMPLETE_SERVICE_UUID16;
			memcpy((scan_rsp_data + scan_length), list_uuid16, length);
			scan_length += length;
		}
	}

	return STATUS_SUCCEEDED;
}

/** @brief Set advertisement data with incomplete list of service uuid of 16bits
 *
 * @param[in] list_uuid16 list of service uuid of 16bits
 * @param[in] length length of the list
 * @return @ref STATUS_SUCCEEDED adding the list to advertisement data success
 * @return @ref STATUS_MAX_LENGTH_REACHED advertisement data does not have
 * enough space to hold the list
 * @return @ref STATUS_INVALID_PARAM parameters passed are invalid
 * @return @ref STATUS_OPERATION_NOT_SUPPORTED trying to send incomplete list on
 * scan response data.
 *
 */
static status_t brd_adv_incomp_list_service_uuid16(uint8_t *list_uuid16, uint8_t length)
{
	if (length <= 0 || !list_uuid16) {
		return STATUS_INVALID_PARAM;
	}

	if (data_type == ADVERTISEMENT_DATA) {
		if ((adv_length + length) > (MAX_ADV_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			adv_data[adv_length++] = length + ADV_TYPE_LEN;
			adv_data[adv_length++] = ADV_DATA_TYPE_INCOMPLETE_SERVICE_UUID16;
			memcpy((adv_data + adv_length), list_uuid16, length);
			adv_length += length;
		}
	} else if (data_type == SCAN_RESP_DATA) {
		return STATUS_OPERATION_NOT_SUPPORTED;
	}

	return STATUS_SUCCEEDED;
}

/** @brief Set advertisement data with complete list of service uuid of 32bits
 *
 * @param[in] list_uuid32 list of service uuid of 32bits
 * @param[in] length length of the list
 * @return @ref STATUS_SUCCEEDED adding the list to advertisement data success
 * @return @ref STATUS_MAX_LENGTH_REACHED advertisement data does not have
 * enough space to hold the list
 * @return @ref STATUS_INVALID_PARAM parameters passed are invalid
 *
 */
static status_t brd_adv_comp_list_service_uuid32(uint8_t *list_uuid32, uint8_t length)
{
	if (length <= 0 || !list_uuid32) {
		return STATUS_INVALID_PARAM;
	}

	if (data_type == ADVERTISEMENT_DATA) {
		if ((adv_length + length) > (MAX_ADV_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			adv_data[adv_length++] = length + ADV_TYPE_LEN;
			adv_data[adv_length++] = ADV_DATA_TYPE_COMPLETE_SERVICE_UUID32;
			memcpy((adv_data + adv_length), list_uuid32, length);
			adv_length += length;
		}
	} else {
		if ((scan_length + length) > (MAX_SCAN_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			scan_rsp_data[scan_length++] = length + ADV_TYPE_LEN;
			scan_rsp_data[scan_length++] = ADV_DATA_TYPE_COMPLETE_SERVICE_UUID32;
			memcpy((scan_rsp_data + scan_length), list_uuid32, length);
			scan_length += length;
		}
	}

	return STATUS_SUCCEEDED;
}

/** @brief Set advertisement data with incomplete list of service uuid of 32bits
 *
 * @param[in] list_uuid32 list of service uuid of 32bits
 * @param[in] length length of the list
 * @return @ref STATUS_SUCCEEDED adding the list to advertisement data success
 * @return @ref STATUS_MAX_LENGTH_REACHED advertisement data does not have
 * enough space to hold the list
 * @return @ref STATUS_INVALID_PARAM parameters passed are invalid
 * @return @ref STATUS_OPERATION_NOT_SUPPORTED trying to send incomplete list on
 * scan response data.
 *
 */
static status_t brd_adv_incomp_list_service_uuid32(uint8_t *list_uuid32, uint8_t length)
{
	if (length <= 0 || !list_uuid32) {
		return STATUS_INVALID_PARAM;
	}

	if (data_type == ADVERTISEMENT_DATA) {
		if ((adv_length + length) > (MAX_ADV_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			adv_data[adv_length++] = length + ADV_TYPE_LEN;
			adv_data[adv_length++] = ADV_DATA_TYPE_INCOMPLETE_SERVICE_UUID32;
			memcpy((adv_data + adv_length), list_uuid32, length);
			adv_length += length;
		}
	} else {
		return STATUS_OPERATION_NOT_SUPPORTED;
	}

	return STATUS_SUCCEEDED;
}

/** @brief Set advertisement data with complete list of service uuid of 128bits
 *
 * @param[in] list_uuid128 list of service uuid of 128bits
 * @param[in] length length of the list
 * @return @ref STATUS_SUCCEEDED adding the list to advertisement data success
 * @return @ref STATUS_MAX_LENGTH_REACHED advertisement data does not have
 * enough space to hold the list
 * @return @ref STATUS_INVALID_PARAM parameters passed are invalid
 *
 */
static status_t brd_adv_comp_list_service_uuid128(uint8_t *list_uuid128, uint8_t length)
{
	if (length <= 0 || !list_uuid128) {
		return STATUS_INVALID_PARAM;
	}

	if (data_type == ADVERTISEMENT_DATA) {
		if ((adv_length + length) > (MAX_ADV_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			adv_data[adv_length++] = length + ADV_TYPE_LEN;
			adv_data[adv_length++] = ADV_DATA_TYPE_COMPLETE_SERVICE_UUID128;
			memcpy((adv_data + adv_length), list_uuid128, length);
			adv_length += length;
		}
	} else {
		if ((scan_length + length) > (MAX_SCAN_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			scan_rsp_data[scan_length++] = length + ADV_TYPE_LEN;
			scan_rsp_data[scan_length++]
				= ADV_DATA_TYPE_COMPLETE_SERVICE_UUID128;
			memcpy((scan_rsp_data + scan_length), list_uuid128,
					length);
			scan_length += length;
		}
	}

	return STATUS_SUCCEEDED;
}

/** @brief Set advertisement data with incomplete list of service uuid of 32bits
 *
 * @param[in] list_uuid128 list of service uuid of 128bits
 * @param[in] length length of the list
 * @return @ref STATUS_SUCCEEDED adding the list to advertisement data success
 * @return @ref STATUS_MAX_LENGTH_REACHED advertisement data does not have
 * enough space to hold the list
 * @return @ref STATUS_INVALID_PARAM parameters passed are invalid
 * @return @ref STATUS_OPERATION_NOT_SUPPORTED trying to send incomplete list on
 * scan response data.
 *
 */
static status_t brd_adv_incomp_list_service_uuid128(uint8_t *list_uuid128, uint8_t length)
{
	if (length <= 0 || !list_uuid128) {
		return STATUS_INVALID_PARAM;
	}

	if (data_type == ADVERTISEMENT_DATA) {
		if ((adv_length + length) > (MAX_ADV_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			adv_data[adv_length++] = length + ADV_TYPE_LEN;
			adv_data[adv_length++] = ADV_DATA_TYPE_INCOMPLETE_SERVICE_UUID128;
			memcpy((adv_data + adv_length), list_uuid128, length);
			adv_length += length;
		}
	} else {
		return STATUS_OPERATION_NOT_SUPPORTED;
	}

	return STATUS_SUCCEEDED;
}

/** @brief Set advertisement data with complete local name
 *
 * @param[in] local_name local name of the device
 * @param[in] length length of the local name
 * @return @ref STATUS_SUCCEEDED adding the complete local name to advertisement
 * data success
 * @return @ref STATUS_MAX_LENGTH_REACHED advertisement data does not have
 * enough space to hold the local name
 * @return @ref STATUS_INVALID_PARAM parameters passed are invalid
 *
 */
static status_t brd_adv_comp_local_name(uint8_t *local_name, uint8_t length)
{
	if (length <= 0 || !local_name) {
		return STATUS_INVALID_PARAM;
	}

	if (data_type == ADVERTISEMENT_DATA) {
		if ((adv_length + length) > (MAX_ADV_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			adv_data[adv_length++] = length + ADV_TYPE_LEN;
			adv_data[adv_length++] = ADV_DATA_TYPE_COMPLETE_LOCAL_NAME;
			memcpy((adv_data + adv_length), local_name, length);
			adv_length += length;
		}
	} else {
		if ((scan_length + length) > (MAX_SCAN_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			scan_rsp_data[scan_length++] = length + ADV_TYPE_LEN;
			scan_rsp_data[scan_length++] = ADV_DATA_TYPE_COMPLETE_LOCAL_NAME;
			memcpy((scan_rsp_data + scan_length), local_name, length);
			scan_length += length;
		}
	}

	return STATUS_SUCCEEDED;
}

/** @brief Set advertisement data with shortened local name
 *
 * @param[in] local_name local name of the device
 * @param[in] length length of the local name
 * @return @ref STATUS_SUCCEEDED adding the shortened local name to advertisement
 * data success
 * @return @ref STATUS_MAX_LENGTH_REACHED advertisement data does not have
 * enough space to hold the local name
 * @return @ref STATUS_INVALID_PARAM parameters passed are invalid
 * @return @ref STATUS_OPERATION_NOT_SUPPORTED trying to send shortened local
 * name.
 *
 */
static status_t brd_adv_shortened_local_name(uint8_t *local_name, uint8_t length)
{
	if (length <= 0 || !local_name) {
		return STATUS_INVALID_PARAM;
	}

	if (data_type == ADVERTISEMENT_DATA) {
		if ((adv_length + length) > (MAX_ADV_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			adv_data[adv_length++] = length + ADV_TYPE_LEN;
			adv_data[adv_length++] = ADV_DATA_TYPE_SHORTENED_LOCAL_NAME;
			memcpy((adv_data + adv_length), local_name, length);
			adv_length += length;
		}
	} else {
		return STATUS_OPERATION_NOT_SUPPORTED;
	}

	return STATUS_SUCCEEDED;
}

/** @brief Set advertisement type
 *
 * @param[in] type type can be ADV_TYPE_SCANNABLE_UNDIRECTED or
 * ADV_TYPE_NONCONN_UNDIRECTED
 * @return @ref STATUS_SUCCEEDED successfully set advertisement type
 * @return @ref STATUS_INVALID_PARAM parameters passed are invalid
 *
 */
static status_t brd_set_advertisement_type(uint8_t type)
{
	if (type == ADV_TYPE_SCANNABLE_UNDIRECTED) {
		adv_type = AT_BLE_ADV_TYPE_SCANNABLE_UNDIRECTED;
		DBG_LOG("Advertisement type set to scannable undirected");
		} else if (type == ADV_TYPE_NONCONN_UNDIRECTED) {
		adv_type = AT_BLE_ADV_TYPE_NONCONN_UNDIRECTED;
		DBG_LOG("Advertisement type set to nonconnectable undirected");
		} else {
		return STATUS_INVALID_PARAM;
	}

	return STATUS_SUCCEEDED;
}
/** @brief Set advertisement data with appearance of the device
 *
 * @param[in] appearance appearance of the device
 * @param[in] length length of the local name
 * @return @ref STATUS_SUCCEEDED adding the appearance to advertisement data
 * success
 * @return @ref STATUS_MAX_LENGTH_REACHED advertisement data does not have
 * enough space to hold the local name
 * @return @ref STATUS_INVALID_PARAM parameters passed are invalid
 *
 */
static status_t brd_adv_appearance(uint8_t *appearance, uint8_t length)
{
	if (length <= 0 || !appearance) {
		return STATUS_INVALID_PARAM;
	}

	if (data_type == ADVERTISEMENT_DATA) {
		if ((adv_length + length) > (MAX_ADV_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			adv_data[adv_length++] = length + ADV_TYPE_LEN;
			adv_data[adv_length++] = ADV_DATA_TYPE_APPEARANCE;
			memcpy((adv_data + adv_length), appearance, length);
			adv_length += length;
		}
	} else {
		if ((scan_length + length) > (MAX_SCAN_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			scan_rsp_data[scan_length++] = length + ADV_TYPE_LEN;
			scan_rsp_data[scan_length++] = ADV_DATA_TYPE_APPEARANCE;
			memcpy((scan_rsp_data + scan_length), appearance, length);
			scan_length += length;
		}
	}

	return STATUS_SUCCEEDED;
}

/** @brief Set advertisement data with manufacturer data
 *
 * @param[in] data manufacturer data of the device
 * @param[in] length length of the local name
 * @return @ref STATUS_SUCCEEDED adding the manufacturer data to advertisement
 * data success
 * @return @ref STATUS_MAX_LENGTH_REACHED advertisement data does not have
 * enough space to hold the local name
 * @return @ref STATUS_INVALID_PARAM parameters passed are invalid
 *
 */
static status_t brd_adv_manufacturer_data(uint8_t *data, uint8_t length)
{
	if (length <= 0 || !data) {
		return STATUS_INVALID_PARAM;
	}

	if (data_type == ADVERTISEMENT_DATA) {
		if ((adv_length + length) > (MAX_ADV_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			adv_data[adv_length++] = length + ADV_TYPE_LEN;
			adv_data[adv_length++] = ADV_DATA_TYPE_MANUFACTURER_DATA;
			memcpy((adv_data + adv_length), data, length);
			adv_length += length;
		}
	} else {
		if ((scan_length + length) > (MAX_SCAN_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			scan_rsp_data[scan_length++] = length + ADV_TYPE_LEN;
			scan_rsp_data[scan_length++] = ADV_DATA_TYPE_MANUFACTURER_DATA;
			memcpy((scan_rsp_data + scan_length), data, length);
			scan_length += length;
		}
	}

	return STATUS_SUCCEEDED;
}

/** @brief Set advertisement data
 *
 * @param[in] adv_data_type type of the advertisement data type
 * @param[in] adv_usr_data reference to a user data corresponding to the
 * advertisement data type
 * @return @ref STATUS_SUCCEEDED successfully added user data to the advertisement
 * data
 * @return @ref STATUS_INVALID_PARAM parameters passed are invalid
 * @return @ref STATUS_MAX_LENGTH_REACHED advertisement data does not have
 * enough space to hold the user data
 * @return @ref STATUS_OPERATION_NOT_SUPPORTED trying to add data in unmatched
 * advertisement type
 *
 */
static status_t brd_set_advertisement_data(adv_data_type_t adv_data_type, uint8_t *adv_usr_data, uint8_t length)
{
	status_t status;

	switch (adv_data_type) {
	case ADV_DATA_TYPE_INCOMPLETE_SERVICE_UUID16:
	{
		if ((status = brd_adv_incomp_list_service_uuid16(adv_usr_data, length)) != STATUS_SUCCEEDED) {
			DBG_LOG( "adding incomplete list of service uuid16 failed");
			return status;
		}

		DBG_LOG("incomplete list of service uuid16 set");
	}
	break;

	case ADV_DATA_TYPE_INCOMPLETE_SERVICE_UUID32:
	{
		if ((status = brd_adv_incomp_list_service_uuid32(adv_usr_data, length)) != STATUS_SUCCEEDED) {
			DBG_LOG("adding incomplete list of service uuid32 failed");
			return status;
		}

		DBG_LOG("incomplete list of service uuid32 set");
	}
	break;

	case ADV_DATA_TYPE_INCOMPLETE_SERVICE_UUID128:
	{
		if ((status = brd_adv_incomp_list_service_uuid128( adv_usr_data, length)) != STATUS_SUCCEEDED) {
			DBG_LOG(
					"adding incomplete list of service uuid128 failed");
			return status;
		}

		DBG_LOG("incomplete list of service uuid128 set");
	}
	break;

	case ADV_DATA_TYPE_COMPLETE_SERVICE_UUID16:
	{
		if ((status = brd_adv_comp_list_service_uuid16( adv_usr_data, length)) != STATUS_SUCCEEDED) {
			DBG_LOG("adding complete list of service uuid16 failed");
			return status;
		}

		DBG_LOG("complete list of service uuid16 set");
	}
	break;

	case ADV_DATA_TYPE_COMPLETE_SERVICE_UUID32:
	{
		DBG_LOG("name set");
		if ((status = brd_adv_comp_list_service_uuid32( adv_usr_data, length)) != STATUS_SUCCEEDED) {
			DBG_LOG("adding complete list of service uuid32 failed");
			return status;
		}

		DBG_LOG("complete list of service uuid32 set");
	}
	break;

	case ADV_DATA_TYPE_COMPLETE_SERVICE_UUID128:
	{
		if ((status = brd_adv_comp_list_service_uuid128(adv_usr_data, length)) != STATUS_SUCCEEDED) {
			DBG_LOG("adding complete list of service uuid128 failed");
			return status;
		}

		DBG_LOG("complete list of service uuid128 set");
	}
	break;

	case ADV_DATA_TYPE_COMPLETE_LOCAL_NAME:
	{
		if ((status = brd_adv_comp_local_name(adv_usr_data, length)) != STATUS_SUCCEEDED) {
			DBG_LOG("adding complete local name failed");
			return status;
		}

		DBG_LOG("complete name set");
	}
	break;

	case ADV_DATA_TYPE_SHORTENED_LOCAL_NAME:
	{
		if ((status = brd_adv_shortened_local_name( adv_usr_data, length)) != STATUS_SUCCEEDED) {
			DBG_LOG("adding shortened complete local name failed");
			return status;
		}

		DBG_LOG("shortened name set");
	}
	break;

	case ADV_DATA_TYPE_MANUFACTURER_DATA:
	{
		if ((status = brd_adv_manufacturer_data(adv_usr_data, length)) != STATUS_SUCCEEDED) {
			DBG_LOG("adding manufacturer data failed");
			return status;
		}

		DBG_LOG("manufacturer data set");
	}
	break;

	case ADV_DATA_TYPE_APPEARANCE:
	{
		if ((status = brd_adv_appearance(adv_usr_data, length)) != STATUS_SUCCEEDED) {
			DBG_LOG("adding appearance failed");
			return status;
		}

		DBG_LOG("appearance set");
	}
	break;

	default:
	{
		DBG_LOG("invalid advertisement data type");
		return STATUS_INVALID_PARAM;
	}
	break;
	}
	return STATUS_SUCCEEDED;
}

/** @brief initialize the broadcaster event
 * *
 */
static status_t brd_adv_init(void)
{
	status_t status;

	status = brd_set_advertisement_type(ADV_TYPE_SCANNABLE_UNDIRECTED);
	if (status != STATUS_SUCCEEDED) {
		DBG_LOG("Advertisement type set failed(%d)", status);
		return STATUS_FAILED;
	}

	/*set advertisement data type */
	status = brd_set_data_type(ADVERTISEMENT_DATA);
	if (status != STATUS_SUCCEEDED) {
		DBG_LOG("Advertisement data type set failed(%d)", status);
		return STATUS_FAILED;
	}

	/* set advertisement data with local name, appearance and complete list
	 * of service uuid16 */
	brd_set_advertisement_data(ADV_DATA_TYPE_COMPLETE_LOCAL_NAME,
			(uint8_t *)BRD_ADV_DATA_NAME_DATA,
			BRD_ADV_DATA_NAME_LEN);
	brd_set_advertisement_data(ADV_DATA_TYPE_APPEARANCE,
			(uint8_t *)BRD_ADV_DATA_APPEARANCE_DATA,
			BRD_ADV_DATA_APPEARANCE_LEN);
	brd_set_advertisement_data(ADV_DATA_TYPE_COMPLETE_SERVICE_UUID16,
			(uint8_t *)BRD_ADV_DATA_UUID_DATA,
			BRD_ADV_DATA_UUID_LEN);
	
	/*set advertisement data type */
	if ((status = brd_set_data_type(SCAN_RESP_DATA)) != STATUS_SUCCEEDED) {
			DBG_LOG("Advertisement data type set failed(%d)", status);
			return STATUS_FAILED;
	}
	/* set Scan response data with manufacturer specific data */
	brd_set_advertisement_data(ADV_DATA_TYPE_MANUFACTURER_DATA,
								(uint8_t *)BRD_ADV_DATA_MANUFACTURER_DATA,
								BRD_ADV_DATA_MANUFACTURER_LEN);
								
	return STATUS_SUCCEEDED;
}

int main(void)
{
	platform_driver_init();
	acquire_sleep_lock();

	/* Initialize serial console */
	serial_console_init();

	DBG_LOG("Initializing Broadcaster Application");

	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);

	/* Intialize LED */
	led_init();

	brd_adv_init();

	/* starting advertisement in broadcast mode */
	brd_start_broadcast();
}
