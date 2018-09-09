/**
 * \file
 *
 * \brief Simple Broadcaster Application
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

/**
 * \mainpage Simple Broadcaster
 * \section Introduction
 * ******************************Introduction ***********************************
 *
 * The Simple Broadcaster application continuously broadcasts the BLE advertisement data over the air.
 * The Simple Broadcaster example application supports 10 advertisement data types. They are listed below:
 *  + Incomplete List of 16-bit Service Class UUIDs
 *  + Complete List of 16-bit Service Class UUIDs 
 *  + Incomplete List of 32-bit Service Class UUIDs 
 *  + Complete List of 32-bit Service Class UUIDs 
 *  + Incomplete List of 128-bit Service Class UUIDs 
 *  + Complete List of 128-bit Service Class UUIDs 
 *  + Shortened Local Name 
 *  + Complete Local Name 
 *  + Appearance 
 *  + Manufacturer Specific Data
 *
 * - Supported Evolution Kit -
 *	+ ATSAML21-XPRO-B + ATBTLC1000 XPRO
 *	+ ATSAMD21-XPRO + ATBTLC1000 XPRO
 *	+ ATSAMG55-XPRO + ATBTLC1000 XPRO
 *	+ ATSAM4S-XPRO + ATBTLC1000 XPRO
 * 
 * - Running the demo -
 *  + 1. Build and flash the binary into supported evaluation board.
 *  + 2. Open the console using TeraTerm or any serial port monitor.
 *  + 3. Press the Reset button.
 *  + 4. Wait for around 10 seconds for the patches to be downloaded device will initialize and start-up.
 *  + 5. The device is now in advertising mode.
 *
 * \section Modules
 * ***************************** MODULES ***************************************** 
 * - BLE Manager - 
 *   + The Event Manager is responsible for handling the following:
 *    + Generic BLE Event Handling:-
 *       + BLE Event Manager handles the events triggered by BLE stack and also responsible 
 *  	 for invoking all registered callbacks for respective events. BLE Manager 
 *  	 handles all GAP related functionality. In addition to that handles multiple connection 
 *  	 instances, Pairing, Encryption, Scanning.
 *    + Handling Multi-role/multi-connection:-
 *  	  + BLE Event Manager is responsible for handling multiple connection instances 
 *  	  and stores bonding information and Keys to retain the bonded device. 
 *  	  BLE Manager is able to identify and remove the device information when pairing/encryption 
 *		  gets failed. In case of multi-role, it handles the state/event handling of both central and peripheral in multiple contexts.
 *    + Controlling the Advertisement data:-
 *  	  + BLE Event Manager is responsible for generating the advertisement and scan response data
 *  	  for BLE profiles/services that are attached with BLE Manager.
 *
 * - BLE Platform Services -
 *  +  Interface Settings -
 *	  + Connect ATBTLC1000 XPRO to SAML21-XPRO-B -> EXT1
 *	  + Connect ATBTLC1000 XPRO to SAMD21-XPRO -> EXT1
 *	  + Connect ATBTLC1000 XPRO to SAMG55-XPRO -> EXT1
 *	  + Connect ATBTLC1000 XPRO to SAM4S-XPRO  -> EXT1
 *  +  Serial Console COM port settings -
 *    + Baudrate 115200
 *	  + Parity None, Stop Bit 1, Start Bit 1
 *	  + No Hardware Handshake
 *	+  6-Wire Mode Connection Setup -
 *    + Pins are 1:1 match with SAML21/D21 Xpro EXT1 Header to BTLC1000 XPro Header
 *	  + UART(No Flow Control)-SAM L21/D21 XPro Pins (Rx-Pin13, Tx-Pin14)
 *	  + UART(With Flow Control)-SAM G55 Xpro Pins (Rx-Pin13, Tx-Pin14, RTS-Pin5, CTS-Pin6, Rx-Pin16, Tx-Pin17)
 *	  + BTLC1000 Wakeup Pin-SAM G55 XPro Pins(Pin4)
 *	  + BTLC1000 Chip Enable Pin-SAM G55 XPro Pins(Pin10)
 *	  + BTLC1000 Vcc Pin-SAM L21/D21/G55 Xpro Pins(Pin20)
 *	  + BTLC1000 GND Pin-SAM L21/D21/G55 Xpro Pins(Pin19)
 *  +  4-Wire Mode Connection setup -
 * 	  + UART(With Flow Control)-SAM L21/D21 XPro Pins (Rx-Pin15, Tx-Pin17, RTS-Pin16, CTS-Pin18)
 * 	  + BTLC1000 Wakeup Pin-SAM L21/D21 XPro Pins (Rx-Pin6)
 * 	  + BTLC1000 Chip Enable Pin-SAM L21/D21 XPro Pins (Rx-Pin4)
 * 	  + UART(With Flow Control)-SAM G55/4S Xpro Pins (Rx-Pin13, Tx-Pin14, RTS-Pin5, CTS-Pin6)
 * 	  + BTLC1000 Wakeup Pin-SAM G55/4S XPro Pins(Pin4)
 * 	  + BTLC1000 Chip Enable Pin-SAM G55/4S XPro Pins(Pin10)
 * 	  + BTLC1000 Vcc Pin-SAM L21/D21/G55/4S Xpro Pins(Pin20)
 * 	  + BTLC1000 GND Pin-SAM L21/D21/G55/4S Xpro Pins(Pin19)
 *
 * \section BluSDK Package
 * ***************************** BluSDK Package *****************************************
 * - BluSDK -
 *		+ http://www.microchip.com/wwwproducts/en/ATBTLC1000?tab=documents
 * - ATBTLC1000 - 
 *		+ http://www.microchip.com/wwwproducts/en/ATBTLC1000 
 * - Developement kit -
 *		+ http://www.microchip.com/wwwproducts/en/ATBTLC1000?tab=tools
 *		+ SAM L21 + BTLC1000 XPro -
 *			+ http://www.microchip.com/developmenttools/productdetails/atbtlc1000-xstk
 *		+ BTLC1000 XPro -
 *				+ http://www.microchip.com/developmenttools/productdetails/atbtlc1000-xpro
 * - Applications - 
 *		+ http://www.microchip.com/devices/ATBTLC1000.aspx?tab=applications
 * - Support and FAQ visit - 
 *		+ <a href="https://www.microchip.com/support/">Microchip Support</a>
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
	} else if (type == SCAN_RESP_DATA && adv_type ==
			ADV_TYPE_SCANNABLE_UNDIRECTED) {
		data_type = SCAN_RESP_DATA;
	} else {
		return STATUS_INVALID_PARAM;
	}

	return STATUS_SUCCESS;
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

	if (at_ble_adv_data_set(adv_data, adv_length, scan_rsp_data,
			scan_length) != AT_BLE_SUCCESS) {
		DBG_LOG("BLE Broadcast data set failed");
		return;
	} else {
		DBG_LOG("BLE Broadcast data set success");
	}

	if ((status
				= at_ble_adv_start((at_ble_adv_type_t)adv_type,
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
 * @return @ref STATUS_SUCCESS adding the list to advertisement data success
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
			adv_data[adv_length++]
				= ADV_DATA_TYPE_COMPLETE_SERVICE_UUID16;
			memcpy((adv_data + adv_length), list_uuid16, length);
			adv_length += length;
		}
	} else if (data_type == SCAN_RESP_DATA) {
		if ((scan_length + length) > (MAX_SCAN_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			scan_rsp_data[scan_length++] = length + ADV_TYPE_LEN;
			scan_rsp_data[scan_length++]
				= ADV_DATA_TYPE_COMPLETE_SERVICE_UUID16;
			memcpy((scan_rsp_data + scan_length), list_uuid16,
					length);
			scan_length += length;
		}
	}

	return STATUS_SUCCESS;
}

/** @brief Set advertisement data with incomplete list of service uuid of 16bits
 *
 * @param[in] list_uuid16 list of service uuid of 16bits
 * @param[in] length length of the list
 * @return @ref STATUS_SUCCESS adding the list to advertisement data success
 * @return @ref STATUS_MAX_LENGTH_REACHED advertisement data does not have
 * enough space to hold the list
 * @return @ref STATUS_INVALID_PARAM parameters passed are invalid
 * @return @ref STATUS_OPERATION_NOT_SUPPORTED trying to send incomplete list on
 * scan response data.
 *
 */
static status_t brd_adv_incomp_list_service_uuid16(uint8_t *list_uuid16,
		uint8_t length)
{
	if (length <= 0 || !list_uuid16) {
		return STATUS_INVALID_PARAM;
	}

	if (data_type == ADVERTISEMENT_DATA) {
		if ((adv_length + length) > (MAX_ADV_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			adv_data[adv_length++] = length + ADV_TYPE_LEN;
			adv_data[adv_length++]
				= ADV_DATA_TYPE_INCOMPLETE_SERVICE_UUID16;
			memcpy((adv_data + adv_length), list_uuid16, length);
			adv_length += length;
		}
	} else if (data_type == SCAN_RESP_DATA) {
		return STATUS_OPERATION_NOT_SUPPORTED;
	}

	return STATUS_SUCCESS;
}

/** @brief Set advertisement data with complete list of service uuid of 32bits
 *
 * @param[in] list_uuid32 list of service uuid of 32bits
 * @param[in] length length of the list
 * @return @ref STATUS_SUCCESS adding the list to advertisement data success
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
			adv_data[adv_length++]
				= ADV_DATA_TYPE_COMPLETE_SERVICE_UUID32;
			memcpy((adv_data + adv_length), list_uuid32, length);
			adv_length += length;
		}
	} else {
		if ((scan_length + length) > (MAX_SCAN_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			scan_rsp_data[scan_length++] = length + ADV_TYPE_LEN;
			scan_rsp_data[scan_length++]
				= ADV_DATA_TYPE_COMPLETE_SERVICE_UUID32;
			memcpy((scan_rsp_data + scan_length), list_uuid32,
					length);
			scan_length += length;
		}
	}

	return STATUS_SUCCESS;
}

/** @brief Set advertisement data with incomplete list of service uuid of 32bits
 *
 * @param[in] list_uuid32 list of service uuid of 32bits
 * @param[in] length length of the list
 * @return @ref STATUS_SUCCESS adding the list to advertisement data success
 * @return @ref STATUS_MAX_LENGTH_REACHED advertisement data does not have
 * enough space to hold the list
 * @return @ref STATUS_INVALID_PARAM parameters passed are invalid
 * @return @ref STATUS_OPERATION_NOT_SUPPORTED trying to send incomplete list on
 * scan response data.
 *
 */
static status_t brd_adv_incomp_list_service_uuid32(uint8_t *list_uuid32,
		uint8_t length)
{
	if (length <= 0 || !list_uuid32) {
		return STATUS_INVALID_PARAM;
	}

	if (data_type == ADVERTISEMENT_DATA) {
		if ((adv_length + length) > (MAX_ADV_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			adv_data[adv_length++] = length + ADV_TYPE_LEN;
			adv_data[adv_length++]
				= ADV_DATA_TYPE_INCOMPLETE_SERVICE_UUID32;
			memcpy((adv_data + adv_length), list_uuid32, length);
			adv_length += length;
		}
	} else {
		return STATUS_OPERATION_NOT_SUPPORTED;
	}

	return STATUS_SUCCESS;
}

/** @brief Set advertisement data with complete list of service uuid of 128bits
 *
 * @param[in] list_uuid128 list of service uuid of 128bits
 * @param[in] length length of the list
 * @return @ref STATUS_SUCCESS adding the list to advertisement data success
 * @return @ref STATUS_MAX_LENGTH_REACHED advertisement data does not have
 * enough space to hold the list
 * @return @ref STATUS_INVALID_PARAM parameters passed are invalid
 *
 */
static status_t brd_adv_comp_list_service_uuid128(uint8_t *list_uuid128,
		uint8_t length)
{
	if (length <= 0 || !list_uuid128) {
		return STATUS_INVALID_PARAM;
	}

	if (data_type == ADVERTISEMENT_DATA) {
		if ((adv_length + length) > (MAX_ADV_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			adv_data[adv_length++] = length + ADV_TYPE_LEN;
			adv_data[adv_length++]
				= ADV_DATA_TYPE_COMPLETE_SERVICE_UUID128;
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

	return STATUS_SUCCESS;
}

/** @brief Set advertisement data with incomplete list of service uuid of 32bits
 *
 * @param[in] list_uuid128 list of service uuid of 128bits
 * @param[in] length length of the list
 * @return @ref STATUS_SUCCESS adding the list to advertisement data success
 * @return @ref STATUS_MAX_LENGTH_REACHED advertisement data does not have
 * enough space to hold the list
 * @return @ref STATUS_INVALID_PARAM parameters passed are invalid
 * @return @ref STATUS_OPERATION_NOT_SUPPORTED trying to send incomplete list on
 * scan response data.
 *
 */
static status_t brd_adv_incomp_list_service_uuid128(uint8_t *list_uuid128,
		uint8_t length)
{
	if (length <= 0 || !list_uuid128) {
		return STATUS_INVALID_PARAM;
	}

	if (data_type == ADVERTISEMENT_DATA) {
		if ((adv_length + length) > (MAX_ADV_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			adv_data[adv_length++] = length + ADV_TYPE_LEN;
			adv_data[adv_length++]
				= ADV_DATA_TYPE_INCOMPLETE_SERVICE_UUID128;
			memcpy((adv_data + adv_length), list_uuid128, length);
			adv_length += length;
		}
	} else {
		return STATUS_OPERATION_NOT_SUPPORTED;
	}

	return STATUS_SUCCESS;
}

/** @brief Set advertisement data with complete local name
 *
 * @param[in] local_name local name of the device
 * @param[in] length length of the local name
 * @return @ref STATUS_SUCCESS adding the complete local name to advertisement
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
			adv_data[adv_length++]
				= ADV_DATA_TYPE_COMPLETE_LOCAL_NAME;
			memcpy((adv_data + adv_length), local_name, length);
			adv_length += length;
		}
	} else {
		if ((scan_length + length) > (MAX_SCAN_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			scan_rsp_data[scan_length++] = length + ADV_TYPE_LEN;
			scan_rsp_data[scan_length++]
				= ADV_DATA_TYPE_COMPLETE_LOCAL_NAME;
			memcpy((scan_rsp_data + scan_length), local_name,
					length);
			scan_length += length;
		}
	}

	return STATUS_SUCCESS;
}

/** @brief Set advertisement data with shortened local name
 *
 * @param[in] local_name local name of the device
 * @param[in] length length of the local name
 * @return @ref STATUS_SUCCESS adding the shortened local name to advertisement
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
			adv_data[adv_length++]
				= ADV_DATA_TYPE_SHORTENED_LOCAL_NAME;
			memcpy((adv_data + adv_length), local_name, length);
			adv_length += length;
		}
	} else {
		return STATUS_OPERATION_NOT_SUPPORTED;
	}

	return STATUS_SUCCESS;
}

/** @brief Set advertisement type
 *
 * @param[in] type type can be ADV_TYPE_SCANNABLE_UNDIRECTED or
 * ADV_TYPE_NONCONN_UNDIRECTED
 * @return @ref STATUS_SUCCESS successfully set advertisement type
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

	return STATUS_SUCCESS;
}
/** @brief Set advertisement data with appearance of the device
 *
 * @param[in] appearance appearance of the device
 * @param[in] length length of the local name
 * @return @ref STATUS_SUCCESS adding the appearance to advertisement data
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
			memcpy((scan_rsp_data + scan_length), appearance,
					length);
			scan_length += length;
		}
	}

	return STATUS_SUCCESS;
}

/** @brief Set advertisement data with manufacturer data
 *
 * @param[in] data manufacturer data of the device
 * @param[in] length length of the local name
 * @return @ref STATUS_SUCCESS adding the manufacturer data to advertisement
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
			adv_data[adv_length++]
				= ADV_DATA_TYPE_MANUFACTURER_DATA;
			memcpy((adv_data + adv_length), data, length);
			adv_length += length;
		}
	} else {
		if ((scan_length + length) > (MAX_SCAN_LEN - 2)) {
			return STATUS_MAX_LENGTH_REACHED;
		} else {
			scan_rsp_data[scan_length++] = length + ADV_TYPE_LEN;
			scan_rsp_data[scan_length++]
				= ADV_DATA_TYPE_MANUFACTURER_DATA;
			memcpy((scan_rsp_data + scan_length), data, length);
			scan_length += length;
		}
	}

	return STATUS_SUCCESS;
}

/** @brief Set advertisement data
 *
 * @param[in] adv_data_type type of the advertisement data type
 * @param[in] adv_usr_data reference to a user data corresponding to the
 * advertisement data type
 * @return @ref STATUS_SUCCESS successfully added user data to the advertisement
 * data
 * @return @ref STATUS_INVALID_PARAM parameters passed are invalid
 * @return @ref STATUS_MAX_LENGTH_REACHED advertisement data does not have
 * enough space to hold the user data
 * @return @ref STATUS_OPERATION_NOT_SUPPORTED trying to add data in unmatched
 * advertisement type
 *
 */
static status_t brd_set_advertisement_data(adv_data_type_t adv_data_type,
		uint8_t *adv_usr_data, uint8_t length)
{
	status_t status;

	switch (adv_data_type) {
	case ADV_DATA_TYPE_INCOMPLETE_SERVICE_UUID16:
	{
		if ((status
					= brd_adv_incomp_list_service_uuid16(
						adv_usr_data,
						length)) != STATUS_SUCCESS) {
			DBG_LOG(
					"adding incomplete list of service uuid16 failed");
			return status;
		}

		DBG_LOG("incomplete list of service uuid16 set");
	}
	break;

	case ADV_DATA_TYPE_INCOMPLETE_SERVICE_UUID32:
	{
		if ((status
					= brd_adv_incomp_list_service_uuid32(
						adv_usr_data,
						length)) != STATUS_SUCCESS) {
			DBG_LOG(
					"adding incomplete list of service uuid32 failed");
			return status;
		}

		DBG_LOG("incomplete list of service uuid32 set");
	}
	break;

	case ADV_DATA_TYPE_INCOMPLETE_SERVICE_UUID128:
	{
		if ((status
					= brd_adv_incomp_list_service_uuid128(
						adv_usr_data,
						length)) != STATUS_SUCCESS) {
			DBG_LOG(
					"adding incomplete list of service uuid128 failed");
			return status;
		}

		DBG_LOG("incomplete list of service uuid128 set");
	}
	break;

	case ADV_DATA_TYPE_COMPLETE_SERVICE_UUID16:
	{
		if ((status
					= brd_adv_comp_list_service_uuid16(
						adv_usr_data,
						length)) != STATUS_SUCCESS) {
			DBG_LOG("adding complete list of service uuid16 failed");
			return status;
		}

		DBG_LOG("complete list of service uuid16 set");
	}
	break;

	case ADV_DATA_TYPE_COMPLETE_SERVICE_UUID32:
	{
		DBG_LOG("name set");
		if ((status
					= brd_adv_comp_list_service_uuid32(
						adv_usr_data,
						length)) != STATUS_SUCCESS) {
			DBG_LOG("adding complete list of service uuid32 failed");
			return status;
		}

		DBG_LOG("complete list of service uuid32 set");
	}
	break;

	case ADV_DATA_TYPE_COMPLETE_SERVICE_UUID128:
	{
		if ((status
					= brd_adv_comp_list_service_uuid128(
						adv_usr_data,
						length)) != STATUS_SUCCESS) {
			DBG_LOG("adding complete list of service uuid128 failed");
			return status;
		}

		DBG_LOG("complete list of service uuid128 set");
	}
	break;

	case ADV_DATA_TYPE_COMPLETE_LOCAL_NAME:
	{
		if ((status
					= brd_adv_comp_local_name(adv_usr_data,
						length)) != STATUS_SUCCESS) {
			DBG_LOG("adding complete local name failed");
			return status;
		}

		DBG_LOG("complete name set");
	}
	break;

	case ADV_DATA_TYPE_SHORTENED_LOCAL_NAME:
	{
		if ((status
					= brd_adv_shortened_local_name(
						adv_usr_data,
						length)) != STATUS_SUCCESS) {
			DBG_LOG("adding shortened complete local name failed");
			return status;
		}

		DBG_LOG("shortened name set");
	}
	break;

	case ADV_DATA_TYPE_MANUFACTURER_DATA:
	{
		if ((status
					= brd_adv_manufacturer_data(adv_usr_data,
						length)) != STATUS_SUCCESS) {
			DBG_LOG("adding manufacturer data failed");
			return status;
		}

		DBG_LOG("manufacturer data set");
	}
	break;

	case ADV_DATA_TYPE_APPEARANCE:
	{
		if ((status
					= brd_adv_appearance(adv_usr_data,
						length)) != STATUS_SUCCESS) {
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
	return STATUS_SUCCESS;
}

/** @brief initialize the broadcaster event
 * *
 */
static status_t brd_adv_init(void)
{
	status_t status;

	if ((status
				= brd_set_advertisement_type(
					ADV_TYPE_SCANNABLE_UNDIRECTED)) !=
			STATUS_SUCCESS) {
		DBG_LOG("Advertisement type set failed(%d)", status);
		return STATUS_FAILED;
	}

	/*set advertisement data type */
	if ((status = brd_set_data_type(ADVERTISEMENT_DATA)) !=
			STATUS_SUCCESS) {
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
	if ((status = brd_set_data_type(SCAN_RESP_DATA)) != STATUS_SUCCESS) {
			DBG_LOG("Advertisement data type set failed(%d)", status);
			return STATUS_FAILED;
	}
	/* set Scan response data with manufacturer specific data */
	brd_set_advertisement_data(ADV_DATA_TYPE_MANUFACTURER_DATA,
								(uint8_t *)BRD_ADV_DATA_MANUFACTURER_DATA,
								BRD_ADV_DATA_MANUFACTURER_LEN);
								
	return STATUS_SUCCESS;
}

int main(void)
{
	#if SAMG55 || SAM4S
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();
	#elif SAM0
	system_init();
	#endif

	/* Initialize serial console */
	serial_console_init();

	DBG_LOG("Initializing Broadcaster Application");
	
	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);

	brd_adv_init();

	/* starting advertisement in broadcast mode */
	brd_start_broadcast();
}
